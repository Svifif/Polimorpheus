#include "generate_maze.hpp"          // Здесь уже есть все константы!
#include "dijkstra_search.hpp"
#include "multi_agent_predictive.hpp"
#include "hero_vs_enemy_real.hpp"       // Panic Mode (оригинальный с багами)
#include "hero_vs_enemy_fixed.hpp"      // Fixed Mode (исправленный)
#include "visualization.hpp"
#include "genetic_evolution.hpp"        
#include <SFML/Graphics.hpp>
#include <iomanip>
#ifdef _WIN32
#include <windows.h>
#endif

// const int WIDTH = 20;
// const int HEIGHT = 20;
// const int CELL_SIZE = 30;
// const int INITIAL_HUNGER = 20;
// const int FOOD_RESTORE = 5;
// const int HUNGER_COST_PER_STEP = 1;
// const int FOOD_COUNT = 8;

// Глобальные переменные, определенные в других файлах
std::vector<std::vector<Cell>> maze(HEIGHT, std::vector<Cell>(WIDTH));
std::vector<std::pair<int, int>> food_locations;
std::pair<int, int> enemy_start;
std::vector<std::pair<int, int>> path;
std::vector<std::vector<std::pair<int, int>>> multi_agent_paths;

/**
 * Функция поиска пути в зависимости от выбранного режима работы.
 */
bool find_path_with_mode(int mode)
{
    std::cout << "\n=== SELECTING MODE " << mode << " ===\n";

    switch (mode)
    {
    case 1:
        std::cout << "Algorithm: Dijkstra with hunger constraints\n";
        return find_shortest_path();

    case 2:
        std::cout << "Algorithm: 2 Agents with mutual prediction\n";
        multi_agent_paths = find_paths_two_agents_predictive();
        return !multi_agent_paths.empty() && !multi_agent_paths[0].empty();

    case 3:


        multi_agent_paths = find_hero_vs_enemy_paths();
        return !multi_agent_paths.empty() && !multi_agent_paths[0].empty();

    case 4:
        std::cout << "Algorithm: Neural Network Pathfinding (Train new)\n";
        path = find_path_weighted_neural(false);
        return !path.empty();

    case 5:
        std::cout << "Algorithm: Neural Network (Load trained)\n";
        path = find_path_weighted_neural(true);
        return !path.empty();

    case 6:
        std::cout << "Algorithm: HERO VS ENEMY - FIXED MODE \n";
        std::cout << " Режим 'Стратегическое противостояние':\n";
        std::cout << "   • Герой использует разумную тактику\n";
        std::cout << "   • НИКАКОЙ ТЕЛЕПОРТАЦИИ!\n";
        std::cout << "   • Реалистичное преследование\n";
        std::cout << "   • Для серьёзных игроков\n\n";

        multi_agent_paths = find_hero_vs_enemy_paths_fixed();
        return !multi_agent_paths.empty() && !multi_agent_paths[0].empty();

    default:
        std::cout << "Invalid mode, using Dijkstra (mode 1)\n";
        return find_shortest_path();
    }
}


/**
 * Запуск анимации для одного агента.
 */
void run_single_agent_animation(sf::RenderWindow& window,
    const std::vector<std::vector<Cell>>& maze,
    const std::vector<std::pair<int, int>>& path,
    sf::Font& font)
{
    run_animation(window, maze, path, font);
}

/**
 * Запуск анимации для нескольких агентов.
 */
