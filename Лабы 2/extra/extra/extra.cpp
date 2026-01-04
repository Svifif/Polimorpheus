#include "generate_maze.hpp"
#include "dijkstra_search.hpp"
#include "multi_agent_predictive.hpp"
#include "hero_vs_enemy_real.hpp"
#include "visualization.hpp"
#include "genetic_evolution.hpp"  // Теперь здесь нейросеть!
#include <SFML/Graphics.hpp>
#ifdef _WIN32
#include <windows.h>
#endif

// Глобальные переменные, определенные в других файлах
std::vector<std::vector<Cell>> maze(HEIGHT, std::vector<Cell>(WIDTH));
std::vector<std::pair<int, int>> food_locations;
std::pair<int, int> enemy_start;
std::vector<std::pair<int, int>> path;
std::vector<std::vector<std::pair<int, int>>> multi_agent_paths;

/**
 * Функция поиска пути в зависимости от выбранного режима работы.
 *
 * @param mode Режим работы (1-5)
 * @return true если путь(и) найдены успешно, false в противном случае
 *
 * Сложность: Зависит от выбранного алгоритма:
 *  - Режим 1: O(S log S) - алгоритм Дейкстры
 *  - Режим 2: O(2 × A*) - два независимых агента
 *  - Режим 3: O(D + P) - Дейкстра + преследование
 *  - Режим 4,5: O(N) - нейронная сеть (быстрее но требует обучения)
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
        std::cout << "Algorithm: Hero vs Enemy (pursuit mode)\n";
        multi_agent_paths = find_hero_vs_enemy_paths();
        return !multi_agent_paths.empty() && !multi_agent_paths[0].empty();

    case 4:
        // Используем функцию из genetic_evolution.hpp (которая теперь содержит нейросеть)
        std::cout << "Algorithm: Neural Network Pathfinding\n";
        path = find_path_weighted_neural(false); // false = обучить новую
        return !path.empty();

    case 5:
        std::cout << "Algorithm: Neural Network (load trained)\n";
        path = find_path_weighted_neural(true); // true = загрузить сохраненную
        return !path.empty();

    default:
        std::cout << "Invalid mode, using Dijkstra (mode 1)\n";
        return find_shortest_path();
    }
}

/**
 * Запуск анимации для одного агента.
 * Используется в режимах 1, 4, 5.
 *
 * @param window Окно SFML для отрисовки
 * @param maze Лабиринт
 * @param path Путь для анимации
 * @param font Шрифт для отображения информации
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
 * Используется в режимах 2 и 3.
 *
 * @param window Окно SFML для отрисовки
 * @param maze Лабиринт
 * @param paths Вектор путей для каждого агента
 * @param font Шрифт для отображения информации
 * @param current_mode Текущий режим работы (2 или 3)
 *
 * Сложность: O(F × A × P), где F - частота кадров, A - количество агентов, P - длина пути
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

    // Информация о режиме
    if (current_mode == 3)
    {
        std::cout << "MODE: Hero vs Enemy (Yellow = Hero, Red = Enemy)\n";
        std::cout << "Hero starts at: (0, 0)\n";
        std::cout << "Enemy starts at: (" << enemy_start.first << ", " << enemy_start.second << ")\n";
        std::cout << "🚨 COLLISION RULES: Instant defeat for hero!\n";
    }

    // Вывод информации о путях каждого агента
    for (size_t i = 0; i < paths.size(); i++)
    {
        std::cout << "Agent " << i << " path length: "
            << (paths[i].empty() ? 0 : paths[i].size() - 1) << " steps\n";
        if (!paths[i].empty())
        {
            std::cout << "  Starts at: (" << paths[i][0].first << ", " << paths[i][0].second << ")\n";
        }
    }

    std::cout << "Press ESC to stop animation\n\n";

    // Структура данных для хранения состояния анимации каждого агента
    struct AgentAnimationData
    {
        float progress = 0.0f;      // Текущий прогресс анимации (в шагах)
        int current_step = 0;       // Текущий шаг пути
        int current_hunger = INITIAL_HUNGER; // Текущий уровень голода
        int total_points = 0;       // Накопленные очки
        int food_eaten = 0;         // Количество съеденной еды
        bool alive = true;          // Агент жив
        bool finished = false;      // Агент достиг цели
        int start_x = 0;            // Стартовая позиция X
        int start_y = 0;            // Стартовая позиция Y
    };

    // Инициализация данных для каждого агента
    std::vector<AgentAnimationData> agents_data(paths.size());

    // Установка стартовых позиций агентов
    for (size_t i = 0; i < paths.size(); i++)
    {
        if (!paths[i].empty())
        {
            agents_data[i].start_x = paths[i][0].first;
            agents_data[i].start_y = paths[i][0].second;
        }
    }

    // Выбор цветов агентов в зависимости от режима
    std::vector<sf::Color> agent_colors;
    if (current_mode == 3)
    {
        agent_colors = { sf::Color::Yellow, sf::Color::Red }; // Герой и враг
    }
    else
    {
        agent_colors = { sf::Color::Yellow, sf::Color::Cyan,
                        sf::Color::Magenta, sf::Color::Green }; // Разные цвета
    }

    // Состояние еды в лабиринте
    std::vector<bool> food_available(FOOD_COUNT, true);   // Доступна ли еда
    std::vector<int> food_eaten_by(FOOD_COUNT, -1);       // Кто съел еду

    // Заголовок таблицы пошаговой информации
    std::cout << std::left << std::setw(6) << "STEP";
    std::cout << std::setw(8) << "AGENT";
    std::cout << std::setw(12) << "POSITION";
    std::cout << std::setw(10) << "HUNGER";
    std::cout << std::setw(8) << "FOOD";
    std::cout << std::setw(12) << "POINTS";
    std::cout << "ACTION\n";
    std::cout << std::string(70, '-') << "\n";

    // Переменные управления анимацией
    bool animating = true;
    sf::Clock clock;
    bool collision_occurred = false; // Флаг столкновения для режима 3

    // Главный цикл анимации
    while (animating && window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Обработка закрытия окна
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return;
            }
            // Остановка анимации по нажатию ESC
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                animating = false;
                std::cout << "\nAnimation stopped by user\n";
            }
        }

        // Вычисление времени между кадрами
        float delta_time = clock.restart().asSeconds();

        // Обновление состояния всех агентов
        for (size_t agent_id = 0; agent_id < paths.size(); agent_id++)
        {
            if (paths[agent_id].empty()) continue;

            auto& data = agents_data[agent_id];
            if (!data.alive || data.finished) continue;

            const auto& agent_path = paths[agent_id];

            // Увеличение прогресса анимации (скорость 2 шага в секунду)
            data.progress += delta_time * 2.0f;

            // Проверка достижения конечной точки
            if (data.progress >= agent_path.size() - 1)
            {
                data.progress = agent_path.size() - 1;
                data.finished = true;
                std::cout << "Agent " << agent_id << " reached destination\n";
            }

            // Обработка перехода на новый шаг
            int step = static_cast<int>(data.progress + 0.01f);
            if (step > data.current_step && step < static_cast<int>(agent_path.size()))
            {
                data.current_step = step;
                int cx = agent_path[step].first;
                int cy = agent_path[step].second;
                std::string action = "move";

                // Проверка сбора еды (только для героя в режиме 3)
                bool ate_food = false;
                if (current_mode != 3 || agent_id == 0)
                {
                    // Поиск еды в текущей позиции
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
                            action = "+" + std::to_string(FOOD_RESTORE) + " hunger";
                            break;
                        }
                    }
                }

                // Обработка движения (не для начальной позиции)
                if (step > 0)
                {
                    // Для героя в режиме 3 и всех в других режимах
                    if (current_mode != 3 || agent_id == 0)
                    {
                        data.current_hunger -= HUNGER_COST_PER_STEP;
                        data.total_points += maze[cy][cx].cost;

                        // Проверка смерти от голода
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
                    // Начальный шаг
                    action = "START at (" + std::to_string(cx) + "," + std::to_string(cy) + ")";
                }

                // Вывод информации о шаге в таблицу
                std::cout << std::left << std::setw(6) << step;
                std::cout << std::setw(8) << agent_id;
                std::cout << std::setw(12) << ("(" + std::to_string(cx) + "," + std::to_string(cy) + ")");
                std::cout << std::setw(10) << data.current_hunger;
                std::cout << std::setw(8) << (ate_food ? "YES" : "NO");
                std::cout << std::setw(12) << data.total_points;
                std::cout << action << "\n";
            }
        }

        // Проверка столкновений для режима "Герой vs Враг"
        if (current_mode == 3 && paths.size() >= 2 && !collision_occurred)
        {
            auto& hero = agents_data[0];
            auto& enemy = agents_data[1];

            // Проверка, если оба агента живы и не закончили путь
            if (hero.alive && enemy.alive && !hero.finished && !enemy.finished)
            {
                int hero_idx = static_cast<int>(hero.progress);
                int enemy_idx = static_cast<int>(enemy.progress);

                // Проверка что индексы в пределах массива
                if (hero_idx < paths[0].size() && enemy_idx < paths[1].size())
                {
                    int hx = paths[0][hero_idx].first;
                    int hy = paths[0][hero_idx].second;
                    int ex = paths[1][enemy_idx].first;
                    int ey = paths[1][enemy_idx].second;

                    // Проверка совпадения позиций
                    if (hx == ex && hy == ey)
                    {
                        hero.current_hunger = 0;
                        collision_occurred = true;

                        std::cout << "\n⚔️ ⚔️ ⚔️ DEADLY COLLISION! ⚔️ ⚔️ ⚔️\n";
                        std::cout << "Enemy caught hero at position (" << hx << "," << hy << ")\n";

                        hero.alive = false;
                        hero.finished = true;
                    }
                }
            }
        }

        // Проверка завершения анимации (все агенты закончили или умерли)
        bool all_finished = true;
        for (size_t i = 0; i < paths.size(); i++)
        {
            if (!paths[i].empty() && agents_data[i].alive && !agents_data[i].finished)
            {
                all_finished = false;
                break;
            }
        }

        // Если анимация завершена, выводим итоговую статистику
        if (all_finished)
        {
            animating = false;
            std::cout << "\n=== ANIMATION FINISHED ===\n";

            // Сводка по распределению еды
            std::cout << "\nFood distribution:\n";
            int total_food_eaten = 0;
            for (size_t i = 0; i < food_locations.size(); ++i)
            {
                if (food_eaten_by[i] != -1)
                {
                    std::cout << "Food " << i << " at (" << food_locations[i].first
                        << "," << food_locations[i].second << "): eaten by Agent "
                        << food_eaten_by[i] << "\n";
                    total_food_eaten++;
                }
                else
                {
                    std::cout << "Food " << i << " at (" << food_locations[i].first
                        << "," << food_locations[i].second << "): not eaten\n";
                }
            }
            std::cout << "Total food eaten: " << total_food_eaten << "/" << FOOD_COUNT << "\n";

            // Статистика по каждому агенту
            for (size_t i = 0; i < paths.size(); i++)
            {
                if (!paths[i].empty())
                {
                    std::string agent_name;
                    if (current_mode == 3)
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

                    // Вывод голода только для героя в режиме 3 и всех в других режимах
                    if (current_mode != 3 || i == 0)
                    {
                        std::cout << ", Final hunger: " << agents_data[i].current_hunger;
                    }
                    std::cout << "\n";
                }
            }

            // Особый вывод для режима "Герой vs Враг"
            if (current_mode == 3)
            {
                std::cout << "\n=== HERO VS ENEMY RESULTS ===\n";
                if (collision_occurred)
                {
                    std::cout << "⚔️  ENEMY VICTORY! Hero was caught and defeated!\n";
                }
                else
                {
                    std::cout << "🎉 HERO VICTORY! Hero escaped from the enemy!\n";
                }
                std::cout << "Enemy reached destination: "
                    << (agents_data[1].finished ? "YES" : "NO") << "\n";
            }

            std::cout << "===========================\n\n";
        }

        // Отрисовка текущего кадра
        window.clear(sf::Color::Black);
        visualize_maze(window, maze, font);

        // Отрисовка путей всех агентов
        for (size_t i = 0; i < paths.size(); i++)
        {
            if (!paths[i].empty())
            {
                visualize_path(window, i, paths[i]);
            }
        }

        // Отрисовка агентов на их текущих позициях
        for (size_t i = 0; i < paths.size(); i++)
        {
            if (paths[i].empty() || !agents_data[i].alive) continue;

            const auto& agent_path = paths[i];
            float progress = agents_data[i].progress;

            if (progress < agent_path.size())
            {
                size_t idx = static_cast<size_t>(progress);
                float frac = progress - idx;

                // Вычисление текущей позиции с интерполяцией
                float pos_x = static_cast<float>(agent_path[idx].first * CELL_SIZE + CELL_SIZE / 2);
                float pos_y = static_cast<float>(agent_path[idx].second * CELL_SIZE + CELL_SIZE / 2);

                // Интерполяция к следующей точке пути
                if (idx + 1 < agent_path.size())
                {
                    pos_x += frac * (agent_path[idx + 1].first - agent_path[idx].first) * CELL_SIZE;
                    pos_y += frac * (agent_path[idx + 1].second - agent_path[idx].second) * CELL_SIZE;
                }

                // Создание и отрисовка агента
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
 * Вывод информации об управлении программой.
 */
