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
#include "generate_maze.hpp"
#include "dijkstra_search.hpp"

// Глобальная переменная - стартовая позиция врага
extern std::pair<int, int> enemy_start;

// Параметры ИИ врага
const int ENEMY_VISION_RANGE = 8;      // Радиус видимости врага (в клетках)
const int ENEMY_MEMORY_STEPS = 15;     // Длительность памяти о последнем месте героя
const int ENEMY_PATROL_RADIUS = 6;     // Радиус патрулирования вокруг начальной позиции

/**
 * Структура памяти врага.
 * Хранит информацию о герое и состоянии ИИ.
 */
struct EnemyMemory
{
    std::pair<int, int> last_seen_position;  // Последняя видимая позиция героя
    int steps_since_last_seen;               // Шагов с момента последнего контакта
    bool has_target;                         // Есть ли текущая цель
    std::pair<int, int> patrol_target;       // Текущая цель патрулирования
    bool chasing_mode;                       // Режим преследования активен

    EnemyMemory() : steps_since_last_seen(999), has_target(false), chasing_mode(false)
    {
        last_seen_position = std::make_pair(-1, -1);
        patrol_target = std::make_pair(-1, -1);
    }
};

/**
 * Упрощенный алгоритм A* для поиска пути врага.
 * Не учитывает голод и еду, только геометрию лабиринта.
 * Сложность: O(N log N), где N - количество исследованных клеток.
 */
std::vector<std::pair<int, int>> find_simple_enemy_path(int start_x, int start_y,
    int target_x, int target_y)
{
    // Структура узла для A*
    struct Node
    {
        int x, y;
        int g, h; // g - стоимость от старта, h - эвристика до цели

        bool operator>(const Node& other) const
        {
            return (g + h) > (other.g + other.h);
        }
    };

    // Хеш-функция для узла (простая индексация по координатам)
    struct NodeHash
    {
        size_t operator()(const Node& n) const
        {
            return static_cast<size_t>(n.y) * WIDTH + n.x;
        }
    };

    // Сравнение узлов на равенство
    struct NodeEqual
    {
        bool operator()(const Node& a, const Node& b) const
        {
            return a.x == b.x && a.y == b.y;
        }
    };

    // Структуры данных для A*
    std::unordered_map<Node, Node, NodeHash, NodeEqual> came_from; // Для восстановления пути
    std::unordered_map<Node, int, NodeHash, NodeEqual> g_score;    // Лучшие стоимости
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open_set; // Очередь с приоритетом

    // Инициализация начального узла
    Node start_node = { start_x, start_y, 0, abs(target_x - start_x) + abs(target_y - start_y) };
    g_score[start_node] = 0;
    open_set.push(start_node);

    // Направления движения
    const int dx[4] = { 0, 1, 0, -1 };
    const int dy[4] = { 1, 0, -1, 0 };

    // Основной цикл A*
    while (!open_set.empty())
    {
        Node current = open_set.top();
        open_set.pop();

        // Проверка достижения цели
        if (current.x == target_x && current.y == target_y)
        {
            // Восстановление пути от цели к старту
            std::vector<std::pair<int, int>> path;
            Node node = current;
            while (true)
            {
                path.push_back(std::make_pair(node.x, node.y));
                auto it = came_from.find(node);
                if (it == came_from.end()) break; // Достигли стартовой точки
                node = it->second;
            }
            std::reverse(path.begin(), path.end()); // Разворачиваем путь
            return path;
        }

        // Исследование соседних клеток
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

            // Вычисление новой стоимости
            int tentative_g = current.g + 1;
            Node neighbor = { nx, ny, tentative_g, abs(target_x - nx) + abs(target_y - ny) };

            // Проверка, нашли ли лучший путь к этой клетке
            auto it = g_score.find(neighbor);
            if (it == g_score.end() || tentative_g < it->second)
            {
                came_from[neighbor] = current;
                g_score[neighbor] = tentative_g;
                open_set.push(neighbor);
            }
        }
    }

    // Путь не найден
    return std::vector<std::pair<int, int>>();
}