void run_multi_agent_animation(sf::RenderWindow& window,
    const std::vector<std::vector<Cell>>& maze,
    const std::vector<std::vector<std::pair<int, int>>>& paths,
    sf::Font& font, int current_mode)
{
    // Проверка наличия путей для анимации
    if (paths.empty())
    {
        std::cout << "ERROR: No paths to animate\n";
        return;
    }

    std::cout << "\n=== MULTI-AGENT ANIMATION STARTED ===\n";
    std::cout << "Number of agents: " << paths.size() << "\n";

    // Особые сообщения для разных режимов
    if (current_mode == 3)  // Panic Mode
    {
        std::cout << " MODE: PANIC MODE - PSYCHOLOGICAL THRILLER!\n";
        std::cout << " Hero: Paranoid schizophrenic (YELLOW)\n";
        std::cout << " Enemy: Calm hunter (RED)\n";
        std::cout << " Symptoms: Random teleportation, panic attacks\n";
        std::cout << " Prognosis: Likely self-destruction\n\n";

        std::cout << "=== MEDICAL WARNING ===\n";
        std::cout << "The hero suffers from:\n";
        std::cout << "1. Acute Paranoia\n";
        std::cout << "2. Teleportation Disorder\n";
        std::cout << "3. Strategy Deficiency Syndrome\n";
        std::cout << "4. Chronic Panic Attacks\n";
        std::cout << "========================\n\n";
    }
    else if (current_mode == 6)  // Fixed Mode
    {
        std::cout << " MODE: STRATEGIC PURSUIT - MIND GAME\n";
        std::cout << " Hero: Strategic thinker (GOLD)\n";
        std::cout << " Enemy: Persistent hunter (DARK RED)\n";
        std::cout << " Goal: Outsmart the opponent\n";
        std::cout << "  Rules: Collision = instant defeat\n\n";
    }
    else if (current_mode == 2)
    {
        std::cout << "MODE: Two Agents Cooperation\n";
    }

    std::cout << "Hero starts at: (0, 0)\n";
    std::cout << "Enemy starts at: (" << enemy_start.first << ", " << enemy_start.second << ")\n";

    if (current_mode == 3 || current_mode == 6)
    {
        std::cout << "  COLLISION RULES: Instant defeat for hero!\n";
    }

    for (size_t i = 0; i < paths.size(); i++)
    {
        std::cout << "Agent " << i << " path length: "
            << (paths[i].empty() ? 0 : paths[i].size() - 1) << " steps\n";
    }

    std::cout << "\nPress ESC to stop animation\n";
    std::cout << "Press P to pause/resume\n\n";

    struct AgentAnimationData
    {
        float progress = 0.0f;
        int current_step = 0;
        int current_hunger = INITIAL_HUNGER;
        int total_points = 0;
        int food_eaten = 0;
        bool alive = true;
        bool finished = false;
    };

    std::vector<AgentAnimationData> agents_data(paths.size());

    // Цвета агентов в зависимости от режима
    std::vector<sf::Color> agent_colors;
    if (current_mode == 3)  // Panic Mode
    {
        agent_colors = { sf::Color(255, 255, 0),    // Герой - ярко-жёлтый (паникёр)
                         sf::Color(255, 0, 0) };    // Враг - красный
    }
    else if (current_mode == 6)  // Fixed Mode
    {
        agent_colors = { sf::Color(255, 215, 0),    // Герой - золотой (король)
                         sf::Color(139, 0, 0) };    // Враг - тёмно-красный
    }
    else
    {
        agent_colors = { sf::Color::Yellow, sf::Color::Cyan,
                        sf::Color::Magenta, sf::Color::Green };
    }

    std::vector<bool> food_available(FOOD_COUNT, true);
    std::vector<int> food_eaten_by(FOOD_COUNT, -1);

    // Заголовок таблицы
    std::cout << std::left << std::setw(6) << "STEP";
    std::cout << std::setw(12) << "AGENT";
    std::cout << std::setw(15) << "POSITION";
    std::cout << std::setw(10) << "HUNGER";
    std::cout << std::setw(8) << "FOOD";
    std::cout << std::setw(12) << "POINTS";
    std::cout << "ACTION\n";
    std::cout << std::string(70, '-') << "\n";

    bool animating = true;
    bool paused = false;
    sf::Clock clock;
    bool collision_occurred = false;

    while (animating && window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return;
            }
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    animating = false;
                    std::cout << "\nAnimation stopped by user\n";
                }
                else if (event.key.code == sf::Keyboard::P)
                {
                    paused = !paused;
                    std::cout << (paused ? "\nAnimation PAUSED\n" : "\nAnimation RESUMED\n");
                }
            }
        }

        if (paused) continue;

        float delta_time = clock.restart().asSeconds();

        for (size_t agent_id = 0; agent_id < paths.size(); agent_id++)
        {
            if (paths[agent_id].empty()) continue;

            auto& data = agents_data[agent_id];
            if (!data.alive || data.finished) continue;

            const auto& agent_path = paths[agent_id];
            data.progress += delta_time * 2.0f;

            if (data.progress >= agent_path.size() - 1)
            {
                data.progress = agent_path.size() - 1;
                data.finished = true;
                std::cout << "Agent " << agent_id << " reached destination\n";
            }

            int step = static_cast<int>(data.progress + 0.01f);
            if (step > data.current_step && step < static_cast<int>(agent_path.size()))
            {
                data.current_step = step;
                int cx = agent_path[step].first;
                int cy = agent_path[step].second;
                std::string action = "move";

                // Проверка сбора еды (только для героя в режиме 3 и 6)
                bool ate_food = false;
                if (current_mode != 3 && current_mode != 6 || agent_id == 0)
                {
                    for (size_t i = 0; i < food_locations.size(); ++i)
                    {
                        if (food_available[i] &&
                            food_locations[i].first == cx &&
                            food_locations[i].second == cy)
                        {
                            data.current_hunger += FOOD_RESTORE;
                            data.current_hunger = std::min(data.current_hunger,
                                INITIAL_HUNGER + FOOD_RESTORE * FOOD_COUNT);
                            data.food_eaten++;
                            food_available[i] = false;
                            food_eaten_by[i] = agent_id;
                            ate_food = true;
                            action = "+" + std::to_string(FOOD_RESTORE) + " food";
                            break;
                        }
                    }
                }

                // Движение и голод
                if (step > 0)
                {
                    if (current_mode != 3 && current_mode != 6 || agent_id == 0)
                    {
                        data.current_hunger -= HUNGER_COST_PER_STEP;
                        data.total_points += maze[cy][cx].cost;

                        if (data.current_hunger <= 0)
                        {
                            data.alive = false;
                            action = "DIED";
                            std::cout << "Agent " << agent_id << " died from hunger!\n";
                        }
                    }
                }
                else
                {
                    action = "START";
                }

                // Вывод в таблицу
                std::cout << std::left << std::setw(6) << step;

                if (current_mode == 3)
                {
                    std::cout << std::setw(12) << (agent_id == 0 ? "PANIC_HERO" : "ENEMY");
                }
                else if (current_mode == 6)
                {
                    std::cout << std::setw(12) << (agent_id == 0 ? "HERO" : "ENEMY");
                }
                else
                {
                    std::cout << std::setw(12) << ("Agent " + std::to_string(agent_id));
                }

                std::cout << std::setw(15) << ("(" + std::to_string(cx) + "," + std::to_string(cy) + ")");
                std::cout << std::setw(10) << data.current_hunger;
                std::cout << std::setw(8) << (ate_food ? "YES" : "NO");
                std::cout << std::setw(12) << data.total_points;
                std::cout << action << "\n";
            }
        }

        // Проверка столкновений
        if ((current_mode == 3 || current_mode == 6) &&
            paths.size() >= 2 && !collision_occurred)
        {
            auto& hero = agents_data[0];
            auto& enemy = agents_data[1];

            if (hero.alive && enemy.alive && !hero.finished && !enemy.finished)
            {
                int hero_idx = static_cast<int>(hero.progress);
                int enemy_idx = static_cast<int>(enemy.progress);

                if (hero_idx < paths[0].size() && enemy_idx < paths[1].size())
                {
                    int hx = paths[0][hero_idx].first;
                    int hy = paths[0][hero_idx].second;
                    int ex = paths[1][enemy_idx].first;
                    int ey = paths[1][enemy_idx].second;

                    if (hx == ex && hy == ey)
                    {
                        hero.current_hunger = 0;
                        collision_occurred = true;

                        std::cout << "\n DEADLY COLLISION! \n";
                        std::cout << "Enemy caught hero at position (" << hx << "," << hy << ")\n";

                        hero.alive = false;
                        hero.finished = true;
                    }
                }
            }
        }

        // Проверка завершения
        bool all_finished = true;
        for (size_t i = 0; i < paths.size(); i++)
        {
            if (!paths[i].empty() && agents_data[i].alive && !agents_data[i].finished)
            {
                all_finished = false;
                break;
            }
        }

        if (all_finished)
        {
            animating = false;
            std::cout << "\n=== ANIMATION FINISHED ===\n";

            // Статистика еды
            int total_food_eaten = 0;
            for (size_t i = 0; i < food_locations.size(); ++i)
            {
                if (food_eaten_by[i] != -1) total_food_eaten++;
            }
            std::cout << "Total food eaten: " << total_food_eaten << "/" << FOOD_COUNT << "\n";

            // Итоговая статистика
            for (size_t i = 0; i < paths.size(); i++)
            {
                if (!paths[i].empty())
                {
                    std::string agent_name;
                    if (current_mode == 3)
                    {
                        agent_name = (i == 0) ? "Panic Hero" : "Enemy";
                    }
                    else if (current_mode == 6)
                    {
                        agent_name = (i == 0) ? "Hero" : "Enemy";
                    }
                    else
                    {
                        agent_name = "Agent " + std::to_string(i);
                    }

                    std::cout << agent_name << ": "
                        << (agents_data[i].alive ? "survived" : "died")
                        << ", Points: " << agents_data[i].total_points
                        << ", Food: " << agents_data[i].food_eaten;

                    if (current_mode != 3 && current_mode != 6 || i == 0)
                    {
                        std::cout << ", Final hunger: " << agents_data[i].current_hunger;
                    }
                    std::cout << "\n";
                }
            }

            // Особые сообщения
            if (current_mode == 3)
            {
                std::cout << "\n=== PANIC MODE RESULTS ===\n";
                if (collision_occurred)
                {
                    std::cout << " Hero was caught (as expected with all that teleportation!)\n";
                }
                else if (agents_data[0].alive)
                {
                    std::cout << " Hero survived (despite the panic attacks!)\n";
                }
                else
                {
                    std::cout << "  Hero died from hunger (too busy panicking to eat)\n";
                }
                std::cout << "=========================\n";
            }
            else if (current_mode == 6)
            {
                std::cout << "\n=== STRATEGIC MODE RESULTS ===\n";
                if (collision_occurred)
                {
                    std::cout << "  Enemy victory - better strategy needed!\n";
                }
                else if (agents_data[0].alive)
                {
                    std::cout << " Hero victory - excellent tactics!\n";
                }
                else
                {
                    std::cout << " Hero perished - need more resources!\n";
                }
                std::cout << "============================\n";
            }

            std::cout << "\n";
        }

        // Отрисовка
        window.clear(sf::Color::Black);
        visualize_maze(window, maze, font);

        // Отрисовка путей
        for (size_t i = 0; i < paths.size(); i++)
        {
            if (!paths[i].empty())
            {
                visualize_path(window, i, paths[i]);
            }
        }

        // Отрисовка агентов
        for (size_t i = 0; i < paths.size(); i++)
        {
            if (paths[i].empty() || !agents_data[i].alive) continue;

            const auto& agent_path = paths[i];
            float progress = agents_data[i].progress;

            if (progress < agent_path.size())
            {
                size_t idx = static_cast<size_t>(progress);
                float frac = progress - idx;

                float pos_x = static_cast<float>(agent_path[idx].first * CELL_SIZE + CELL_SIZE / 2);
                float pos_y = static_cast<float>(agent_path[idx].second * CELL_SIZE + CELL_SIZE / 2);

                if (idx + 1 < agent_path.size())
                {
                    pos_x += frac * (agent_path[idx + 1].first - agent_path[idx].first) * CELL_SIZE;
                    pos_y += frac * (agent_path[idx + 1].second - agent_path[idx].second) * CELL_SIZE;
                }

                sf::CircleShape player(10);
                player.setFillColor(agent_colors[i % agent_colors.size()]);
                player.setOutlineColor(sf::Color::Black);
                player.setOutlineThickness(2);
                player.setPosition(pos_x - 10, pos_y - 10);
                window.draw(player);
            }
        }

        window.display();
    }
}