void print_controls()
{
    std::cout << "\n=== CONTROLS ===\n";
    std::cout << "1-5: Select algorithm mode\n";
    std::cout << "  1: Single agent - Dijkstra with hunger\n";
    std::cout << "  2: Two agents - Mutual prediction\n";
    std::cout << "  3: Hero vs Enemy - Deadly pursuit\n";
    std::cout << "  4: Neural Network - Train new\n";
    std::cout << "  5: Neural Network - Load trained\n";
    std::cout << "R: Regenerate maze\n";
    std::cout << "SPACE: Start animation\n";
    std::cout << "ESC: Stop animation\n";
    std::cout << "C: Show this help\n";
    std::cout << "Q: Quit program\n";
    std::cout << "================\n\n";
}

/**
 * Основная функция программы.
 * Управляет генерацией лабиринта, выбором алгоритмов и визуализацией.
 *
 * Сложность: Зависит от выбранного пользователем режима и действий.
 */
int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::cout << "========================================\n";
    std::cout << "        MAZE PATHFINDING SIMULATION     \n";
    std::cout << "========================================\n\n";

    print_controls();

    // Инициализация переменных состояния
    int current_mode = 0;     // Текущий выбранный режим
    bool path_found = false;  // Найден ли путь

    // Генерация начального лабиринта
    std::cout << "Generating maze...\n";
    generate_maze();

    // Создание графического окна
    sf::RenderWindow window(sf::VideoMode(WIDTH * CELL_SIZE, HEIGHT * CELL_SIZE),
        "Maze Pathfinding - Select Mode (1-5)");
    window.setFramerateLimit(60);

    // Загрузка шрифта для отображения стоимости клеток
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        if (!font.loadFromFile("C:/Windows/Fonts/calibri.ttf"))
        {
            std::cout << "Font not loaded, costs won't be displayed\n";
        }
    }

    std::cout << "\nMaze generated. Select mode (1-5) to find path.\n";
    std::cout << "Press C for controls, R to regenerate maze.\n";

    // Главный цикл обработки событий
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Обработка закрытия окна
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            // Обработка нажатий клавиш
            if (event.type == sf::Event::KeyPressed)
            {
                // Выбор режима работы (1-5)
                if (event.key.code >= sf::Keyboard::Num1 &&
                    event.key.code <= sf::Keyboard::Num5)
                {
                    int new_mode = event.key.code - sf::Keyboard::Num1 + 1;
                    current_mode = new_mode;

                    std::cout << "\n=== MODE " << current_mode << " SELECTED ===\n";
                    path_found = find_path_with_mode(current_mode);

                    // Обновление заголовка окна в зависимости от результата
                    if (path_found)
                    {
                        if (current_mode == 2 || current_mode == 3)
                        {
                            window.setTitle("Maze - Mode " + std::to_string(current_mode) +
                                " - Multi-Agent - Press SPACE to animate");
                            std::cout << "Multi-agent ready. Press SPACE to animate.\n";
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
                    window.setTitle("Maze Pathfinding - Select Mode (1-5)");
                    std::cout << "Maze regenerated. Select mode (1-5) to find path.\n";
                }

                // Запуск анимации
                if (event.key.code == sf::Keyboard::Space)
                {
                    if (!path_found)
                    {
                        std::cout << "\nERROR: No path to animate. Select mode (1-5) and find path first.\n";
                    }
                    else if (current_mode == 2 || current_mode == 3)
                    {
                        // Запуск мульти-агентной анимации
                        if (!multi_agent_paths.empty() && !multi_agent_paths[0].empty())
                        {
                            std::cout << "\nStarting multi-agent animation...\n";
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
                        // Запуск одно-агентной анимации
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

                // Вывод справки по управлению
                if (event.key.code == sf::Keyboard::C)
                {
                    print_controls();
                    std::cout << "Current mode: "
                        << (current_mode > 0 ? std::to_string(current_mode) : "not selected") << "\n";
                    std::cout << "Path found: " << (path_found ? "yes" : "no") << "\n";

                    // Дополнительная информация для мульти-агентных режимов
                    if ((current_mode == 2 || current_mode == 3) && path_found)
                    {
                        for (size_t i = 0; i < multi_agent_paths.size(); i++)
                        {
                            std::cout << "Agent " << i << " path length: "
                                << (multi_agent_paths[i].empty() ? 0 : multi_agent_paths[i].size() - 1)
                                << ", starts at: (" << multi_agent_paths[i][0].first
                                << ", " << multi_agent_paths[i][0].second << ")\n";
                        }
                    }
                }

                // Выход из программы
                if (event.key.code == sf::Keyboard::Q)
                {
                    std::cout << "\nExiting program...\n";
                    window.close();
                }
            }
        }

        // Отрисовка текущего состояния
        window.clear(sf::Color::Black);
        visualize_maze(window, maze, font);

        // Отрисовка пути в зависимости от режима
        if (path_found)
        {
            if ((current_mode == 2 || current_mode == 3) && !multi_agent_paths.empty())
            {
                // Отрисовка путей всех агентов
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
                // Отрисовка пути одного агента
                visualize_path(window, 0, path);
            }
        }

        window.display();
    }

    std::cout << "\n========================================\n";
    std::cout << "        PROGRAM TERMINATED              \n";
    std::cout << "========================================\n";

    return 0;
}