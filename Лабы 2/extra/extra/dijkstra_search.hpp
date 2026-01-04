#ifndef DIJKSTRA_SEARCH_HPP
#define DIJKSTRA_SEARCH_HPP

#include <vector>
#include <queue>
#include <limits>
#include <unordered_map>
#include <algorithm>
#include <iostream>

/**
 * Структура состояния агента в лабиринте.
 * Содержит все параметры, необходимые для описания текущего положения и ресурсов.
 */
struct State
{
    int x, y;               // Координаты в лабиринте
    int hunger;             // Текущий уровень голода
    int cost;               // Накопленная стоимость пути
    unsigned int food_mask; // Битовая маска собранной еды (1 бит на каждую единицу еды)

    // Оператор сравнения для приоритетной очереди (min-heap)
    bool operator>(const State& other) const
    {
        return cost > other.cost;
    }
};

/**
 * Хеш-функция для структуры State.
 * Позволяет использовать State в качестве ключа в unordered_map.
 */
struct StateHash
{
    size_t operator()(const State& s) const
    {
        // Комбинирование всех полей состояния в один хеш
        return ((s.x << 20) ^ (s.y << 15) ^ (s.hunger << 7) ^ s.food_mask);
    }
};

/**
 * Функтор сравнения состояний на равенство.
 * Определяет, когда два состояния считаются одинаковыми.
 */
struct StateEqual
{
    bool operator()(const State& a, const State& b) const
    {
        return a.x == b.x && a.y == b.y &&
            a.hunger == b.hunger && a.food_mask == b.food_mask;
    }
};

/**
 * Информация о предыдущем состоянии для восстановления пути.
 */
struct PrevInfo
{
    int prev_hunger;
    int prev_x, prev_y;
    unsigned int prev_mask;
    bool valid;

    PrevInfo() : valid(false)
    {
    }

    PrevInfo(int h, int x, int y, unsigned int m) :
        prev_hunger(h), prev_x(x), prev_y(y), prev_mask(m), valid(true)
    {
    }
};

// Глобальная переменная для хранения найденного пути
extern std::vector<std::pair<int, int>> path;

/**
 * Находит кратчайший путь с учетом голода и возможности сбора еды.
 * Использует алгоритм Дейкстры с расширенным пространством состояний.
 *
 * Асимптотическая сложность: O(S log S), где S = W×H×H_max×2^F
 *   W×H - размер лабиринта (до 400)
 *   H_max - максимальный уровень голода
 *   2^F - все возможные комбинации собранной еды
 *
 * В худшем случае (при 8 единицах еды): 400 × 100 × 256 ≈ 10^7 состояний
 *
 * @return true если путь найден, false если агент не может дойти до цели
 */