/**
 * Вывод информации об управлении.
 */
void print_controls()
{
    std::cout << "\n=== CONTROLS ===\n";
    std::cout << "1-6: Select algorithm mode\n";
    std::cout << "  1: Single agent - Dijkstra with hunger\n";
    std::cout << "  2: Two agents - Mutual prediction\n";
    std::cout << "  3: Hero vs Enemy - PANIC MODE  (LEGENDARY BUGS!)\n";
    std::cout << "  4: Neural Network - Train new\n";
    std::cout << "  5: Neural Network - Load trained\n";
    std::cout << "  6: Hero vs Enemy - FIXED MODE  (No teleportation)\n";
    std::cout << "R: Regenerate maze\n";
    std::cout << "SPACE: Start animation\n";
    std::cout << "P: Pause/resume animation\n";
    std::cout << "ESC: Stop animation\n";
    std::cout << "C: Show this help\n";
    std::cout << "Q: Quit program\n";
    std::cout << "================\n\n";

    std::cout << "=== SPECIAL MODE 3 INFO ===\n";
    std::cout << " PANIC MODE features:\n";
    std::cout << "   • Hero with psychological issues\n";
    std::cout << "   • Random teleportation episodes\n";
    std::cout << "   • Panic attacks and paranoia\n";
    std::cout << "   • Unpredictable behavior\n";
    std::cout << "   • GUARANTEED ENTERTAINMENT!\n";
    std::cout << "===========================\n\n";
}