/**
 * Проверка видимости героя врагом.
 * Упрощенная версия - учитывает только манхэттенское расстояние без учета стен.
 * Сложность: O(1)
 */
bool enemy_can_see_hero(int enemy_x, int enemy_y, int hero_x, int hero_y)
{
    // Простая проверка по манхэттенскому расстоянию (без учета стен)
    int dist = abs(enemy_x - hero_x) + abs(enemy_y - hero_y);
    return dist <= ENEMY_VISION_RANGE;
}

/**
 * Выбор случайной цели для патрулирования в заданном радиусе.
 * Сложность: O(1) в среднем случае, O(20) в худшем
 */
std::pair<int, int> get_patrol_target(int enemy_x, int enemy_y)
{
    int attempts = 0;
    while (attempts < 20)
    {
        // Генерация случайного смещения в пределах радиуса патрулирования
        int dx = (rand() % (ENEMY_PATROL_RADIUS * 2 + 1)) - ENEMY_PATROL_RADIUS;
        int dy = (rand() % (ENEMY_PATROL_RADIUS * 2 + 1)) - ENEMY_PATROL_RADIUS;

        int target_x = enemy_x + dx;
        int target_y = enemy_y + dy;

        // Проверка валидности координат
        if (target_x >= 0 && target_x < WIDTH &&
            target_y >= 0 && target_y < HEIGHT &&
            (target_x != enemy_x || target_y != enemy_y))
        {
            return std::make_pair(target_x, target_y);
        }
        attempts++;
    }

    // Fallback: возвращаем текущую позицию
    return std::make_pair(enemy_x, enemy_y);
}

/**
 * Выбор точки для засады на пути героя.
 * Выбирает точку на 1/3 пути героя от старта.
 * Сложность: O(1)
 */
std::pair<int, int> get_ambush_point(const std::vector<std::pair<int, int>>& hero_path,
    int enemy_x, int enemy_y)
{
    if (hero_path.empty())
    {
        return std::make_pair(enemy_x, enemy_y);
    }

    // Выбираем точку на пути героя на 1/3 от начала
    size_t ambush_idx = hero_path.size() / 3;
    if (ambush_idx >= hero_path.size())
    {
        ambush_idx = hero_path.size() - 1;
    }

    return hero_path[ambush_idx];
}

/**
 * Агрессивное преследование героя с высокой вероятностью столкновения.
 * ИИ врага пытается перехватить героя на пути к выходу.
 * Сложность: O(T × P), где T - количество шагов, P - сложность A*
 */