bool find_shortest_path()
{
    // Максимально возможный уровень голода (начальный + вся еда + запас)
    const int MAX_H = INITIAL_HUNGER + FOOD_RESTORE * FOOD_COUNT + 50;

    // dist хранит минимальную стоимость достижения каждого состояния
    std::unordered_map<State, int, StateHash, StateEqual> dist;

    // prev хранит информацию для восстановления пути
    std::unordered_map<State, PrevInfo, StateHash, StateEqual> prev;

    // Приоритетная очередь для алгоритма Дейкстры (min-heap)
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;

    // Инициализация начального состояния
    State start{ 0, 0, INITIAL_HUNGER, 0, 0 };
    dist[start] = 0;
    pq.push(start);

    // Смещения для 4 направлений: юг, восток, север, запад
    const int dx[4] = { 0, 1, 0, -1 };
    const int dy[4] = { 1, 0, -1, 0 };

    State best_final_state;
    bool found = false;
    int best_cost = std::numeric_limits<int>::max();

    // Основной цикл алгоритма Дейкстры
    while (!pq.empty())
    {
        // Извлекаем состояние с минимальной стоимостью
        State current = pq.top();
        pq.pop();

        // Пропускаем устаревшие записи (если нашли лучший путь к этому состоянию)
        auto dist_it = dist.find(current);
        if (dist_it == dist.end() || current.cost > dist_it->second)
        {
            continue;
        }

        // Проверка достижения конечной точки (правый нижний угол)
        if (current.x == WIDTH - 1 && current.y == HEIGHT - 1)
        {
            // Обновляем лучший найденный путь к цели
            if (current.cost < best_cost)
            {
                best_cost = current.cost;
                best_final_state = current;
                found = true;
            }
            continue;
        }

        // Проверяем, есть ли в текущей клетке еда
        int food_index = -1;
        unsigned int new_mask = current.food_mask;
        int new_hunger = current.hunger;

        // Поиск индекса еды в текущей позиции
        for (size_t i = 0; i < food_locations.size(); ++i)
        {
            if (food_locations[i].first == current.x &&
                food_locations[i].second == current.y)
            {
                food_index = static_cast<int>(i);
                break;
            }
        }

        // Если еда найдена и еще не собрана в этом состоянии
        if (food_index != -1 && ((new_mask >> food_index) & 1) == 0)
        {
            // Устанавливаем соответствующий бит в маске
            new_mask |= (1u << food_index);
            // Восстанавливаем голод, но не больше максимума
            new_hunger = std::min(new_hunger + FOOD_RESTORE, MAX_H);
        }

        // Перебор всех возможных направлений движения
        for (int direction = 0; direction < 4; ++direction)
        {
            int next_x = current.x + dx[direction];
            int next_y = current.y + dy[direction];

            // Проверка выхода за границы лабиринта
            if (next_x < 0 || next_x >= WIDTH || next_y < 0 || next_y >= HEIGHT)
            {
                continue;
            }

            // Проверка наличия прохода в выбранном направлении
            bool can_pass = false;
            switch (direction)
            {
            case 0: can_pass = !maze[current.y][current.x].south; break;  // Юг
            case 1: can_pass = !maze[current.y][current.x].east; break;   // Восток
            case 2: can_pass = !maze[current.y][current.x].north; break;  // Север
            case 3: can_pass = !maze[current.y][current.x].west; break;   // Запад
            }

            // Если стена блокирует путь
            if (!can_pass)
            {
                continue;
            }

            // Уменьшаем голод за шаг
            int next_hunger = new_hunger - HUNGER_COST_PER_STEP;

            // Проверка смерти от голода
            if (next_hunger <= 0)
            {
                continue;
            }

            // Увеличиваем стоимость пути на стоимость следующей клетки
            int new_cost = current.cost + maze[next_y][next_x].cost;

            // Создаем новое состояние после перемещения
            State next_state{ next_x, next_y, next_hunger, new_cost, new_mask };

            // Проверяем, нашли ли мы более короткий путь к этому состоянию
            auto next_dist_it = dist.find(next_state);

            // Если это состояние новое или найден путь с меньшей стоимостью
            if (next_dist_it == dist.end() || new_cost < next_dist_it->second)
            {
                dist[next_state] = new_cost;
                prev[next_state] = PrevInfo(current.hunger, current.x,
                    current.y, current.food_mask);
                pq.push(next_state);
            }
        }
    }

    // Если не удалось найти ни одного пути до цели
    if (!found)
    {
        std::cout << "NO PATH FOUND - Agent would die from hunger!\n";
        path.clear();
        return false;
    }

    // Восстановление оптимального пути от конечной точки к начальной
    std::cout << "Optimal path found! Reconstructing...\n";
    path.clear();
    State current = best_final_state;

    while (true)
    {
        path.push_back(std::make_pair(current.x, current.y));

        auto prev_it = prev.find(current);

        // Останавливаемся, когда достигаем начального состояния
        if (prev_it == prev.end() || !prev_it->second.valid)
        {
            break;
        }

        // Создаем шаблон для поиска предыдущего состояния
        PrevInfo prev_info = prev_it->second;
        State prev_state_template{ prev_info.prev_x, prev_info.prev_y,
                                 prev_info.prev_hunger, 0, prev_info.prev_mask };

        // Ищем полное предыдущее состояние (не только координаты)
        bool found_prev = false;
        for (const auto& kv : dist)
        {
            const State& key = kv.first;

            // Проверяем полное совпадение всех параметров состояния
            if (key.x == prev_state_template.x && key.y == prev_state_template.y &&
                key.hunger == prev_state_template.hunger &&
                key.food_mask == prev_state_template.food_mask)
            {
                current = key;
                found_prev = true;
                break;
            }
        }

        if (!found_prev)
        {
            std::cerr << "Error: cannot find previous state!\n";
            break;
        }
    }

    // Путь был собран от конца к началу, поэтому разворачиваем его
    std::reverse(path.begin(), path.end());

    // Удаляем возможные дубликаты последовательных клеток
    std::vector<std::pair<int, int>> cleaned_path;
    for (size_t i = 0; i < path.size(); ++i)
    {
        // Добавляем точку, если это первая точка или она отличается от предыдущей
        if (i == 0 || path[i] != path[i - 1])
        {
            cleaned_path.push_back(path[i]);
        }
    }
    path = cleaned_path;

    // Вывод детальной информации о найденном пути
    std::cout << "\n========================================\n";
    std::cout << "PATH FOUND!\n";
    std::cout << "Length: " << (path.size() - 1) << " steps\n";
    std::cout << "Total cost: " << best_cost << " points\n";
    std::cout << "Final hunger: " << best_final_state.hunger << "\n";

    std::cout << "Food eaten: ";
    bool any_food = false;
    for (size_t i = 0; i < food_locations.size(); ++i)
    {
        // Проверяем, какие биты установлены в маске (какая еда была собрана)
        if ((best_final_state.food_mask >> i) & 1)
        {
            std::cout << "(" << food_locations[i].first
                << "," << food_locations[i].second << ") ";
            any_food = true;
        }
    }
    if (!any_food)
    {
        std::cout << "none";
    }
    std::cout << "\n========================================\n\n";

    return true;
}

/**
 * Выводит информацию о конфигурации лабиринта.
 * Сложность: O(W×H) - линейная от размера лабиринта.
 */
void print_maze_config()
{
    std::cout << "=== MAZE CONFIGURATION ===\n";
    std::cout << "Food locations:\n";
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            // Выводим координаты клеток, содержащих еду
            if (maze[y][x].has_food)
            {
                std::cout << "(" << x << "," << y << ") ";
            }
        }
    }
    std::cout << "\n\nCell costs:\n";
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            std::cout << maze[y][x].cost << " ";
        }
        std::cout << "\n";
    }
    std::cout << "===========================\n\n";
}

#endif