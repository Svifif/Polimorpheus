#ifndef HERO_VS_ENEMY_FIXED_HPP
#define HERO_VS_ENEMY_FIXED_HPP

#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <set>
#include <iomanip>
#include "generate_maze.hpp"
#include "dijkstra_search.hpp"

// Внешние переменные из других файлов
extern std::vector<std::vector<Cell>> maze;
extern std::vector<std::pair<int, int>> food_locations;
extern std::pair<int, int> enemy_start;
extern std::vector<std::pair<int, int>> path;

// Внешние константы (они определены в других файлах)
extern const int WIDTH;
extern const int HEIGHT;

// Уникальные константы для Fixed Mode
const int FIXED_ENEMY_VISION_RANGE = 6;
const int FIXED_ENEMY_MEMORY_STEPS = 10;
const int FIXED_HERO_EVASION_RANGE = 10;
const int FIXED_HERO_BAIT_DISTANCE = 3;

/**
 * Структура памяти врага для Fixed Mode.
 */
struct FixedEnemyMemory
{
    std::pair<int, int> last_seen_position;
    int steps_since_last_seen;
    bool chasing_mode;
    std::pair<int, int> patrol_point;
    int patrol_direction;

    FixedEnemyMemory() : steps_since_last_seen(999), chasing_mode(false), patrol_direction(0)
    {
        last_seen_position = std::make_pair(-1, -1);
        patrol_point = std::make_pair(-1, -1);
    }
};

/**
 * Структура для тактики героя в Fixed Mode.
 */
struct FixedHeroTactics
{
    enum Strategy
    {
        OPTIMAL_PATH_F,
        EVADE_ENEMY_F,
        BAIT_ENEMY_F,
        HIDE_AND_WAIT_F
    };

    Strategy current_strategy;
    std::pair<int, int> last_enemy_position;
    int steps_with_enemy_nearby;
    bool enemy_spotted;
    bool bait_active;
    int strategy_cooldown;

    FixedHeroTactics() : current_strategy(OPTIMAL_PATH_F),
        steps_with_enemy_nearby(0),
        enemy_spotted(false),
        bait_active(false),
        strategy_cooldown(0)
    {
        last_enemy_position = std::make_pair(-1, -1);
    }
};

/**
 * Упрощенный A* для Fixed Mode с ДЕТАЛЬНЫМ ЛОГИРОВАНИЕМ.
 */
