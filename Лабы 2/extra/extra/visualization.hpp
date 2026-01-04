#ifndef VISUALIZATION_HPP
#define VISUALIZATION_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <iomanip>
#include "generate_maze.hpp"

// Флаги для контроля однократного вывода описаний в консоль
static bool maze_described = false;  // Флаг описания лабиринта
static bool path_described = false;  // Флаг описания пути

/**
 * Сброс флагов описаний.
 * Должен вызываться при перегенерации лабиринта или пути.
 */
void reset_descriptions()
{
    maze_described = false;
    path_described = false;
}

/**
 * Визуализация лабиринта в окне SFML с однократным подробным выводом в консоль.
 * Сложность: O(W×H) - отрисовка каждой клетки лабиринта
 *
 * @param window Окно SFML для отрисовки
 * @param maze Матрица клеток лабиринта
 * @param font Шрифт для отображения стоимости клеток
 */
void visualize_maze(sf::RenderWindow& window, const std::vector<std::vector<Cell>>& maze, sf::Font& font)
{
    // Отрисовка всех клеток лабиринта
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            // Создание прямоугольника для клетки
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            cell.setPosition(static_cast<float>(x * CELL_SIZE), static_cast<float>(y * CELL_SIZE));

            // Выбор цвета клетки в зависимости от её содержимого
            if (maze[y][x].has_food)
            {
                // Коричневый цвет для клеток с едой
                cell.setFillColor(sf::Color(139, 69, 19));
            }
            else if (x == 0 && y == 0)
            {
                // Желтый цвет для стартовой клетки
                cell.setFillColor(sf::Color::Yellow);
            }
            else if (x == WIDTH - 1 && y == HEIGHT - 1)
            {
                // Красный цвет для конечной клетки
                cell.setFillColor(sf::Color::Red);
            }
            else
            {
                // Градиентный цвет в зависимости от стоимости клетки
                int intensity = 255 - (maze[y][x].cost - 1) * 20;
                cell.setFillColor(sf::Color(50, 100, intensity));
            }
            window.draw(cell);

            // Отображение стоимости клетки красным цветом
            sf::Text text(std::to_string(maze[y][x].cost), font, 14);
            text.setFillColor(sf::Color::Red);
            text.setStyle(sf::Text::Bold);

            // Центрирование текста в клетке
            sf::FloatRect bounds = text.getLocalBounds();
            text.setOrigin(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
            text.setPosition(static_cast<float>(x * CELL_SIZE + CELL_SIZE / 2),
                static_cast<float>(y * CELL_SIZE + CELL_SIZE / 2));
            window.draw(text);

            // Отрисовка стен клетки
            sf::RectangleShape wall;
            wall.setFillColor(sf::Color::White);

            // Северная стена
            if (maze[y][x].north)
            {
                wall.setSize(sf::Vector2f(CELL_SIZE, 4));
                wall.setPosition(static_cast<float>(x * CELL_SIZE), static_cast<float>(y * CELL_SIZE));
                window.draw(wall);
            }

            // Южная стена
            if (maze[y][x].south)
            {
                wall.setSize(sf::Vector2f(CELL_SIZE, 4));
                wall.setPosition(static_cast<float>(x * CELL_SIZE), static_cast<float>((y + 1) * CELL_SIZE - 4));
                window.draw(wall);
            }

            // Западная стена
            if (maze[y][x].west)
            {
                wall.setSize(sf::Vector2f(4, CELL_SIZE));
                wall.setPosition(static_cast<float>(x * CELL_SIZE), static_cast<float>(y * CELL_SIZE));
                window.draw(wall);
            }

            // Восточная стена
            if (maze[y][x].east)
            {
                wall.setSize(sf::Vector2f(4, CELL_SIZE));
                wall.setPosition(static_cast<float>((x + 1) * CELL_SIZE - 4), static_cast<float>(y * CELL_SIZE));
                window.draw(wall);
            }
        }
    }

    // Вывод детального описания лабиринта в консоль (только один раз)
    if (!maze_described)
    {
        std::cout << "=== MAZE GENERATED ===\n";
        std::cout << "Size: " << WIDTH << "x" << HEIGHT << "\n";

        // Подсчет и вывод позиций еды
        int food_count = 0;
        std::cout << "Food locations: ";
        for (int y = 0; y < HEIGHT; ++y)
        {
            for (int x = 0; x < WIDTH; ++x)
            {
                if (maze[y][x].has_food)
                {
                    std::cout << "(" << x << "," << y << ") ";
                    food_count++;
                }
            }
        }
        std::cout << "\nTotal food: " << food_count << "\n";

        // Вывод стоимости всех клеток
        std::cout << "Cell costs (1-10):\n";
        for (int y = 0; y < HEIGHT; ++y)
        {
            std::cout << "Row " << y << ": ";
            for (int x = 0; x < WIDTH; ++x)
            {
                std::cout << maze[y][x].cost << " ";
            }
            std::cout << "\n";
        }
        std::cout << "=====================\n\n";

        // Устанавливаем флаг, чтобы описание не выводилось повторно
        maze_described = true;
    }
}

