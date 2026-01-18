#ifndef HERO_VS_ENEMY_REAL_HPP
#define HERO_VS_ENEMY_REAL_HPP

#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <set>
#include "generate_maze.hpp"
#include "dijkstra_search.hpp"

// Глобальная переменная - стартовая позиция врага
extern std::pair<int, int> enemy_start;

// Параметры ИИ
const int ENEMY_VISION_RANGE = 8;        // Радиус видимости врага
const int ENEMY_MEMORY_STEPS = 15;       // Длительность памяти
const int ENEMY_PATROL_RADIUS = 6;       // Радиус патрулирования
const int ENEMY_PREDICTION_DEPTH = 10;   // На сколько шагов вперед предсказывает враг
const int HERO_EVASION_RANGE = 12;       // На каком расстоянии герой замечает врага
const int HERO_BAIT_DISTANCE = 4;        // На каком расстоянии начинать "приманку"

// Предварительные объявления вспомогательных функций
std::pair<int, int> find_bait_location(int hero_x, int hero_y,
    int enemy_x, int enemy_y);
std::pair<int, int> find_evasion_point(int hero_x, int hero_y,
    int enemy_x, int enemy_y,
    int exit_x, int exit_y);

/**
 * Улучшенная структура памяти врага с предсказанием.
 */
struct SmartEnemyMemory
{
    std::pair<int, int> last_seen_position;      // Последняя видимая позиция героя
    std::vector<std::pair<int, int>> predicted_path; // Предсказанный путь героя
    int steps_since_last_seen;                   // Шагов с момента последнего контакта
    bool has_target;                             // Есть ли текущая цель
    std::pair<int, int> ambush_point;            // Точка для засады
    bool chasing_mode;                           // Режим преследования
    bool bait_mode;                              // Герой пытается заманить?
    int prediction_accuracy;                     // Точность предсказаний (0-100)

    SmartEnemyMemory() : steps_since_last_seen(999), has_target(false),
        chasing_mode(false), bait_mode(false), prediction_accuracy(50)
    {
        last_seen_position = std::make_pair(-1, -1);
        ambush_point = std::make_pair(-1, -1);
    }
};

/**
 * Структура для тактики героя.
 */
struct HeroTactics
{
    enum Strategy
    {
        OPTIMAL_PATH,      // Идти по оптимальному пути
        EVADE_ENEMY,       // Уклоняться от врага
        BAIT_ENEMY,        // Заманивать врага
        ALTERNATIVE_PATH   // Искать альтернативный путь
    };

    Strategy current_strategy;
    std::pair<int, int> last_enemy_position;
    int steps_with_enemy_nearby;
    bool enemy_spotted;
    bool bait_active;

    HeroTactics() : current_strategy(OPTIMAL_PATH),
        steps_with_enemy_nearby(0),
        enemy_spotted(false),
        bait_active(false)
    {
        last_enemy_position = std::make_pair(-1, -1);
    }
};

/**
 * Упрощенный A* для быстрого поиска пути.
 */