std::vector<std::pair<int, int>> aggressive_enemy_pursuit(const std::vector<std::pair<int, int>>& hero_path)
{
    if (hero_path.empty())
    {
        return std::vector<std::pair<int, int>>();
    }

    std::vector<std::pair<int, int>> enemy_path;
    EnemyMemory memory;

    // Начальная позиция врага
    int ex = enemy_start.first;
    int ey = enemy_start.second;
    enemy_path.push_back(std::make_pair(ex, ey));

    // Устанавливаем начальную цель - точку засады на пути героя
    memory.patrol_target = get_ambush_point(hero_path, ex, ey);
    memory.chasing_mode = true;

    size_t hero_idx = 0;
    int steps_without_contact = 0;
    const int MAX_STEPS_WITHOUT_CONTACT = 30;

    std::cout << "Enemy AI: Aggressive mode activated!\n";
    std::cout << "Enemy AI: Ambush point at ("
        << memory.patrol_target.first << ","
        << memory.patrol_target.second << ")\n\n";

    // Основной цикл преследования
    while (hero_idx < hero_path.size() && enemy_path.size() < 100)
    {
        // Текущая позиция героя
        int hx = hero_path[std::min(hero_idx, hero_path.size() - 1)].first;
        int hy = hero_path[std::min(hero_idx, hero_path.size() - 1)].second;

        // Проверка видимости героя
        bool can_see = enemy_can_see_hero(ex, ey, hx, hy);
        int dist_to_hero = abs(ex - hx) + abs(ey - hy);

        // Если герой виден или враг в режиме преследования
        if (can_see || memory.chasing_mode)
        {
            // Обновляем память о герое
            memory.last_seen_position = std::make_pair(hx, hy);
            memory.steps_since_last_seen = 0;
            memory.has_target = true;
            memory.chasing_mode = true;

            if (can_see)
            {
                std::cout << "Enemy AI: Step " << enemy_path.size()
                    << " - HERO SPOTTED! Distance: " << dist_to_hero << " cells\n";
            }

            // Преследование: ищем путь к текущей позиции героя
            std::vector<std::pair<int, int>> pursuit_path =
                find_simple_enemy_path(ex, ey, hx, hy);

            if (!pursuit_path.empty() && pursuit_path.size() > 1)
            {
                ex = pursuit_path[1].first;
                ey = pursuit_path[1].second;
                enemy_path.push_back(std::make_pair(ex, ey));
                steps_without_contact = 0;
            }
        }
        else
        {
            // Герой не виден - двигаемся к точке засады
            steps_without_contact++;

            // Если долго не видим героя, меняем точку засады
            if (steps_without_contact > MAX_STEPS_WITHOUT_CONTACT)
            {
                memory.patrol_target = get_ambush_point(hero_path, ex, ey);
                steps_without_contact = 0;
                std::cout << "Enemy AI: Changing ambush point to ("
                    << memory.patrol_target.first << ","
                    << memory.patrol_target.second << ")\n";
            }

            // Движение к точке засады
            std::vector<std::pair<int, int>> patrol_path =
                find_simple_enemy_path(ex, ey,
                    memory.patrol_target.first,
                    memory.patrol_target.second);

            if (!patrol_path.empty() && patrol_path.size() > 1)
            {
                ex = patrol_path[1].first;
                ey = patrol_path[1].second;
                enemy_path.push_back(std::make_pair(ex, ey));
            }
        }

        // Герой движется вперед
        hero_idx++;

        // Если враг близко к герою, ускоряем погоню (драматический эффект)
        if (dist_to_hero < 10 && hero_idx + 1 < hero_path.size())
        {
            hero_idx++; // Герой "убегает" быстрее
        }
    }

    std::cout << "\nEnemy AI: Aggressive pursuit finished\n";
    std::cout << "Enemy path length: " << (enemy_path.size() - 1) << " steps\n";
    std::cout << "WARNING: Contact likely! Collision = INSTANT DEFEAT for hero!\n";

    return enemy_path;
}

/**
 * Нахождение пути героя с учетом присутствия врага.
 * Использует стандартный алгоритм Дейкстры, но анализирует безопасность пути.
 * Сложность: O(S log S) (смотри комментарий к find_shortest_path)
 */
std::vector<std::pair<int, int>> find_hero_path_with_evasion(int enemy_start_x, int enemy_start_y)
{
    // Очищаем глобальный путь
    path.clear();

    // Используем стандартный алгоритм поиска пути
    bool found = find_shortest_path();

    if (!found || path.empty())
    {
        return std::vector<std::pair<int, int>>();
    }

    // Анализ безопасности пути: проверяем близость к стартовой позиции врага
    int min_dist_to_enemy_start = 1000;
    for (const auto& p : path)
    {
        int dist = abs(p.first - enemy_start_x) + abs(p.second - enemy_start_y);
        if (dist < min_dist_to_enemy_start)
        {
            min_dist_to_enemy_start = dist;
        }
    }

    // Вывод информации о безопасности пути
    std::cout << "Hero path analysis:\n";
    std::cout << "  Total steps: " << path.size() - 1 << "\n";
    std::cout << "  Min distance to enemy start: " << min_dist_to_enemy_start << " cells\n";

    // Предупреждение о опасной близости к врагу
    if (min_dist_to_enemy_start < 8)
    {
        std::cout << "  WARNING: Path passes close to enemy! High risk of encounter!\n";
    }
    else
    {
        std::cout << "  Path is relatively safe from initial enemy position.\n";
    }

    return path;
}