std::vector<std::pair<int, int>> find_simple_path_fixed(int start_x, int start_y,
    int target_x, int target_y, bool debug = false)
{
    if (debug)
    {
        std::cout << "\n🔍 PATH DEBUG STARTED =================================\n";
        std::cout << "From: (" << start_x << "," << start_y << ") to ("
            << target_x << "," << target_y << ")\n";
    }

    // Если уже на цели
    if (start_x == target_x && start_y == target_y) {
        if (debug) std::cout << "Already at target, returning single point\n";
        std::vector<std::pair<int, int>> single;
        single.push_back(std::make_pair(start_x, start_y));
        return single;
    }

    struct NodeFixed
    {
        int x, y;
        int g, h, f;
        bool operator>(const NodeFixed& other) const { return f > other.f; }
    };

    const int GRID_SIZE = WIDTH * HEIGHT;
    std::vector<int> came_from(GRID_SIZE, -1);
    std::vector<int> g_score(GRID_SIZE, INT_MAX);
    std::vector<bool> closed_set(GRID_SIZE, false);

    std::priority_queue<NodeFixed, std::vector<NodeFixed>, std::greater<NodeFixed>> open_set;

    auto get_index = [](int x, int y) { return y * WIDTH + x; };

    NodeFixed start_node = { start_x, start_y, 0,
                       abs(target_x - start_x) + abs(target_y - start_y),
                       abs(target_x - start_x) + abs(target_y - start_y) };

    g_score[get_index(start_x, start_y)] = 0;
    open_set.push(start_node);

    if (debug) std::cout << "Pushed start node to open set\n";

    const int dx[4] = { 0, 1, 0, -1 };
    const int dy[4] = { 1, 0, -1, 0 };

    int nodes_expanded = 0;

    while (!open_set.empty())
    {
        NodeFixed current = open_set.top();
        open_set.pop();
        nodes_expanded++;

        int current_idx = get_index(current.x, current.y);
        if (closed_set[current_idx]) continue;
        closed_set[current_idx] = true;

        if (debug && nodes_expanded % 10 == 0)
        {
            std::cout << "  Processing node #" << nodes_expanded
                << " at (" << current.x << "," << current.y
                << ") f=" << current.f << "\n";
        }

        if (current.x == target_x && current.y == target_y)
        {
            if (debug)
            {
                std::cout << "🎯 TARGET REACHED! Reconstructing path...\n";
                std::cout << "Nodes expanded: " << nodes_expanded << "\n";
            }

            std::vector<std::pair<int, int>> path_fixed;
            int x = current.x, y = current.y;
            int steps = 0;

            if (debug) std::cout << "Reconstruction:\n";

            while (x != start_x || y != start_y)
            {
                path_fixed.push_back(std::make_pair(x, y));
                steps++;

                int idx = get_index(x, y);
                int prev_idx = came_from[idx];

                if (prev_idx == -1)
                {
                    if (debug) std::cout << "  ERROR: came_from[" << idx << "] = -1 !\n";
                    break;
                }

                int prev_x = prev_idx % WIDTH;
                int prev_y = prev_idx / WIDTH;

                if (debug && steps <= 10)
                {
                    std::cout << "  Step " << steps << ": (" << x << "," << y
                        << ") <- (" << prev_x << "," << prev_y << ")\n";
                }

                x = prev_x;
                y = prev_y;

                if (steps > GRID_SIZE) // Защита от бесконечного цикла
                {
                    if (debug) std::cout << "  WARNING: Infinite loop detected!\n";
                    break;
                }
            }

            path_fixed.push_back(std::make_pair(start_x, start_y));
            std::reverse(path_fixed.begin(), path_fixed.end());

            if (debug)
            {
                std::cout << "✅ Path reconstructed, length: " << path_fixed.size() << "\n";
                std::cout << "First 5 points: ";
                for (size_t i = 0; i < std::min(path_fixed.size(), size_t(5)); ++i)
                {
                    std::cout << "(" << path_fixed[i].first << "," << path_fixed[i].second << ") ";
                }
                std::cout << "\n";

                // Проверяем последовательность
                bool valid = true;
                for (size_t i = 1; i < path_fixed.size(); ++i)
                {
                    int dist = abs(path_fixed[i].first - path_fixed[i - 1].first) +
                        abs(path_fixed[i].second - path_fixed[i - 1].second);
                    if (dist > 1)
                    {
                        std::cout << "❌ TELEPORTATION DETECTED! Step " << i
                            << ": distance = " << dist << "\n";
                        std::cout << "   From: (" << path_fixed[i - 1].first
                            << "," << path_fixed[i - 1].second << ")\n";
                        std::cout << "   To: (" << path_fixed[i].first
                            << "," << path_fixed[i].second << ")\n";
                        valid = false;
                    }
                }
                if (valid) std::cout << "✅ Path is valid (no teleportation)\n";
                std::cout << "================================================\n";
            }

            return path_fixed;
        }

        for (int i = 0; i < 4; ++i)
        {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT) continue;

            bool can_pass = false;
            switch (i)
            {
            case 0: can_pass = !maze[current.y][current.x].south; break; // ЮГ
            case 1: can_pass = !maze[current.y][current.x].east; break;  // ВОСТОК
            case 2: can_pass = !maze[current.y][current.x].north; break; // СЕВЕР
            case 3: can_pass = !maze[current.y][current.x].west; break;  // ЗАПАД
            }

            if (!can_pass) continue;

            int tentative_g = current.g + 1;
            int neighbor_idx = get_index(nx, ny);

            if (tentative_g < g_score[neighbor_idx])
            {
                came_from[neighbor_idx] = current_idx;
                g_score[neighbor_idx] = tentative_g;

                int h = abs(target_x - nx) + abs(target_y - ny);
                int f = tentative_g + h;

                open_set.push({ nx, ny, tentative_g, h, f });
            }
        }
    }

    if (debug)
    {
        std::cout << "❌ NO PATH FOUND!\n";
        std::cout << "Nodes expanded: " << nodes_expanded << "\n";
        std::cout << "================================================\n";
    }

    return std::vector<std::pair<int, int>>();
}

/**
 * Проверка видимости для Fixed Mode.
 */