std::vector<std::pair<int, int>> find_simple_path(int start_x, int start_y,
    int target_x, int target_y)
{
    struct Node
    {
        int x, y;
        int g, h;
        bool operator>(const Node& other) const
        {
            return (g + h) > (other.g + other.h);
        }
    };

    struct NodeHash
    {
        size_t operator()(const Node& n) const
        {
            return static_cast<size_t>(n.y) * WIDTH + n.x;
        }
    };

    struct NodeEqual
    {
        bool operator()(const Node& a, const Node& b) const
        {
            return a.x == b.x && a.y == b.y;
        }
    };

    std::unordered_map<Node, Node, NodeHash, NodeEqual> came_from;
    std::unordered_map<Node, int, NodeHash, NodeEqual> g_score;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open_set;

    Node start_node = { start_x, start_y, 0,
                       abs(target_x - start_x) + abs(target_y - start_y) };
    g_score[start_node] = 0;
    open_set.push(start_node);

    const int dx[4] = { 0, 1, 0, -1 };
    const int dy[4] = { 1, 0, -1, 0 };

    while (!open_set.empty())
    {
        Node current = open_set.top();
        open_set.pop();

        if (current.x == target_x && current.y == target_y)
        {
            std::vector<std::pair<int, int>> path;
            Node node = current;
            while (true)
            {
                path.push_back(std::make_pair(node.x, node.y));
                auto it = came_from.find(node);
                if (it == came_from.end()) break;
                node = it->second;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (int i = 0; i < 4; ++i)
        {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT) continue;

            bool can_pass = false;
            switch (i)
            {
            case 0: can_pass = !maze[current.y][current.x].south; break;
            case 1: can_pass = !maze[current.y][current.x].east; break;
            case 2: can_pass = !maze[current.y][current.x].north; break;
            case 3: can_pass = !maze[current.y][current.x].west; break;
            }
            if (!can_pass) continue;

            int tentative_g = current.g + 1;
            Node neighbor = { nx, ny, tentative_g,
                             abs(target_x - nx) + abs(target_y - ny) };

            auto it = g_score.find(neighbor);
            if (it == g_score.end() || tentative_g < it->second)
            {
                came_from[neighbor] = current;
                g_score[neighbor] = tentative_g;
                open_set.push(neighbor);
            }
        }
    }
    return std::vector<std::pair<int, int>>();
}

/**
 * Проверка видимости с учетом стен (упрощенный лучевой каст).
 */
bool can_see(int from_x, int from_y, int to_x, int to_y)
{
    // Упрощенная проверка - только расстояние
    int dist = abs(from_x - to_x) + abs(from_y - to_y);
    return dist <= ENEMY_VISION_RANGE;

    // В реальной реализации нужно добавить проверку стен
    // но для простоты оставляем так
}

/**
 * Предсказание пути героя на N шагов вперед.
 * Использует алгоритм Дейкстры для предсказания.
 */
std::vector<std::pair<int, int>> predict_hero_path(
    const std::pair<int, int>& hero_pos,
    const std::pair<int, int>& exit_pos,
    int max_depth = ENEMY_PREDICTION_DEPTH)
{
    // Если герой близко к выходу, предсказываем прямой путь
    if (abs(hero_pos.first - exit_pos.first) +
        abs(hero_pos.second - exit_pos.second) <= max_depth)
    {
        // Ищем кратчайший путь к выходу
        return find_simple_path(hero_pos.first, hero_pos.second,
            exit_pos.first, exit_pos.second);
    }

    // Предсказываем движение к выходу с учетом еды
    // Упрощенная версия: ищем путь к выходу длиной не более max_depth
    std::vector<std::pair<int, int>> path;
    std::pair<int, int> current = hero_pos;

    for (int i = 0; i < max_depth; ++i)
    {
        path.push_back(current);

        // Определяем направление к выходу
        int dx = exit_pos.first - current.first;
        int dy = exit_pos.second - current.second;

        // Выбираем направление с максимальным смещением
        if (abs(dx) > abs(dy))
        {
            if (dx > 0) current.first++;
            else current.first--;
        }
        else
        {
            if (dy > 0) current.second++;
            else current.second--;
        }

        // Проверяем границы и стены
        if (current.first < 0 || current.first >= WIDTH ||
            current.second < 0 || current.second >= HEIGHT)
        {
            break;
        }
    }

    return path;
}

/**
 * Поиск хорошей точки для засады на предсказанном пути.
 */
std::pair<int, int> find_best_ambush_point(
    const std::vector<std::pair<int, int>>& predicted_path,
    const std::pair<int, int>& enemy_pos)
{
    if (predicted_path.empty())
    {
        return enemy_pos;
    }

    // Ищем точку на пути, которая:
    // 1. Достижима для врага
    // 2. Имеет хорошую видимость
    // 3. Находится на 1/3-2/3 пути героя

    size_t start_idx = predicted_path.size() / 3;
    size_t end_idx = 2 * predicted_path.size() / 3;

    std::pair<int, int> best_point = predicted_path[start_idx];
    int best_score = -1000;

    for (size_t i = start_idx; i < end_idx && i < predicted_path.size(); ++i)
    {
        const auto& point = predicted_path[i];

        // Проверяем достижимость
        auto path_to_point = find_simple_path(enemy_pos.first, enemy_pos.second,
            point.first, point.second);
        if (path_to_point.empty()) continue;

        // Оцениваем точку
        int score = 0;

        // Близость к врагу (чем ближе, тем лучше)
        int dist_to_enemy = abs(point.first - enemy_pos.first) +
            abs(point.second - enemy_pos.second);
        score -= dist_to_enemy * 2;

        // Позиция на пути (ближе к середине лучше)
        int position_penalty = abs((int)i - (int)predicted_path.size() / 2);
        score -= position_penalty;

        // Проверяем видимость в окрестностях
        int visible_cells = 0;
        for (int dx = -2; dx <= 2; ++dx)
        {
            for (int dy = -2; dy <= 2; ++dy)
            {
                int nx = point.first + dx;
                int ny = point.second + dy;
                if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT)
                {
                    visible_cells++;
                }
            }
        }
        score += visible_cells / 2;

        if (score > best_score)
        {
            best_score = score;
            best_point = point;
        }
    }

    return best_point;
}