/**
 * Визуализация пути агента в окне SFML с однократным выводом информации в консоль.
 * Сложность: O(P) - отрисовка линий между точками пути
 *
 * @param window Окно SFML для отрисовки
 * @param agent_id Идентификатор агента (определяет цвет пути)
 * @param path Вектор координат точек пути
 */
void visualize_path(sf::RenderWindow& window, int agent_id, const std::vector<std::pair<int, int>>& path)
{
    // Выбор цвета пути в зависимости от идентификатора агента
    sf::Color path_color;
    switch (agent_id % 4)
    {
    case 0: path_color = sf::Color(0, 255, 0, 150); break;   // Зеленый
    case 1: path_color = sf::Color(0, 255, 255, 150); break; // Голубой
    case 2: path_color = sf::Color(255, 0, 255, 150); break; // Пурпурный
    case 3: path_color = sf::Color(255, 255, 0, 150); break; // Желтый
    }

    // Отрисовка линий между последовательными точками пути
    for (size_t i = 1; i < path.size(); ++i)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(
                static_cast<float>(path[i - 1].first * CELL_SIZE + CELL_SIZE / 2),
                static_cast<float>(path[i - 1].second * CELL_SIZE + CELL_SIZE / 2)),
                path_color),
            sf::Vertex(sf::Vector2f(
                static_cast<float>(path[i].first * CELL_SIZE + CELL_SIZE / 2),
                static_cast<float>(path[i].second * CELL_SIZE + CELL_SIZE / 2)),
                path_color)
        };
        window.draw(line, 2, sf::Lines);
    }

    // Вывод информации о пути в консоль (только один раз)
    if (!path_described && !path.empty())
    {
        std::cout << "=== PATH CREATED ===\n";
        std::cout << "Agent ID: " << agent_id << "\n";
        std::cout << "Total steps: " << (path.size() - 1) << "\n";
        std::cout << "Start: (" << path[0].first << "," << path[0].second << ")\n";
        std::cout << "End: (" << path.back().first << "," << path.back().second << ")\n";

        // Проверка достижения выхода
        if (path.back().first == WIDTH - 1 && path.back().second == HEIGHT - 1)
        {
            std::cout << "Status: EXIT REACHED\n";
        }
        else
        {
            std::cout << "Status: EXIT NOT REACHED\n";
        }
        std::cout << "===================\n\n";

        // Устанавливаем флаг, чтобы описание не выводилось повторно
        path_described = true;
    }
}

/**
 * Анимация движения агента по пути с пошаговым выводом информации в консоль.
 * Сложность: O(F×P), где F - частота кадров, P - длина пути
 *
 * @param window Окно SFML для анимации
 * @param maze Матрица клеток лабиринта
 * @param path Путь для анимации
 * @param font Шрифт для отображения информации
 */