bool can_see_fixed(int from_x, int from_y, int to_x, int to_y)
{
    int dist = abs(from_x - to_x) + abs(from_y - to_y);
    return dist <= FIXED_ENEMY_VISION_RANGE;
}

/**
 * Ищет безопасную точку для уклонения с логированием.
 */
std::pair<int, int> find_safe_point_fixed(int hero_x, int hero_y,
    int enemy_x, int enemy_y,
    int exit_x, int exit_y, bool debug = false)
{
    if (debug)
    {
        std::cout << "\n🔍 FIND_SAFE_POINT DEBUG:\n";
        std::cout << "Hero: (" << hero_x << "," << hero_y << ")\n";
        std::cout << "Enemy: (" << enemy_x << "," << enemy_y << ")\n";
        std::cout << "Exit: (" << exit_x << "," << exit_y << ")\n";
    }

    std::pair<int, int> best_point = std::make_pair(hero_x, hero_y);
    int best_score = -10000;
    int points_checked = 0;
    int valid_points = 0;

    const int search_radius = 3;

    for (int dx = -search_radius; dx <= search_radius; ++dx)
    {
        for (int dy = -search_radius; dy <= search_radius; ++dy)
        {
            points_checked++;
            int tx = hero_x + dx;
            int ty = hero_y + dy;

            if (tx < 0 || tx >= WIDTH || ty < 0 || ty >= HEIGHT) continue;

            int dist_to_enemy = abs(tx - enemy_x) + abs(ty - enemy_y);
            if (dist_to_enemy < 3) continue;

            auto path_fixed = find_simple_path_fixed(hero_x, hero_y, tx, ty, debug);
            if (path_fixed.empty() || path_fixed.size() > 6) continue;

            valid_points++;

            int score = 0;
            score += dist_to_enemy * 3;

            int dist_to_exit = abs(tx - exit_x) + abs(ty - exit_y);
            score -= dist_to_exit;

            int dist_from_hero = abs(tx - hero_x) + abs(ty - hero_y);
            score -= dist_from_hero;

            int exits = 0;
            if (!maze[ty][tx].north) exits++;
            if (!maze[ty][tx].south) exits++;
            if (!maze[ty][tx].east) exits++;
            if (!maze[ty][tx].west) exits++;
            score += exits * 2;

            if (score > best_score)
            {
                if (debug)
                {
                    std::cout << "  New best point: (" << tx << "," << ty
                        << ") score: " << score
                        << " (prev best: " << best_score << ")\n";
                }
                best_score = score;
                best_point = std::make_pair(tx, ty);
            }
        }
    }

    if (debug)
    {
        std::cout << "✅ SAFE POINT FOUND:\n";
        std::cout << "   Best point: (" << best_point.first << "," << best_point.second << ")\n";
        std::cout << "   Score: " << best_score << "\n";
        std::cout << "   Points checked: " << points_checked << "\n";
        std::cout << "   Valid points: " << valid_points << "\n";
        std::cout << "================================\n";
    }

    return best_point;
}

/**
 * Ищет точку для приманки.
 */
std::pair<int, int> find_bait_point_fixed(int hero_x, int hero_y,
    int enemy_x, int enemy_y, bool debug = false)
{
    if (debug)
    {
        std::cout << "\n🔍 FIND_BAIT_POINT DEBUG:\n";
        std::cout << "Hero: (" << hero_x << "," << hero_y << ")\n";
        std::cout << "Enemy: (" << enemy_x << "," << enemy_y << ")\n";
    }

    for (int dist = 1; dist <= 4; ++dist)
    {
        for (int dx = -dist; dx <= dist; ++dx)
        {
            for (int dy = -dist; dy <= dist; ++dy)
            {
                if (abs(dx) + abs(dy) != dist) continue;

                int tx = hero_x + dx;
                int ty = hero_y + dy;

                if (tx < 0 || tx >= WIDTH || ty < 0 || ty >= HEIGHT) continue;

                int exits = 0;
                if (!maze[ty][tx].north) exits++;
                if (!maze[ty][tx].south) exits++;
                if (!maze[ty][tx].east) exits++;
                if (!maze[ty][tx].west) exits++;

                if (exits == 1) // Тупик
                {
                    auto path_fixed = find_simple_path_fixed(hero_x, hero_y, tx, ty, debug);
                    if (!path_fixed.empty() && path_fixed.size() <= 6)
                    {
                        if (debug)
                        {
                            std::cout << "✅ BAIT POINT FOUND: (" << tx << "," << ty << ")\n";
                            std::cout << "   Distance: " << dist << "\n";
                            std::cout << "   Path length: " << path_fixed.size() << "\n";
                            std::cout << "================================\n";
                        }
                        return std::make_pair(tx, ty);
                    }
                }
            }
        }
    }

    if (debug)
    {
        std::cout << "❌ NO BAIT POINT FOUND, using fallback\n";
        std::cout << "================================\n";
    }

    return std::make_pair(
        std::max(0, std::min(WIDTH - 1, hero_x + (hero_x < WIDTH / 2 ? 3 : -3))),
        std::max(0, std::min(HEIGHT - 1, hero_y + (hero_y < HEIGHT / 2 ? 3 : -3)))
    );
}