/**
 * Основная функция для генерации путей героя и врага.
 * Создает сценарий погони с высокой вероятностью столкновения.
 * Сложность: O(H + E), где H - сложность поиска пути героя, E - сложность преследования
 */
std::vector<std::vector<std::pair<int, int>>> find_hero_vs_enemy_paths()
{
    std::cout << "\n=== HERO VS ENEMY: DEADLY PURSUIT ===\n";
    std::cout << "Hero (Yellow) - escape or die!\n";
    std::cout << "Enemy (Red) - AGGRESSIVE & DEADLY\n";
    std::cout << "  • Vision range: " << ENEMY_VISION_RANGE << " cells\n";
    std::cout << "  • Memory: " << ENEMY_MEMORY_STEPS << " steps\n";
    std::cout << "  • Patrol radius: " << ENEMY_PATROL_RADIUS << " cells\n";
    std::cout << "  • COLLISION = INSTANT DEFEAT (hero loses ALL hunger)\n";
    std::cout << "Food: " << FOOD_COUNT << " pieces\n\n";

    // Инициализация генератора случайных чисел
    srand(static_cast<unsigned int>(time(nullptr)));

    // Генерация позиции врага (близко к старту героя для драматизма)
    int ex, ey;
    int attempts = 0;

    // Генерация позиции врага в пределах 10-15 клеток от старта
    do
    {
        ex = 5 + rand() % 10;
        ey = 5 + rand() % 10;
        attempts++;
    } while ((abs(ex - 0) + abs(ey - 0)) < 5 || attempts < 10);

    // Установка стартовой позиции врага
    enemy_start = std::make_pair(ex, ey);

    // Вывод информации о позиции врага
    std::cout << "⚠️  ENEMY SPAWNED AT (" << ex << ", " << ey << ")\n";
    std::cout << "⚠️  Distance from hero: " << (abs(ex - 0) + abs(ey - 0)) << " cells\n";
    std::cout << "⚠️  WARNING: Enemy is CLOSE! High chance of encounter!\n\n";

    // Поиск пути для героя
    std::cout << "Hero planning escape route...\n";
    std::vector<std::pair<int, int>> hero_path = find_hero_path_with_evasion(ex, ey);

    if (hero_path.empty())
    {
        std::cout << "HERO TRAPPED! No escape possible.\n";
        return std::vector<std::vector<std::pair<int, int>>>();
    }

    // Генерация пути преследования для врага
    std::cout << "\nEnemy preparing deadly pursuit...\n";
    std::vector<std::pair<int, int>> enemy_path = aggressive_enemy_pursuit(hero_path);

    // Запасной вариант: если путь врага не найден, остаемся на месте
    if (enemy_path.empty())
    {
        enemy_path.push_back(enemy_start);
    }

    // Финальная информация о сценарии
    std::cout << "\n⚔️  === DEADLY PURSUIT READY === ⚔️\n";
    std::cout << "Hero path: " << hero_path.size() - 1 << " steps\n";
    std::cout << "Enemy path: " << enemy_path.size() - 1 << " steps\n";
    std::cout << "🚨 COLLISION RULES:\n";
    std::cout << "   - If hero meets enemy: INSTANT DEFEAT\n";
    std::cout << "   - Hero loses ALL remaining hunger\n";
    std::cout << "   - Game over for hero!\n";
    std::cout << "Press SPACE to start the DEADLY chase!\n\n";

    // Возвращаем пути обоих участников
    std::vector<std::vector<std::pair<int, int>>> result;
    result.push_back(hero_path);
    result.push_back(enemy_path);
    return result;
}

#endif