/**
 * Вспомогательная функция для поиска точки приманки.
 */
std::pair<int, int> find_bait_location(int hero_x, int hero_y,
    int enemy_x, int enemy_y)
{
    // Ищем тупик или петлю поблизости
    for (int dx = -5; dx <= 5; ++dx)
    {
        for (int dy = -5; dy <= 5; ++dy)
        {
            int tx = hero_x + dx;
            int ty = hero_y + dy;

            if (tx >= 0 && tx < WIDTH && ty >= 0 && ty < HEIGHT)
            {
                // Проверяем, является ли клетка тупиком
                int exits = 0;
                if (!maze[ty][tx].north) exits++;
                if (!maze[ty][tx].south) exits++;
                if (!maze[ty][tx].east) exits++;
                if (!maze[ty][tx].west) exits++;

                if (exits == 1)
                {
                    // Тупик - идеально для приманки
                    return std::make_pair(tx, ty);
                }
            }
        }
    }

    // Если тупик не найден, возвращаем точку в сторону от выхода
    return std::make_pair(std::max(0, hero_x - 3), std::max(0, hero_y - 3));
}

/**
 * Вспомогательная функция для поиска точки уклонения.
 */
std::pair<int, int> find_evasion_point(int hero_x, int hero_y,
    int enemy_x, int enemy_y,
    int exit_x, int exit_y)
{
    // Ищем точку, которая:
    // 1. Дальше от врага
    // 2. Ближе к выходу
    // 3. Достижима

    std::pair<int, int> best_point = std::make_pair(hero_x, hero_y);
    int best_score = -1000;

    for (int dx = -4; dx <= 4; ++dx)
    {
        for (int dy = -4; dy <= 4; ++dy)
        {
            int tx = hero_x + dx;
            int ty = hero_y + dy;

            if (tx >= 0 && tx < WIDTH && ty >= 0 && ty < HEIGHT)
            {
                // Проверяем достижимость
                auto path = find_simple_path(hero_x, hero_y, tx, ty);
                if (path.empty()) continue;

                // Оценка точки
                int score = 0;

                // Дальше от врага лучше
                int dist_to_enemy = abs(tx - enemy_x) + abs(ty - enemy_y);
                score += dist_to_enemy * 2;

                // Ближе к выходу лучше
                int dist_to_exit = abs(tx - exit_x) + abs(ty - exit_y);
                score -= dist_to_exit;

                // Проверяем, не тупик ли
                int exits = 0;
                if (!maze[ty][tx].north) exits++;
                if (!maze[ty][tx].south) exits++;
                if (!maze[ty][tx].east) exits++;
                if (!maze[ty][tx].west) exits++;
                score += exits * 3; // Больше выходов лучше

                if (score > best_score)
                {
                    best_score = score;
                    best_point = std::make_pair(tx, ty);
                }
            }
        }
    }

    return best_point;
}