/**
 * Умное преследование для Fixed Mode.
 */
std::vector<std::pair<int, int>> smart_enemy_pursuit_fixed(
    const std::vector<std::pair<int, int>>& hero_path)
{
    if (hero_path.empty()) return {};

    std::vector<std::pair<int, int>> enemy_path;
    FixedEnemyMemory memory;

    int ex = enemy_start.first;
    int ey = enemy_start.second;
    enemy_path.push_back(std::make_pair(ex, ey));

    memory.patrol_point = std::make_pair(
        std::max(0, std::min(WIDTH - 1, ex + (rand() % 7 - 3))),
        std::max(0, std::min(HEIGHT - 1, ey + (rand() % 7 - 3)))
    );

    size_t hero_idx = 0;

    while (hero_idx < hero_path.size() && enemy_path.size() < 100)
    {
        int hx = hero_path[std::min(hero_idx, hero_path.size() - 1)].first;
        int hy = hero_path[std::min(hero_idx, hero_path.size() - 1)].second;

        bool can_see_hero = can_see_fixed(ex, ey, hx, hy);

        if (can_see_hero)
        {
            memory.last_seen_position = std::make_pair(hx, hy);
            memory.steps_since_last_seen = 0;
            memory.chasing_mode = true;

            auto pursuit_path = find_simple_path_fixed(ex, ey, hx, hy, false);
            if (!pursuit_path.empty() && pursuit_path.size() > 1)
            {
                ex = pursuit_path[1].first;
                ey = pursuit_path[1].second;
                enemy_path.push_back(std::make_pair(ex, ey));
            }
        }
        else if (memory.chasing_mode)
        {
            memory.steps_since_last_seen++;

            if (memory.steps_since_last_seen > 8)
            {
                memory.chasing_mode = false;
                memory.steps_since_last_seen = 0;
            }
            else
            {
                auto path_to_last = find_simple_path_fixed(
                    ex, ey,
                    memory.last_seen_position.first,
                    memory.last_seen_position.second, false);

                if (!path_to_last.empty() && path_to_last.size() > 1)
                {
                    ex = path_to_last[1].first;
                    ey = path_to_last[1].second;
                    enemy_path.push_back(std::make_pair(ex, ey));
                }
            }
        }
        else
        {
            auto patrol_path = find_simple_path_fixed(ex, ey,
                memory.patrol_point.first,
                memory.patrol_point.second, false);

            if (!patrol_path.empty() && patrol_path.size() > 1)
            {
                ex = patrol_path[1].first;
                ey = patrol_path[1].second;
                enemy_path.push_back(std::make_pair(ex, ey));

                if (ex == memory.patrol_point.first &&
                    ey == memory.patrol_point.second)
                {
                    memory.patrol_point = std::make_pair(
                        std::max(0, std::min(WIDTH - 1, ex + (rand() % 9 - 4))),
                        std::max(0, std::min(HEIGHT - 1, ey + (rand() % 9 - 4)))
                    );
                }
            }
        }

        hero_idx++;

        int dist = abs(ex - hx) + abs(ey - hy);
        if (dist < 6 && hero_idx + 1 < hero_path.size())
        {
            hero_idx++;
        }
    }

    return enemy_path;
}

/**
 * Умный путь героя для Fixed Mode с ДЕТАЛЬНЫМ ЛОГИРОВАНИЕМ.
 */