/**
 * Основная функция.
 */
int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::cout << "========================================\n";
    std::cout << "     MAZE PATHFINDING SIMULATION 2.0    \n";
    std::cout << "     Featuring: PANIC MODE            \n";
    std::cout << "========================================\n\n";

    print_controls();

    int current_mode = 0;
    bool path_found = false;

    std::cout << "Generating maze...\n";
    generate_maze();

    sf::RenderWindow window(sf::VideoMode(WIDTH * CELL_SIZE, HEIGHT * CELL_SIZE),
        "Maze Pathfinding - Select Mode (1-6)");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        if (!font.loadFromFile("C:/Windows/Fonts/calibri.ttf"))
        {
            std::cout << "Font not loaded, costs won't be displayed\n";
        }
    }

    std::cout << "\nMaze generated. Select mode (1-6) to find path.\n";
    std::cout << "Mode 3 = PANIC MODE (recommended for fun!)\n";
    std::cout << "Mode 6 = Fixed version (for serious players)\n";
    std::cout << "Press C for controls, R to regenerate maze.\n";

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed)
            {
                // Выбор режима 1-6
                if (event.key.code >= sf::Keyboard::Num1 &&
                    event.key.code <= sf::Keyboard::Num6)
                {
                    int new_mode = event.key.code - sf::Keyboard::Num1 + 1;
                    current_mode = new_mode;

                    std::cout << "\n=== MODE " << current_mode << " SELECTED ===\n";
                    path_found = find_path_with_mode(current_mode);

                    if (path_found)
                    {
                        if (current_mode == 2 || current_mode == 3 || current_mode == 6)
                        {
                            std::string mode_name = "";
                            if (current_mode == 3) mode_name = "PANIC";
                            else if (current_mode == 6) mode_name = "STRATEGIC";
                            else mode_name = "Multi-Agent";

                            window.setTitle("Maze - Mode " + std::to_string(current_mode) +
                                " (" + mode_name + ") - Press SPACE");
                            std::cout << mode_name << " ready. Press SPACE to animate.\n";
                        }
                        else
                        {
                            window.setTitle("Maze - Mode " + std::to_string(current_mode) +
                                " - Press SPACE to animate");
                            std::cout << "Path found. Press SPACE to animate.\n";
                        }
                    }
                    else
                    {
                        window.setTitle("Maze - Mode " + std::to_string(current_mode) +
                            " - NO PATH FOUND");
                        std::cout << "No path found. Try different maze (R) or mode.\n";
                    }
                }

                // Перегенерация лабиринта
                if (event.key.code == sf::Keyboard::R)
                {
                    std::cout << "\n=== REGENERATING MAZE ===\n";
                    generate_maze();
                    path.clear();
                    multi_agent_paths.clear();
                    path_found = false;
                    current_mode = 0;
                    window.setTitle("Maze Pathfinding - Select Mode (1-6)");
                    std::cout << "Maze regenerated. Select mode (1-6) to find path.\n";
                }

                // Запуск анимации
                if (event.key.code == sf::Keyboard::Space)
                {
                    if (!path_found)
                    {
                        std::cout << "\nERROR: No path to animate. Select mode first.\n";
                    }
                    else if (current_mode == 2 || current_mode == 3 || current_mode == 6)
                    {
                        if (!multi_agent_paths.empty() && !multi_agent_paths[0].empty())
                        {
                            std::cout << "\nStarting animation...\n";
                            if (current_mode == 3)
                            {
                                std::cout << " MEDICAL WARNING: Patient #001 is unstable!\n";
                                std::cout << "   Keep all sharp objects away from screen.\n\n";
                            }
                            run_multi_agent_animation(window, maze, multi_agent_paths, font, current_mode);
                            std::cout << "Animation finished. Ready for commands.\n";
                        }
                        else
                        {
                            std::cout << "\nERROR: No multi-agent paths found.\n";
                        }
                    }
                    else
                    {
                        if (!path.empty())
                        {
                            std::cout << "\nStarting single-agent animation...\n";
                            run_single_agent_animation(window, maze, path, font);
                            std::cout << "Animation finished. Ready for commands.\n";
                        }
                        else
                        {
                            std::cout << "\nERROR: No path to animate.\n";
                        }
                    }
                }

                // Справка
                if (event.key.code == sf::Keyboard::C)
                {
                    print_controls();
                    std::cout << "Current mode: "
                        << (current_mode > 0 ? std::to_string(current_mode) : "not selected") << "\n";
                    std::cout << "Path found: " << (path_found ? "yes" : "no") << "\n";
                }

                // Выход
                if (event.key.code == sf::Keyboard::Q)
                {
                    std::cout << "\nExiting program...\n";
                    window.close();
                }
            }
        }

        // Отрисовка
        window.clear(sf::Color::Black);
        visualize_maze(window, maze, font);

        if (path_found)
        {
            if ((current_mode == 2 || current_mode == 3 || current_mode == 6) &&
                !multi_agent_paths.empty())
            {
                for (size_t i = 0; i < multi_agent_paths.size(); i++)
                {
                    if (!multi_agent_paths[i].empty())
                    {
                        visualize_path(window, i, multi_agent_paths[i]);
                    }
                }
            }
            else if (!path.empty())
            {
                visualize_path(window, 0, path);
            }
        }

        window.display();
    }

    std::cout << "\n========================================\n";
    std::cout << "        PROGRAM TERMINATED              \n";
    std::cout << "  Thank you for testing PANIC MODE!     \n";
    std::cout << "========================================\n";

    return 0;
}