/**
 * Умное преследование с предсказанием и адаптацией.
 */
std::vector<std::pair<int, int>> smart_enemy_pursuit(
    const std::vector<std::pair<int, int>>& hero_path)
{
    if (hero_path.empty())
    {
        return std::vector<std::pair<int, int>>();
    }

    std::vector<std::pair<int, int>> enemy_path;
    SmartEnemyMemory memory;

    // Начальная позиция врага
    int ex = enemy_start.first;
    int ey = enemy_start.second;
    enemy_path.push_back(std::make_pair(ex, ey));

    // Предсказываем начальный путь героя
    memory.predicted_path = predict_hero_path(
        std::make_pair(0, 0), // Герой начинает в (0,0)
        std::make_pair(WIDTH - 1, HEIGHT - 1)
    );

    // Находим лучшую точку для засады
    memory.ambush_point = find_best_ambush_point(memory.predicted_path,
        std::make_pair(ex, ey));
    memory.chasing_mode = false; // Начинаем в режиме засады

    std::cout << " SMART ENEMY AI ACTIVATED!\n";
    std::cout << " Initial ambush point: ("
        << memory.ambush_point.first << ","
        << memory.ambush_point.second << ")\n";
    std::cout << " Prediction depth: " << ENEMY_PREDICTION_DEPTH << " steps\n\n";

    size_t hero_idx = 0;
    int failed_predictions = 0;
    const int MAX_FAILED_PREDICTIONS = 3;

    while (hero_idx < hero_path.size() && enemy_path.size() < 150)
    {
        // Позиция героя
        int hx = hero_path[std::min(hero_idx, hero_path.size() - 1)].first;
        int hy = hero_path[std::min(hero_idx, hero_path.size() - 1)].second;

        // Проверяем видимость
        bool can_see_hero = can_see(ex, ey, hx, hy);
        int dist_to_hero = abs(ex - hx) + abs(ey - hy);

        if (can_see_hero)
        {
            // Герой виден!
            memory.last_seen_position = std::make_pair(hx, hy);
            memory.steps_since_last_seen = 0;
            memory.chasing_mode = true;
            memory.bait_mode = false;

            std::cout << " Enemy spotted hero at ("
                << hx << "," << hy << "), distance: "
                << dist_to_hero << " cells\n";

            // Непосредственное преследование
            auto pursuit_path = find_simple_path(ex, ey, hx, hy);
            if (!pursuit_path.empty() && pursuit_path.size() > 1)
            {
                ex = pursuit_path[1].first;
                ey = pursuit_path[1].second;
                enemy_path.push_back(std::make_pair(ex, ey));
            }
        }
        else if (memory.chasing_mode)
        {
            // Режим преследования, но герой не виден
            memory.steps_since_last_seen++;

            if (memory.steps_since_last_seen > 5)
            {
                // Потеряли героя, делаем новое предсказание
                std::cout << " Enemy lost sight of hero, making new prediction...\n";

                memory.predicted_path = predict_hero_path(
                    memory.last_seen_position,
                    std::make_pair(WIDTH - 1, HEIGHT - 1)
                );

                memory.ambush_point = find_best_ambush_point(
                    memory.predicted_path,
                    std::make_pair(ex, ey)
                );

                memory.chasing_mode = false; // Переходим в режим засады
                memory.steps_since_last_seen = 0;
                failed_predictions++;

                if (failed_predictions >= MAX_FAILED_PREDICTIONS)
                {
                    std::cout << " Enemy giving up pursuit, going to exit\n";
                    // Идем к выходу
                    auto exit_path = find_simple_path(ex, ey,
                        WIDTH - 1, HEIGHT - 1);
                    if (!exit_path.empty())
                    {
                        ex = exit_path[1].first;
                        ey = exit_path[1].second;
                        enemy_path.push_back(std::make_pair(ex, ey));
                    }
                    hero_idx++;
                    continue;
                }
            }
            else
            {
                // Продолжаем идти к последней видимой позиции
                auto path_to_last_seen = find_simple_path(
                    ex, ey,
                    memory.last_seen_position.first,
                    memory.last_seen_position.second
                );

                if (!path_to_last_seen.empty() && path_to_last_seen.size() > 1)
                {
                    ex = path_to_last_seen[1].first;
                    ey = path_to_last_seen[1].second;
                    enemy_path.push_back(std::make_pair(ex, ey));
                }
            }
        }
        else
        {
            // Режим засады/патрулирования
            // Проверяем, должен ли герой быть виден по нашему предсказанию
            bool should_see_hero = false;
            for (const auto& pred_point : memory.predicted_path)
            {
                if (can_see(ex, ey, pred_point.first, pred_point.second))
                {
                    should_see_hero = true;
                    break;
                }
            }

            if (should_see_hero && !can_see_hero)
            {
                // Предсказание не сбылось - герой пошел другим путем
                std::cout << " Prediction failed! Hero took different route.\n";

                // Ищем героя в других вероятных местах
                // Проверяем клетки вокруг предсказанного пути
                std::pair<int, int> search_target = memory.ambush_point;
                for (int dx = -3; dx <= 3; ++dx)
                {
                    for (int dy = -3; dy <= 3; ++dy)
                    {
                        int nx = memory.ambush_point.first + dx;
                        int ny = memory.ambush_point.second + dy;
                        if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT)
                        {
                            // Проверяем путь к выходу от этой точки
                            auto path_to_exit = find_simple_path(
                                nx, ny, WIDTH - 1, HEIGHT - 1);
                            if (!path_to_exit.empty() &&
                                path_to_exit.size() < memory.predicted_path.size() + 5)
                            {
                                search_target = std::make_pair(nx, ny);
                            }
                        }
                    }
                }

                // Идем к новой цели поиска
                auto search_path = find_simple_path(ex, ey,
                    search_target.first,
                    search_target.second);
                if (!search_path.empty() && search_path.size() > 1)
                {
                    ex = search_path[1].first;
                    ey = search_path[1].second;
                    enemy_path.push_back(std::make_pair(ex, ey));
                }
            }
            else
            {
                // Идем к точке засады
                auto ambush_path = find_simple_path(ex, ey,
                    memory.ambush_point.first,
                    memory.ambush_point.second);

                if (!ambush_path.empty() && ambush_path.size() > 1)
                {
                    ex = ambush_path[1].first;
                    ey = ambush_path[1].second;
                    enemy_path.push_back(std::make_pair(ex, ey));

                    // Если достигли точки засады, ждем
                    if (ex == memory.ambush_point.first &&
                        ey == memory.ambush_point.second)
                    {
                        // Остаемся на месте (имитация ожидания)
                        enemy_path.push_back(std::make_pair(ex, ey));
                        std::cout << " Enemy waiting at ambush point...\n";
                    }
                }
            }
        }

        // Герой движется вперед
        hero_idx++;

        // Если враг близко, ускоряем героя (драматический эффект)
        if (dist_to_hero < 8 && hero_idx + 1 < hero_path.size())
        {
            hero_idx++;
        }
    }

    std::cout << "\n SMART ENEMY AI FINISHED\n";
    std::cout << " Enemy path length: " << (enemy_path.size() - 1) << " steps\n";
    std::cout << " Failed predictions: " << failed_predictions << "\n";
    std::cout << " FINAL WARNING: Smart enemy is DANGEROUS!\n";

    return enemy_path;
}