std::vector<std::pair<int, int>> find_smart_hero_path_fixed(
    const std::pair<int, int>& enemy_start_pos, bool debug = false)
{
    std::vector<std::pair<int, int>> hero_path;
    FixedHeroTactics tactics;

    int hx = 0, hy = 0;
    hero_path.push_back(std::make_pair(hx, hy));

    // Используем глобальную функцию для поиска пути
    extern bool find_shortest_path();
    path.clear();
    bool found = find_shortest_path();
    if (!found || path.empty()) return {};

    std::vector<std::pair<int, int>> optimal_path = path;

    int ex = enemy_start_pos.first;
    int ey = enemy_start_pos.second;

    std::vector<std::pair<int, int>> bait_path;
    size_t bait_index = 0;

    if (debug)
    {
        std::cout << "\n🧠 HERO AI DEBUG STARTED ================================\n";
        std::cout << "Enemy start: (" << ex << "," << ey << ")\n";
        std::cout << "Optimal path length: " << optimal_path.size() << "\n";
    }

    for (size_t i = 1; i < optimal_path.size() && hero_path.size() < 150; ++i)
    {
        if (tactics.strategy_cooldown > 0) tactics.strategy_cooldown--;

        int next_hx = optimal_path[i].first;
        int next_hy = optimal_path[i].second;

        bool enemy_visible = can_see_fixed(hx, hy, ex, ey);
        int dist_to_enemy = abs(hx - ex) + abs(hy - ey);

        if (debug)
        {
            std::cout << "\n📊 Step " << i << ":\n";
            std::cout << "  Hero position: (" << hx << "," << hy << ")\n";
            std::cout << "  Next optimal: (" << next_hx << "," << next_hy << ")\n";
            std::cout << "  Enemy: (" << ex << "," << ey << ")\n";
            std::cout << "  Enemy visible: " << (enemy_visible ? "YES" : "NO") << "\n";
            std::cout << "  Distance to enemy: " << dist_to_enemy << "\n";
            std::cout << "  Strategy: ";
            switch (tactics.current_strategy)
            {
            case FixedHeroTactics::OPTIMAL_PATH_F: std::cout << "OPTIMAL_PATH"; break;
            case FixedHeroTactics::EVADE_ENEMY_F: std::cout << "EVADE_ENEMY"; break;
            case FixedHeroTactics::BAIT_ENEMY_F: std::cout << "BAIT_ENEMY"; break;
            case FixedHeroTactics::HIDE_AND_WAIT_F: std::cout << "HIDE_AND_WAIT"; break;
            }
            std::cout << "\n  Cooldown: " << tactics.strategy_cooldown << "\n";
        }

        if (enemy_visible)
        {
            tactics.enemy_spotted = true;
            tactics.last_enemy_position = std::make_pair(ex, ey);
            tactics.steps_with_enemy_nearby++;

            if (dist_to_enemy <= FIXED_HERO_BAIT_DISTANCE &&
                !tactics.bait_active &&
                tactics.strategy_cooldown == 0)
            {
                std::cout << "🎣 BAIT TACTIC: Leading enemy to trap\n";
                if (debug) std::cout << "  Enemy too close, activating BAIT mode\n";

                tactics.current_strategy = FixedHeroTactics::BAIT_ENEMY_F;
                tactics.bait_active = true;
                tactics.strategy_cooldown = 15;

                std::pair<int, int> bait_target = find_bait_point_fixed(hx, hy, ex, ey, debug);
                bait_path = find_simple_path_fixed(hx, hy, bait_target.first, bait_target.second, debug);
                bait_index = 1;

                if (!bait_path.empty() && bait_index < bait_path.size())
                {
                    if (debug)
                    {
                        std::cout << "  Bait path found, length: " << bait_path.size() << "\n";
                        std::cout << "  First point: (" << bait_path[0].first << "," << bait_path[0].second << ")\n";
                        if (bait_path.size() > 1)
                            std::cout << "  Second point: (" << bait_path[1].first << "," << bait_path[1].second << ")\n";
                    }

                    // ПРОВЕРКА перед движением!
                    int next_x = bait_path[bait_index].first;
                    int next_y = bait_path[bait_index].second;
                    int dist_to_next = abs(next_x - hx) + abs(next_y - hy);

                    if (dist_to_next <= 1)
                    {
                        hx = next_x;
                        hy = next_y;
                        hero_path.push_back(std::make_pair(hx, hy));
                        bait_index++;
                        if (debug) std::cout << "  Moving to bait point (" << hx << "," << hy << ")\n";
                        continue;
                    }
                    else
                    {
                        std::cout << "  ⚠️ WARNING: Bait point too far (" << dist_to_next
                            << " cells), ignoring bait tactic\n";
                        tactics.bait_active = false;
                    }
                }
                else
                {
                    if (debug) std::cout << "  No bait path found\n";
                    tactics.bait_active = false;
                }
            }
            else if (dist_to_enemy <= FIXED_ENEMY_VISION_RANGE + 2 &&
                tactics.strategy_cooldown == 0)
            {
                std::cout << "🚫 EVASION: Finding safe route\n";
                if (debug) std::cout << "  Enemy in danger zone, activating EVASION\n";

                tactics.current_strategy = FixedHeroTactics::EVADE_ENEMY_F;
                tactics.strategy_cooldown = 8;

                std::pair<int, int> safe_point = find_safe_point_fixed(
                    hx, hy, ex, ey, WIDTH - 1, HEIGHT - 1, debug);

                if (debug)
                {
                    std::cout << "  Safe point found: (" << safe_point.first
                        << "," << safe_point.second << ")\n";
                }

                auto evasion_path = find_simple_path_fixed(hx, hy,
                    safe_point.first, safe_point.second, debug);

                // ВАЖНОЕ ИСПРАВЛЕНИЕ: проверяем весь путь
                if (!evasion_path.empty() && evasion_path.size() > 1)
                {
                    bool found_valid_step = false;

                    // Ищем первую достижимую точку (соседнюю клетку)
                    for (size_t k = 1; k < evasion_path.size(); ++k)
                    {
                        int test_x = evasion_path[k].first;
                        int test_y = evasion_path[k].second;
                        int dist = abs(test_x - hx) + abs(test_y - hy);

                        if (dist == 1) // ТОЛЬКО соседняя клетка!
                        {
                            hx = test_x;
                            hy = test_y;
                            hero_path.push_back(std::make_pair(hx, hy));
                            tactics.strategy_cooldown = 5;
                            found_valid_step = true;

                            if (debug)
                            {
                                std::cout << "  ✅ Moving to adjacent evasion point ("
                                    << hx << "," << hy << ")\n";
                            }
                            break;
                        }
                        else if (debug && k == 1)
                        {
                            std::cout << "  ⚠️ First evasion point is NOT adjacent! Distance: "
                                << dist << " cells\n";
                            std::cout << "    Hero: (" << hx << "," << hy << ")\n";
                            std::cout << "    Point: (" << test_x << "," << test_y << ")\n";
                        }
                    }

                    if (found_valid_step)
                    {
                        continue;
                    }
                    else
                    {
                        std::cout << "  ❌ No adjacent evasion points found, using normal path\n";
                    }
                }
                else
                {
                    if (debug) std::cout << "  No evasion path found\n";
                }
            }
        }
        else
        {
            tactics.steps_with_enemy_nearby = 0;

            if (tactics.current_strategy != FixedHeroTactics::OPTIMAL_PATH_F &&
                tactics.steps_with_enemy_nearby > 5)
            {
                tactics.current_strategy = FixedHeroTactics::OPTIMAL_PATH_F;
                if (debug) std::cout << "  Switching back to OPTIMAL_PATH strategy\n";
            }
        }

        if (tactics.bait_active && !bait_path.empty() && bait_index < bait_path.size())
        {
            int next_x = bait_path[bait_index].first;
            int next_y = bait_path[bait_index].second;
            int dist_to_next = abs(next_x - hx) + abs(next_y - hy);

            if (dist_to_next <= 1) // ТОЛЬКО соседняя клетка!
            {
                hx = next_x;
                hy = next_y;
                hero_path.push_back(std::make_pair(hx, hy));
                bait_index++;

                if (debug)
                {
                    std::cout << "  Following bait path to (" << hx << "," << hy
                        << "), step " << bait_index << "/" << bait_path.size() << "\n";
                }

                if (bait_index >= bait_path.size())
                {
                    tactics.bait_active = false;
                    tactics.current_strategy = FixedHeroTactics::OPTIMAL_PATH_F;
                    if (debug) std::cout << "  Bait path completed\n";
                }
                continue;
            }
            else
            {
                std::cout << "  ⚠️ Bait point too far (" << dist_to_next
                    << " cells), aborting bait tactic\n";
                tactics.bait_active = false;
                tactics.current_strategy = FixedHeroTactics::OPTIMAL_PATH_F;
            }
        }

        // Обычное движение по оптимальному пути
        hx = next_hx;
        hy = next_hy;
        hero_path.push_back(std::make_pair(hx, hy));
        if (debug) std::cout << "  Moving to optimal path point (" << hx << "," << hy << ")\n";

        // Враг движется к герою
        if (i % 4 == 0)
        {
            auto enemy_move = find_simple_path_fixed(ex, ey, hx, hy, false);
            if (!enemy_move.empty() && enemy_move.size() > 1)
            {
                ex = enemy_move[1].first;
                ey = enemy_move[1].second;
                if (debug) std::cout << "  Enemy moves to (" << ex << "," << ey << ")\n";
            }
        }
    }

    if (debug)
    {
        std::cout << "\n✅ HERO AI FINISHED ==================================\n";
        std::cout << "Hero path length: " << hero_path.size() << "\n";
        std::cout << "Final position: (" << hx << "," << hy << ")\n";
        std::cout << "Enemy spotted: " << (tactics.enemy_spotted ? "YES" : "NO") << "\n";
        std::cout << "Bait used: " << (tactics.bait_active ? "YES" : "NO") << "\n";
        std::cout << "===================================================\n";
    }

    return hero_path;
}