void run_animation(sf::RenderWindow& window,
    const std::vector<std::vector<Cell>>& maze,
    const std::vector<std::pair<int, int>>& path,
    sf::Font& font)
{
    // Проверка наличия пути для анимации
    if (path.empty())
    {
        std::cout << "ERROR: No path to animate\n";
        return;
    }

    std::cout << "\n=== ANIMATION STARTED ===\n";
    std::cout << "Agent 0 following path...\n";
    std::cout << "Press ESC to stop animation\n\n";

    // Заголовок таблицы пошаговой информации
    std::cout << std::left << std::setw(6) << "STEP";
    std::cout << std::setw(12) << "POSITION";
    std::cout << std::setw(10) << "HUNGER";
    std::cout << std::setw(8) << "FOOD";
    std::cout << std::setw(12) << "TOTAL POINTS";
    std::cout << "ACTION\n";
    std::cout << std::string(60, '-') << "\n";

    // Переменные состояния анимации
    bool animating = true;
    float progress = 0.0f;               // Текущий прогресс анимации (в шагах)
    int current_step = 0;                // Текущий шаг пути
    int current_hunger = INITIAL_HUNGER; // Текущий уровень голода
    int total_points = 0;                // Накопленные очки (стоимость пройденных клеток)
    unsigned int current_food_mask = 0;  // Маска собранной еды
    int food_eaten = 0;                  // Количество съеденной еды

    sf::Clock clock; // Таймер для управления скоростью анимации

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
            // Обработка нажатия ESC для остановки анимации
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                animating = false;
                std::cout << "\nAnimation stopped by user\n";
            }
        }

        // Вычисление времени, прошедшего с предыдущего кадра
        float delta_time = clock.restart().asSeconds();

        // Обновление прогресса анимации (скорость 2 шага в секунду)
        progress += delta_time * 2.0f;

        // Проверка завершения анимации
        if (progress >= path.size() - 1)
        {
            progress = path.size() - 1;
            animating = false;
            std::cout << "\n=== ANIMATION FINISHED ===\n";
            std::cout << "Total points collected: " << total_points << "\n";
            std::cout << "Final hunger: " << current_hunger << "\n";
            std::cout << "Food eaten: " << food_eaten << "/" << FOOD_COUNT << "\n";
            std::cout << "===========================\n\n";
        }

        // Обработка нового шага пути
        int step = static_cast<int>(progress + 0.01f);
        if (step > current_step && step < static_cast<int>(path.size()))
        {
            current_step = step;
            int cx = path[step].first;
            int cy = path[step].second;
            int cell_cost = maze[cy][cx].cost;
            std::string action = "";

            // Проверка, есть ли в текущей клетке еда
            bool ate_food = false;
            if (maze[cy][cx].has_food)
            {
                int old_hunger = current_hunger;
                current_hunger += FOOD_RESTORE;
                // Ограничение максимального уровня голода
                current_hunger = std::min(current_hunger, INITIAL_HUNGER + FOOD_RESTORE * FOOD_COUNT);
                ate_food = true;
                food_eaten++;
                action = "+" + std::to_string(FOOD_RESTORE) + " hunger (food)";
            }

            // Обработка движения (не для начальной клетки)
            if (step > 0)
            {
                // Уменьшение голода за шаг
                current_hunger -= HUNGER_COST_PER_STEP;
                // Накопление очков за стоимость клетки
                total_points += cell_cost;

                // Проверка смерти от голода
                if (current_hunger <= 0)
                {
                    // Вывод информации о последнем шаге перед смертью
                    std::cout << std::left << std::setw(6) << current_step;
                    std::cout << std::setw(12) << ("(" + std::to_string(cx) + "," + std::to_string(cy) + ")");
                    std::cout << std::setw(10) << current_hunger;
                    std::cout << std::setw(8) << (ate_food ? "YES" : "NO");
                    std::cout << std::setw(12) << total_points;
                    std::cout << "DIED FROM HUNGER\n";

                    animating = false;
                }
                else
                {
                    // Если действие не определено (не было еды), отмечаем как перемещение
                    if (action.empty())
                    {
                        action = "move";
                    }

                    // Вывод информации о текущем шаге
                    std::cout << std::left << std::setw(6) << current_step;
                    std::cout << std::setw(12) << ("(" + std::to_string(cx) + "," + std::to_string(cy) + ")");
                    std::cout << std::setw(10) << current_hunger;
                    std::cout << std::setw(8) << (ate_food ? "YES" : "NO");
                    std::cout << std::setw(12) << total_points;
                    std::cout << action << "\n";
                }
            }
            else
            {
                // Обработка начального шага (шаг 0)
                action = "START";
                std::cout << std::left << std::setw(6) << current_step;
                std::cout << std::setw(12) << ("(" + std::to_string(cx) + "," + std::to_string(cy) + ")");
                std::cout << std::setw(10) << current_hunger;
                std::cout << std::setw(8) << (ate_food ? "YES" : "NO");
                std::cout << std::setw(12) << total_points;
                std::cout << action << "\n";
            }
        }

        // Отрисовка текущего кадра
        window.clear(sf::Color::Black);
        visualize_maze(window, maze, font);
        visualize_path(window, 0, path);

        // Отрисовка агента (интерполяция между точками для плавного движения)
        if (progress < path.size())
        {
            size_t idx = static_cast<size_t>(progress);
            float frac = progress - idx;

            // Вычисление текущей позиции агента с интерполяцией
            float pos_x = static_cast<float>(path[idx].first * CELL_SIZE + CELL_SIZE / 2);
            float pos_y = static_cast<float>(path[idx].second * CELL_SIZE + CELL_SIZE / 2);

            if (idx + 1 < path.size())
            {
                pos_x += frac * (path[idx + 1].first - path[idx].first) * CELL_SIZE;
                pos_y += frac * (path[idx + 1].second - path[idx].second) * CELL_SIZE;
            }

            // Создание и отрисовка круга, представляющего агента
            sf::CircleShape player(13);
            player.setFillColor(sf::Color::Yellow);
            player.setOutlineColor(sf::Color::Black);
            player.setOutlineThickness(2);
            player.setPosition(pos_x - 13, pos_y - 13);
            window.draw(player);
        }

        window.display();
    }
}

#endif