/**
 * Умный путь героя с тактикой уклонения.
 */
std::vector<std::pair<int, int>> find_smart_hero_path(
    const std::pair<int, int>& enemy_start_pos)
{
    std::vector<std::pair<int, int>> hero_path;
    HeroTactics tactics;

    // Начальная позиция
    int hx = 0, hy = 0;
    hero_path.push_back(std::make_pair(hx, hy));

    // Сначала находим оптимальный путь
    path.clear();
    bool found = find_shortest_path();

    if (!found || path.empty())
    {
        return std::vector<std::pair<int, int>>();
    }

    std::cout << "🧠 SMART HERO AI ACTIVATED!\n";
    std::cout << " Initial optimal path: " << (path.size() - 1) << " steps\n";

    // Преобразуем путь в вектор для удобства
    std::vector<std::pair<int, int>> optimal_path = path;

    // Симулируем движение с тактикой
    int ex = enemy_start_pos.first;
    int ey = enemy_start_pos.second;

    for (size_t i = 1; i < optimal_path.size() && hero_path.size() < 200; ++i)
    {
        int next_hx = optimal_path[i].first;
        int next_hy = optimal_path[i].second;

        // Проверяем, виден ли враг
        int dist_to_enemy = abs(next_hx - ex) + abs(next_hy - ey);
        bool enemy_visible = (dist_to_enemy <= HERO_EVASION_RANGE);

        if (enemy_visible)
        {
            tactics.enemy_spotted = true;
            tactics.last_enemy_position = std::make_pair(ex, ey);
            tactics.steps_with_enemy_nearby++;

            std::cout << " Hero sees enemy at (" << ex << "," << ey
                << "), distance: " << dist_to_enemy << " cells\n";

            if (dist_to_enemy <= HERO_BAIT_DISTANCE && !tactics.bait_active)
            {
                // Враг очень близко - активируем тактику "приманки"
                std::cout << "  BAIT TACTIC ACTIVATED! Leading enemy to dead end\n";

                tactics.current_strategy = HeroTactics::BAIT_ENEMY;
                tactics.bait_active = true;

                // Ищем тупик или петлю, чтобы заманить врага
                std::pair<int, int> bait_target = find_bait_location(
                    hx, hy, ex, ey);

                // Идем к точке приманки
                auto bait_path = find_simple_path(hx, hy,
                    bait_target.first,
                    bait_target.second);

                for (size_t j = 1; j < bait_path.size() && j < 5; ++j)
                {
                    hero_path.push_back(bait_path[j]);
                    hx = bait_path[j].first;
                    hy = bait_path[j].second;
                }

                // После приманки возвращаемся к выходу
                tactics.current_strategy = HeroTactics::ALTERNATIVE_PATH;
                continue;
            }
            else if (dist_to_enemy <= ENEMY_VISION_RANGE + 2)
            {
                // Враг в зоне опасности - ищем обходной путь
                std::cout << "  EVASION TACTIC: Finding alternative route\n";

                tactics.current_strategy = HeroTactics::EVADE_ENEMY;

                // Ищем путь, который избегает врага
                std::pair<int, int> evasion_target = find_evasion_point(
                    hx, hy, ex, ey, WIDTH - 1, HEIGHT - 1);

                auto evasion_path = find_simple_path(hx, hy,
                    evasion_target.first,
                    evasion_target.second);

                if (!evasion_path.empty() && evasion_path.size() > 1)
                {
                    hx = evasion_path[1].first;
                    hy = evasion_path[1].second;
                    hero_path.push_back(std::make_pair(hx, hy));
                    continue;
                }
            }
        }
        else
        {
            tactics.steps_with_enemy_nearby = 0;
        }

        // Если враг не виден или далеко, продолжаем по оптимальному пути
        if (tactics.current_strategy == HeroTactics::OPTIMAL_PATH ||
            tactics.steps_with_enemy_nearby == 0)
        {
            hx = next_hx;
            hy = next_hy;
            hero_path.push_back(std::make_pair(hx, hy));
        }

        // Обновляем позицию врага (в реальной симуляции это было бы отдельно)
        // Здесь для простоты двигаем врага к герою
        if (i % 3 == 0)
        {
            auto enemy_move = find_simple_path(ex, ey, hx, hy);
            if (!enemy_move.empty() && enemy_move.size() > 1)
            {
                ex = enemy_move[1].first;
                ey = enemy_move[1].second;
            }
        }
    }

    std::cout << "\n SMART HERO FINISHED\n";
    std::cout << " Hero path length: " << (hero_path.size() - 1) << " steps\n";
    std::cout << " Enemy spotted: " << (tactics.enemy_spotted ? "YES" : "NO") << "\n";
    std::cout << " Bait tactic used: " << (tactics.bait_active ? "YES" : "NO") << "\n";

    return hero_path;
}

