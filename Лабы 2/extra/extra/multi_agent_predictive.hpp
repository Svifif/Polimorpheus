#ifndef MULTI_AGENT_PREDICTIVE_HPP
#define MULTI_AGENT_PREDICTIVE_HPP

#include <vector>
#include <queue>
#include <limits>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <bitset>

#include "generate_maze.hpp"

/**
 * Реализация алгоритма A* для поиска пути с учетом голода и еды.
 * Сложность: O(S log S), где S = W×H×H_max×2^F
 *   W×H - размер лабиринта
 *   H_max - максимальный уровень голода
 *   2^F - все комбинации собранной еды
 *
 * @param start_x, start_y - начальные координаты
 * @param goal_x, goal_y - целевые координаты
 * @return Вектор пар координат, представляющий путь
 */
std::vector<std::pair<int, int>> find_path_a_star(int start_x, int start_y,
    int goal_x, int goal_y)
{
    struct Node
    {
        int x, y;
        int g;      // Стоимость от старта до текущей точки
        int h;      // Эвристическая оценка до цели (манхэттенское расстояние)
        int hunger; // Текущий уровень голода
        unsigned int food_mask; // Маска собранной еды

        // Оператор для приоритетной очереди (min-heap по f = g + h)
        bool operator>(const Node& other) const
        {
            return (g + h) > (other.g + other.h);
        }
    };

    // Хеш-функция для Node (необходима для unordered_map)
    struct NodeHash
    {
        size_t operator()(const Node& n) const
        {
            return ((static_cast<size_t>(n.x) << 20) ^
                (static_cast<size_t>(n.y) << 15) ^
                (static_cast<size_t>(n.hunger) << 7) ^
                n.food_mask);
        }
    };

    // Функция сравнения Node на равенство
    struct NodeEqual
    {
        bool operator()(const Node& a, const Node& b) const
        {
            return a.x == b.x && a.y == b.y &&
                a.hunger == b.hunger &&
                a.food_mask == b.food_mask;
        }
    };

    // Структуры данных для A*
    std::unordered_map<Node, Node, NodeHash, NodeEqual> came_from; // Для восстановления пути
    std::unordered_map<Node, int, NodeHash, NodeEqual> g_score;    // Лучшая известная стоимость
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open_set; // Очередь с приоритетом

    // Вычисляем манхэттенское расстояние от старта до цели
    int manhattan = std::abs(goal_x - start_x) + std::abs(goal_y - start_y);
    Node start{ start_x, start_y, 0, manhattan, INITIAL_HUNGER, 0 };
    g_score[start] = 0;
    open_set.push(start);

    // Направления движения: юг, восток, север, запад
    const int dx[4] = { 0, 1, 0, -1 };
    const int dy[4] = { 1, 0, -1, 0 };

    Node best_final;
    bool found = false;
    int best_cost = std::numeric_limits<int>::max();

    // Основной цикл алгоритма A*
    while (!open_set.empty())
    {
        Node current = open_set.top();
        open_set.pop();

        // Проверяем, достигли ли цели
        if (current.x == goal_x && current.y == goal_y)
        {
            // Сохраняем лучший найденный путь
            if (current.g < best_cost)
            {
                best_cost = current.g;
                best_final = current;
                found = true;
            }
            continue;
        }

        // Исследуем соседние клетки
        for (int i = 0; i < 4; ++i)
        {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            // Проверка границ лабиринта
            if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT) continue;

            // Проверка наличия прохода
            bool can_pass = false;
            switch (i)
            {
            case 0: can_pass = !maze[current.y][current.x].south; break; // Юг
            case 1: can_pass = !maze[current.y][current.x].east;  break; // Восток
            case 2: can_pass = !maze[current.y][current.x].north; break; // Север
            case 3: can_pass = !maze[current.y][current.x].west;  break; // Запад
            }
            if (!can_pass) continue;

            // Уменьшаем голод за шаг
            int new_hunger = current.hunger - HUNGER_COST_PER_STEP;
            // Проверяем, не умрет ли агент от голода
            if (new_hunger <= 0) continue;

            unsigned int new_mask = current.food_mask;

            // Проверяем, есть ли еда в следующей клетке
            for (size_t j = 0; j < food_locations.size(); ++j)
            {
                if (food_locations[j].first == nx && food_locations[j].second == ny &&
                    ((new_mask >> j) & 1) == 0)
                {
                    // Собираем еду и восстанавливаем голод
                    new_hunger += FOOD_RESTORE;
                    new_hunger = std::min(new_hunger, INITIAL_HUNGER + FOOD_RESTORE * FOOD_COUNT);
                    new_mask |= (1u << j);
                }
            }

            // Вычисляем новую стоимость пути
            int tentative_g = current.g + maze[ny][nx].cost;
            int heuristic = std::abs(goal_x - nx) + std::abs(goal_y - ny);

            Node neighbor{ nx, ny, tentative_g, heuristic, new_hunger, new_mask };

            // Проверяем, нашли ли мы лучший путь к этому состоянию
            auto it = g_score.find(neighbor);
            if (it == g_score.end() || tentative_g < it->second)
            {
                came_from[neighbor] = current;
                g_score[neighbor] = tentative_g;
                open_set.push(neighbor);
            }
        }
    }

    // Если путь не найден, возвращаем пустой вектор
    if (!found)
    {
        return {};
    }

    // Восстановление пути от конечной точки к начальной
    std::vector<std::pair<int, int>> path;
    Node node = best_final;
    while (true)
    {
        path.emplace_back(node.x, node.y);
        auto it = came_from.find(node);
        if (it == came_from.end()) break;
        node = it->second;
    }

    // Разворачиваем путь (был собран от конца к началу)
    std::reverse(path.begin(), path.end());
    return path;
}