/**
 * Основная функция Fixed Mode.
 */
std::vector<std::vector<std::pair<int, int>>> find_hero_vs_enemy_paths_fixed(bool debug = false)
{
    std::cout << "\n=== SMART HERO VS ENEMY (FIXED VERSION) ===\n";
    std::cout << "Hero: Strategic evasion with NO TELEPORTATION!\n";
    std::cout << "Enemy: Persistent pursuit with patrol behavior\n";
    std::cout << "Vision: Hero=" << FIXED_HERO_EVASION_RANGE
        << ", Enemy=" << FIXED_ENEMY_VISION_RANGE << " cells\n";
    std::cout << "COLLISION = INSTANT DEFEAT\n\n";

    srand(static_cast<unsigned int>(time(nullptr)));

    int ex, ey;
    do {
        ex = 8 + rand() % 8;
        ey = 8 + rand() % 8;
    } while ((abs(ex - 0) + abs(ey - 0)) < 10);

    enemy_start = std::make_pair(ex, ey);

    std::cout << "⚠️ ENEMY SPAWNED AT (" << ex << ", " << ey << ")\n";
    std::cout << "⚠️ Distance from hero: " << (abs(ex - 0) + abs(ey - 0)) << " cells\n\n";

    std::cout << "Hero planning strategic route...\n";
    auto hero_path = find_smart_hero_path_fixed(std::make_pair(ex, ey), debug);

    if (hero_path.empty())
    {
        std::cout << "HERO TRAPPED! No escape possible.\n";
        return {};
    }

    std::cout << "\nEnemy initiating pursuit...\n";
    auto enemy_path = smart_enemy_pursuit_fixed(hero_path);

    if (enemy_path.empty())
    {
        enemy_path.push_back(enemy_start);
    }

    bool collision_possible = false;
    std::set<std::pair<int, int>> hero_set(hero_path.begin(), hero_path.end());

    for (const auto& pos : enemy_path)
    {
        if (hero_set.find(pos) != hero_set.end())
        {
            collision_possible = true;
            break;
        }
    }

    std::cout << "\n🎯 ANALYSIS COMPLETE\n";
    std::cout << " Hero path: " << hero_path.size() - 1 << " steps\n";
    std::cout << " Enemy path: " << enemy_path.size() - 1 << " steps\n";
    std::cout << " Collision risk: "
        << (collision_possible ? "HIGH ⚠️" : "LOW ✅") << "\n";

    if (collision_possible)
    {
        std::cout << "WARNING: Hero must evade carefully!\n";
    }

    std::cout << "\n🚀 Press SPACE for strategic chase!\n";

    std::vector<std::vector<std::pair<int, int>>> result;
    result.push_back(hero_path);
    result.push_back(enemy_path);
    return result;
}

#endif // HERO_VS_ENEMY_FIXED_HPP