/**
 * Основная функция с умным ИИ для обоих.
 */
std::vector<std::vector<std::pair<int, int>>> find_hero_vs_enemy_paths()
{
    std::cout << "\n=== SMART HERO VS ENEMY: MIND GAME ===\n";
    std::cout << "Hero: Adaptive tactics with bait & evasion\n";
    std::cout << "Enemy: Predictive AI with learning\n";
    std::cout << " Vision range: " << ENEMY_VISION_RANGE << " cells\n";
    std::cout << " Prediction depth: " << ENEMY_PREDICTION_DEPTH << " steps\n";
    std::cout << " Hero evasion range: " << HERO_EVASION_RANGE << " cells\n";
    std::cout << " COLLISION = INSTANT DEFEAT\n\n";

    srand(static_cast<unsigned int>(time(nullptr)));

    // Генерация позиции врага
    int ex, ey;
    int attempts = 0;
    do
    {
        ex = 5 + rand() % 10;
        ey = 5 + rand() % 10;
        attempts++;
    } while ((abs(ex - 0) + abs(ey - 0)) < 5 || attempts < 10);

    enemy_start = std::make_pair(ex, ey);

    std::cout << " SMART ENEMY SPAWNED AT (" << ex << ", " << ey << ")\n";
    std::cout << " Distance from hero: " << (abs(ex - 0) + abs(ey - 0))
        << " cells\n\n";

    // Умный путь героя
    std::cout << "Hero planning adaptive route...\n";
    std::vector<std::pair<int, int>> hero_path = find_smart_hero_path(
        std::make_pair(ex, ey));

    if (hero_path.empty())
    {
        std::cout << "HERO TRAPPED! No escape possible.\n";
        return std::vector<std::vector<std::pair<int, int>>>();
    }

    // Умное преследование врага
    std::cout << "\nEnemy analyzing hero's pattern...\n";
    std::vector<std::pair<int, int>> enemy_path = smart_enemy_pursuit(hero_path);

    if (enemy_path.empty())
    {
        enemy_path.push_back(enemy_start);
    }

    // Анализ столкновений
    bool collision_possible = false;
    std::set<std::pair<int, int>> hero_positions(
        hero_path.begin(), hero_path.end());

    for (const auto& pos : enemy_path)
    {
        if (hero_positions.find(pos) != hero_positions.end())
        {
            collision_possible = true;
            break;
        }
    }

    std::cout << "\n ANALYSIS COMPLETE\n";
    std::cout << " Hero path: " << hero_path.size() - 1 << " steps\n";
    std::cout << " Enemy path: " << enemy_path.size() - 1 << " steps\n";
    std::cout << " Collision possible: "
        << (collision_possible ? "YES ⚠️" : "NO ") << "\n";

    if (collision_possible)
    {
        std::cout << " RISK: Hero must use evasion tactics!\n";
    }

    std::cout << "\n Press SPACE for the ultimate chase!\n";

    std::vector<std::vector<std::pair<int, int>>> result;
    result.push_back(hero_path);
    result.push_back(enemy_path);
    return result;
}

#endif