/**
 * Основная функция для поиска путей двух агентов с конкуренцией за еду.
 * Оба агента независимо ищут оптимальные пути, еда достается тому, кто придет первым.
 *
 * Сложность: O(2 × T), где T - сложность find_path_a_star
 *
 * @return Вектор из двух путей (для первого и второго агента)
 */
std::vector<std::vector<std::pair<int, int>>> find_paths_two_agents_predictive()
{
    std::cout << "\n=== TWO AGENTS: COMPETITIVE PATHFINDING ===\n";
    std::cout << "Both agents search independently. Food goes to whoever arrives first.\n\n";

    // Поиск путей для обоих агентов независимо
    std::cout << "Searching path for Agent 1...\n";
    std::vector<std::pair<int, int>> path1 = find_path_a_star(0, 0, WIDTH - 1, HEIGHT - 1);

    std::cout << "Searching path for Agent 2...\n";
    std::vector<std::pair<int, int>> path2 = find_path_a_star(0, 0, WIDTH - 1, HEIGHT - 1);

    // Проверяем, что оба агента нашли путь
    if (path1.empty() || path2.empty())
    {
        std::cout << "ERROR: At least one agent could not find a path!\n";
        return {};
    }

    std::cout << "\nAgent 1 (Yellow) path length: " << (path1.size() - 1) << " steps\n";
    std::cout << "Agent 2 (Cyan)   path length: " << (path2.size() - 1) << " steps\n\n";

    // Инициализация структур для симуляции конкуренции за еду
    std::vector<bool> food_eaten(FOOD_COUNT, false);         // Флаги съеденности еды
    std::vector<int> agent_food_count(2, 0);                // Количество еды, съеденное каждым агентом
    std::vector<std::string> agent_names = { "Agent 1 (Yellow)", "Agent 2 (Cyan)" };

    // Дополнительная информация для анализа
    std::vector<int> food_eaten_by(FOOD_COUNT, -1);         // Кто съел каждую единицу еды (-1 = никто)
    std::vector<int> food_eaten_at_step(FOOD_COUNT, -1);    // На каком шаге была съедена еда

    std::cout << "=== FOOD COMPETITION SIMULATION ===\n";

    // Симулируем движение агентов по шагам
    size_t max_steps = std::max(path1.size(), path2.size());
    for (size_t t = 0; t < max_steps; ++t)
    {
        // Обрабатываем шаг для первого агента
        if (t < path1.size())
        {
            int x = path1[t].first;
            int y = path1[t].second;
            for (size_t i = 0; i < food_locations.size(); ++i)
            {
                // Если еда еще не съедена и агент находится в клетке с едой
                if (!food_eaten[i] &&
                    food_locations[i].first == x &&
                    food_locations[i].second == y)
                {
                    food_eaten[i] = true;
                    agent_food_count[0]++;
                    food_eaten_by[i] = 0;
                    food_eaten_at_step[i] = (int)t;
                    std::cout << "Step " << t << ": " << agent_names[0]
                        << " eats food at (" << x << "," << y << ")\n";
                }
            }
        }

        // Обрабатываем шаг для второго агента
        if (t < path2.size())
        {
            int x = path2[t].first;
            int y = path2[t].second;
            for (size_t i = 0; i < food_locations.size(); ++i)
            {
                if (!food_eaten[i] &&
                    food_locations[i].first == x &&
                    food_locations[i].second == y)
                {
                    food_eaten[i] = true;
                    agent_food_count[1]++;
                    food_eaten_by[i] = 1;
                    food_eaten_at_step[i] = (int)t;
                    std::cout << "Step " << t << ": " << agent_names[1]
                        << " eats food at (" << x << "," << y << ")\n";
                }
            }
        }
    }

    // Подсчет пересечений путей (исключая стартовую и конечную точки)
    std::vector<bool> positions1(HEIGHT * WIDTH, false);
    for (size_t i = 1; i < path1.size() - 1; ++i)
    {
        int idx = path1[i].second * WIDTH + path1[i].first;
        positions1[idx] = true;
    }

    int intersections = 0;
    for (size_t i = 1; i < path2.size() - 1; ++i)
    {
        int idx = path2[i].second * WIDTH + path2[i].first;
        // Если вторая клетка уже занята первым агентом, считаем пересечение
        if (positions1[idx]) intersections++;
    }

    // Вывод финальной статистики
    std::cout << "\n=== FINAL RESULTS ===\n";
    std::cout << "Agent 1: " << (path1.size() - 1) << " steps, ate " << agent_food_count[0] << " food\n";
    std::cout << "Agent 2: " << (path2.size() - 1) << " steps, ate " << agent_food_count[1] << " food\n";
    std::cout << "Total food eaten: " << (agent_food_count[0] + agent_food_count[1]) << "/" << FOOD_COUNT << "\n";
    std::cout << "Path intersections (internal cells): " << intersections << "\n";

    // Детальная информация о распределении еды
    std::cout << "\nFood details:\n";
    for (size_t i = 0; i < food_locations.size(); ++i)
    {
        std::cout << "  Food at (" << food_locations[i].first << "," << food_locations[i].second << "): ";
        if (food_eaten_by[i] == -1)
        {
            std::cout << "not eaten\n";
        }
        else
        {
            std::cout << "eaten by " << agent_names[food_eaten_by[i]]
                << " at step " << food_eaten_at_step[i] << "\n";
        }
    }

    // Прогноз выживания агентов (точный расчет)
    int hunger1 = INITIAL_HUNGER;
    int hunger2 = INITIAL_HUNGER;

    // Учитываем восстановление голода от съеденной еды
    for (size_t i = 0; i < food_locations.size(); ++i)
    {
        if (food_eaten_by[i] == 0)
        {
            hunger1 += FOOD_RESTORE;
        }
        else if (food_eaten_by[i] == 1)
        {
            hunger2 += FOOD_RESTORE;
        }
    }

    // Отнимаем голод за каждый шаг пути
    hunger1 -= (int)(path1.size() - 1);
    hunger2 -= (int)(path2.size() - 1);

    // Ограничиваем максимальный уровень голода
    hunger1 = std::min(hunger1, INITIAL_HUNGER + FOOD_RESTORE * FOOD_COUNT);
    hunger2 = std::min(hunger2, INITIAL_HUNGER + FOOD_RESTORE * FOOD_COUNT);

    // Вывод прогноза выживания
    std::cout << "\nEstimated final hunger:\n";
    std::cout << " Agent 1: " << hunger1 << " ("
        << (hunger1 > 0 ? "SURVIVES" : "DIES") << ")\n";
    std::cout << " Agent 2: " << hunger2 << " ("
        << (hunger2 > 0 ? "SURVIVES" : "DIES") << ")\n";

    // Возвращаем найденные пути
    std::vector<std::vector<std::pair<int, int>>> result;
    result.push_back(std::move(path1));
    result.push_back(std::move(path2));
    return result;
}

#endif