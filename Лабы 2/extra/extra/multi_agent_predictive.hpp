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
#include <set>
#include "generate_maze.hpp"

/**
  Сложность: O(b), где b - количество битов в числе (32 в данном случае).
 */
int count_bits(unsigned int mask)
{
    int count = 0;
    while (mask)
    {
        count += mask & 1;  // Проверяем младший бит
        mask >>= 1;         // Сдвигаем маску вправо
    }
    return count;
}

/**
 * Модифицированный алгоритм A* с поддержкой селективного сбора еды.
 * Поддерживает три режима работы с едой:
 * 1. Обязательный сбор определенной еды
 * 2. Избегание определенной еды (чтобы оставить другому агенту)
 * 3. Настройка важности сбора еды в целом
 *
 * Сложность: O(S log S), где S = W×H×H_max×2^F (размер пространства состояний)
 *
 * @param start_x, start_y Стартовая позиция агента
 * @param goal_x, goal_y Целевая позиция (выход)
 * @param must_take_food Битовая маска еды, которую ОБЯЗАТЕЛЬНО нужно собрать
 * @param avoid_food Битовая маска еды, которую нужно ИЗБЕГАТЬ
 * @param food_importance Коэффициент важности сбора еды (0.0-1.0)
 * @return Вектор координат, представляющий оптимальный путь
 */
std::vector<std::pair<int, int>> find_path_a_star_selective(
    int start_x, int start_y,
    int goal_x, int goal_y,
    unsigned int must_take_food = 0,    // Еда, которую ОБЯЗАТЕЛЬНО нужно взять
    unsigned int avoid_food = 0,        // Еда, которую нужно ИЗБЕГАТЬ (оставить другому)
    double food_importance = 1.0)       // Важность сбора еды (0-1)
{
    // Структура узла для алгоритма A* с учетом голода и собранной еды
    struct Node
    {
        int x, y;              // Координаты клетки
        int g;                 // Реальная стоимость пути от старта
        int h;                 // Эвристическая оценка до цели
        int hunger;            // Текущий уровень голода
        unsigned int food_mask;// Маска собранной еды (1 бит на каждую единицу еды)

        // Оператор сравнения для приоритетной очереди (min-heap)
        bool operator>(const Node& other) const
        {
            return (g + h) > (other.g + other.h); // Сравниваем по f = g + h
        }
    };

    // Хеш-функция для использования Node в качестве ключа в unordered_map
    struct NodeHash
    {
        std::size_t operator()(const Node& n) const
        {
            // Комбинируем все поля в один хеш
            return ((static_cast<std::size_t>(n.x) << 20) ^
                (static_cast<std::size_t>(n.y) << 15) ^
                (static_cast<std::size_t>(n.hunger) << 7) ^
                n.food_mask);
        }
    };

    // Функтор сравнения узлов на равенство
    struct NodeEqual
    {
        bool operator()(const Node& a, const Node& b) const
        {
            // Два узла равны, если совпадают все их параметры
            return a.x == b.x && a.y == b.y &&
                a.hunger == b.hunger &&
                a.food_mask == b.food_mask;
        }
    };

    // Структуры данных для алгоритма A*
    std::unordered_map<Node, Node, NodeHash, NodeEqual> came_from; // Для восстановления пути
    std::unordered_map<Node, int, NodeHash, NodeEqual> g_score;    // Лучшие известные стоимости
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open_set; // Очередь с приоритетом

    // Вычисляем базовое манхэттенское расстояние от старта до цели
    int manhattan = std::abs(goal_x - start_x) + std::abs(goal_y - start_y);
    int food_penalty = 0;

    // Если есть обязательная еда, добавляем штраф за её несбор
    // Это заставляет алгоритм искать пути через обязательную еду
    for (size_t i = 0; i < food_locations.size(); ++i)
    {
        if ((must_take_food >> i) & 1) // Проверяем, является ли еда обязательной
        {
            // Расстояние от старта до этой еды
            int food_dist = std::abs(food_locations[i].first - start_x) +
                std::abs(food_locations[i].second - start_y);
            food_penalty += food_dist * 5; // Штраф пропорционален расстоянию
        }
    }

    // Создаем начальный узел с учетом штрафа за обязательную еду
    Node start{ start_x, start_y, 0, manhattan + food_penalty, INITIAL_HUNGER, 0 };
    g_score[start] = 0;
    open_set.push(start);

    // Направления движения: юг, восток, север, запад
    const int dx[4] = { 0, 1, 0, -1 };
    const int dy[4] = { 1, 0, -1, 0 };

    Node best_final;               // Лучшее конечное состояние
    bool found = false;            // Найден ли путь
    int best_cost = std::numeric_limits<int>::max(); // Лучшая стоимость

    // Основной цикл алгоритма A*
    while (!open_set.empty())
    {
        // Извлекаем узел с наименьшей оценкой f = g + h
        Node current = open_set.top();
        open_set.pop();

        // Проверяем, достигли ли цели и собрали ли всю обязательную еду
        bool all_must_food_collected = ((current.food_mask & must_take_food) == must_take_food);

        // Условие завершения: в целевой клетке и вся обязательная еда собрана
        if (current.x == goal_x && current.y == goal_y && all_must_food_collected)
        {
            // Сохраняем лучший найденный путь (с минимальной стоимостью)
            if (current.g < best_cost)
            {
                best_cost = current.g;
                best_final = current;
                found = true;
            }
            continue; // Продолжаем поиск, может быть путь с меньшей стоимостью
        }

        // Исследуем соседние клетки
        for (int i = 0; i < 4; ++i)
        {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            // Проверка границ лабиринта
            if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT) continue;

            // Проверка наличия прохода в этом направлении
            bool can_pass = false;
            switch (i)
            {
            case 0: can_pass = !maze[current.y][current.x].south; break; // Юг
            case 1: can_pass = !maze[current.y][current.x].east; break;  // Восток
            case 2: can_pass = !maze[current.y][current.x].north; break; // Север
            case 3: can_pass = !maze[current.y][current.x].west; break;  // Запад
            }
            if (!can_pass) continue; // Стена блокирует путь

            // Уменьшаем голод за шаг
            int new_hunger = current.hunger - HUNGER_COST_PER_STEP;
            if (new_hunger <= 0) continue; // Агент умрет от голода

            unsigned int new_mask = current.food_mask; // Копируем маску собранной еды

            // Проверяем, есть ли еда в следующей клетке
            for (size_t j = 0; j < food_locations.size(); ++j)
            {
                // Проверяем координаты и факт, что еда еще не собрана
                if (food_locations[j].first == nx &&
                    food_locations[j].second == ny &&
                    ((new_mask >> j) & 1) == 0)
                {
                    // Если эту еду нужно избегать - пропускаем её
                    if ((avoid_food >> j) & 1)
                    {
                        continue; // Оставляем эту еду другому агенту
                    }

                    // Собираем еду
                    new_hunger += FOOD_RESTORE;
                    new_hunger = std::min(new_hunger,
                        INITIAL_HUNGER + FOOD_RESTORE * FOOD_COUNT);
                    new_mask |= (1u << j); // Устанавливаем соответствующий бит
                }
            }

            // Вычисляем стоимость пути до этой клетки
            int tentative_g = current.g + maze[ny][nx].cost;

            // Эвристическая оценка: манхэттенское расстояние до цели
            int heuristic = std::abs(goal_x - nx) + std::abs(goal_y - ny);

            // Добавляем штраф за несобранную обязательную еду
            unsigned int missing_must_food = must_take_food & (~new_mask);
            int remaining_food_penalty = 0;

            for (size_t j = 0; j < food_locations.size(); ++j)
            {
                if ((missing_must_food >> j) & 1) // Если эта еда обязательна и не собрана
                {
                    // Расстояние от текущей позиции до этой еды
                    int food_dist = std::abs(food_locations[j].first - nx) +
                        std::abs(food_locations[j].second - ny);
                    remaining_food_penalty += food_dist * 3; // Штраф за каждую несобранную еду
                }
            }

            heuristic += remaining_food_penalty; // Учитываем штраф в эвристике

            // Создаем узел для соседней клетки
            Node neighbor{ nx, ny, tentative_g, heuristic, new_hunger, new_mask };

            // Проверяем, нашли ли мы лучший путь к этому состоянию
            auto it = g_score.find(neighbor);
            if (it == g_score.end() || tentative_g < it->second)
            {
                came_from[neighbor] = current; // Запоминаем, откуда пришли
                g_score[neighbor] = tentative_g; // Обновляем лучшую стоимость
                open_set.push(neighbor); // Добавляем в очередь для исследования
            }
        }
    }

    // Если путь не найден
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
        if (it == came_from.end()) break; // Достигли начального узла
        node = it->second;
    }

    // Путь был собран от конца к началу, поэтому разворачиваем его
    std::reverse(path.begin(), path.end());
    return path;
}

/**
 * Оценивает минимальное количество еды, необходимое агенту для выживания.
 * Расчет основан на длине пути и механике голода.
 *
 * @param path_length Длина планируемого пути (в шагах)
 * @return Минимальное количество единиц еды, необходимых для выживания
 *
 * Сложность: O(1)
 */
int estimate_min_food_needed(int path_length)
{
    // Общее количество голода, которое будет потрачено
    int hunger_needed = path_length * HUNGER_COST_PER_STEP;

    // Сколько голода нужно восполнить едой (с учетом начального запаса)
    int hunger_to_restore = std::max(0, hunger_needed - INITIAL_HUNGER);

    // Количество еды, необходимое для восстановления этого голода
    // Округляем вверх: (a + b - 1) / b
    int food_needed = (hunger_to_restore + FOOD_RESTORE - 1) / FOOD_RESTORE;

    return food_needed;
}

/**
 * Определяет, какую еду первый агент должен оставить второму.
 * Использует эвристику для выбора наиболее полезной для второго агента еды.
 *
 * @param all_food_mask Маска всей еды, доступной первому агенту
 * @param path_length_second Оценочная длина пути второго агента
 * @return Маска еды, которую стоит оставить второму агенту
 *
 * Сложность: O(F log F), где F - количество единиц еды
 */
unsigned int select_food_to_leave(unsigned int all_food_mask, int path_length_second)
{
    // Если нет доступной еды, нечего оставлять
    if (all_food_mask == 0) return 0;

    // Оцениваем минимальное количество еды, необходимое второму агенту
    int food_needed_for_second = estimate_min_food_needed(path_length_second);

    // Подсчитываем общее количество доступной еды
    int total_food_count = count_bits(all_food_mask);

    // Если еды очень мало, оставляем минимум
    if (total_food_count <= 2)
    {
        // Оставляем только первую еду (бит 0)
        return all_food_mask & 1;
    }

    unsigned int leave_mask = 0; // Маска еды для оставления
    // Определяем, сколько еды оставить (минимум из необходимого и половины доступного)
    int food_to_leave = std::min(food_needed_for_second, total_food_count / 2);

    // Эвристика: оставляем еду, которая ближе к выходу
    // Предполагаем, что второй агент будет двигаться сзади, и ему полезнее еда у выхода
    std::vector<std::pair<int, std::pair<int, int>>> food_with_distances;

    // Собираем информацию о каждой доступной единице еды
    for (size_t i = 0; i < food_locations.size(); ++i)
    {
        if ((all_food_mask >> i) & 1) // Если эта еда доступна
        {
            // Вычисляем расстояние от еды до выхода (правый нижний угол)
            int dist_to_exit = std::abs(WIDTH - 1 - food_locations[i].first) +
                std::abs(HEIGHT - 1 - food_locations[i].second);
            // Сохраняем расстояние и координаты еды
            food_with_distances.push_back(
                std::make_pair(dist_to_exit, food_locations[i]));
        }
    }

    // Сортируем еду по расстоянию до выхода (ближе к выходу = выше приоритет)
    std::sort(food_with_distances.begin(), food_with_distances.end());

    // Выбираем первые food_to_leave единиц еды (ближайшие к выходу)
    for (int i = 0; i < std::min(food_to_leave, (int)food_with_distances.size()); ++i)
    {
        // Находим индекс этой еды в глобальном массиве food_locations
        for (size_t j = 0; j < food_locations.size(); ++j)
        {
            // Сравниваем координаты
            if (food_locations[j].first == food_with_distances[i].second.first &&
                food_locations[j].second == food_with_distances[i].second.second)
            {
                leave_mask |= (1u << j); // Устанавливаем соответствующий бит
                break;
            }
        }
    }

    return leave_mask;
}

/**
 * Основная функция кооперативной стратегии двух агентов.
 * Реализует продвинутую логику взаимодействия:
 * 1. Агент 2 предсказывает, какую еду возьмет Агент 1
 * 2. Агент 1 сознательно оставляет часть еды Агенту 2
 * 3. Оба агента стремятся обеспечить выживание друг друга
 *
 * Сложность: O(3 × T), где T - сложность find_path_a_star_selective
 *
 * @return Вектор из двух путей: [путь_агента1, путь_агента2]
 */
std::vector<std::vector<std::pair<int, int>>> find_paths_two_agents_predictive()
{
    std::cout << "\n=== TWO AGENTS: COOPERATIVE STRATEGY ===\n";
    std::cout << "Agent 2 predicts Agent 1's food choices.\n";
    std::cout << "Agent 1 leaves food for Agent 2 to ensure both survive.\n\n";

    // ----- ШАГ 1: Оптимальный путь Агента 1 без ограничений -----
    std::cout << "Step 1: Finding optimal path for Agent 1 (without restrictions)...\n";
    std::vector<std::pair<int, int>> path1_unrestricted = find_path_a_star_selective(
        0, 0, WIDTH - 1, HEIGHT - 1);

    if (path1_unrestricted.empty())
    {
        std::cout << "ERROR: Agent 1 cannot find any path!\n";
        return {}; // Возвращаем пустой результат
    }

    // Определяем, какую еду Агент 1 соберет на оптимальном пути
    unsigned int food_on_optimal_path = 0;
    for (const auto& pos : path1_unrestricted)
    {
        for (size_t i = 0; i < food_locations.size(); ++i)
        {
            // Проверяем, находится ли еда в текущей позиции
            if (food_locations[i].first == pos.first &&
                food_locations[i].second == pos.second)
            {
                food_on_optimal_path |= (1u << i); // Устанавливаем бит
                break;
            }
        }
    }

    int optimal_food_count = count_bits(food_on_optimal_path);
    std::cout << "Agent 1 optimal path collects " << optimal_food_count
        << " food items\n";

    // ----- ШАГ 2: Агент 2 предсказывает и планирует альтернативный путь -----
    std::cout << "\nStep 2: Agent 2 predicts Agent 1's food choices...\n";

    // Агент 2 знает, какую еду возьмет Агент 1, и избегает её
    std::vector<std::pair<int, int>> path2 = find_path_a_star_selective(
        0, 0, WIDTH - 1, HEIGHT - 1,
        0,                   // Нет обязательной еды для Агента 2
        food_on_optimal_path, // Избегаем еду, которую возьмет Агент 1
        0.5                  // Умеренная важность сбора оставшейся еды
    );

    // Если не удалось найти путь, избегая еды Агента 1, используем fallback
    if (path2.empty())
    {
        std::cout << "WARNING: Agent 2 cannot avoid Agent 1's food.\n";
        std::cout << "Trying fallback - any path to exit...\n";
        path2 = find_path_a_star_selective(0, 0, WIDTH - 1, HEIGHT - 1);
    }

    // Оцениваем потребности Агента 2 в еде
    int food_needed_for_agent2 = estimate_min_food_needed((int)path2.size() - 1);
    std::cout << "Agent 2 needs at least " << food_needed_for_agent2
        << " food items to survive\n";

    // Определяем, какую еду может собрать Агент 2 на своем пути
    unsigned int food_on_path2 = 0;
    for (const auto& pos : path2)
    {
        for (size_t i = 0; i < food_locations.size(); ++i)
        {
            if (food_locations[i].first == pos.first &&
                food_locations[i].second == pos.second)
            {
                food_on_path2 |= (1u << i);
                break;
            }
        }
    }

    int food_on_path2_count = count_bits(food_on_path2);
    std::cout << "Agent 2 can collect " << food_on_path2_count
        << " food items while avoiding Agent 1's food\n";

    // ----- ШАГ 3: Агент 1 планирует кооперативный путь -----
    std::cout << "\nStep 3: Agent 1 plans cooperative path...\n";

    // Определяем, какую еду Агент 1 должен оставить Агенту 2
    unsigned int food_to_leave = select_food_to_leave(
        food_on_optimal_path,
        (int)path2.size() - 1
    );

    // Еда, которую Агент 1 обязательно должен взять
    unsigned int food_must_take = food_on_optimal_path & (~food_to_leave);

    std::cout << "Agent 1 will leave " << count_bits(food_to_leave)
        << " food items for Agent 2\n";
    std::cout << "Agent 1 will take " << count_bits(food_must_take)
        << " food items\n";

    // Агент 1 ищет путь, собирая только обязательную еду
    std::vector<std::pair<int, int>> path1_cooperative = find_path_a_star_selective(
        0, 0, WIDTH - 1, HEIGHT - 1,
        food_must_take,  // Еда, которую обязательно взять
        0,               // Не избегаем еду (вся оставшаяся уже учтена)
        1.0              // Высокая важность обязательной еды
    );

    // Fallback: если кооперативный путь не найден, используем оптимальный
    if (path1_cooperative.empty())
    {
        std::cout << "WARNING: Cooperative path not found, using optimal path\n";
        path1_cooperative = path1_unrestricted;
    }

    // ----- СИМУЛЯЦИЯ И АНАЛИЗ -----
    std::cout << "\n=== COOPERATIVE SIMULATION ===\n";

    // Структуры для отслеживания состояния еды и агентов
    std::vector<bool> food_eaten(FOOD_COUNT, false);        // Была ли съедена еда
    std::vector<int> agent_food_count(2, 0);                // Количество съеденной еды
    std::vector<std::string> agent_names = { "Agent 1 (Yellow)", "Agent 2 (Cyan)" };
    std::vector<int> food_eaten_by(FOOD_COUNT, -1);         // Кто съел каждую еду
    std::vector<int> food_eaten_at_step(FOOD_COUNT, -1);    // На каком шаге была съедена

    // Фаза 1: Агент 1 движется по своему пути
    for (size_t t = 0; t < path1_cooperative.size(); ++t)
    {
        int x = path1_cooperative[t].first;
        int y = path1_cooperative[t].second;

        // Проверяем все единицы еды
        for (size_t i = 0; i < food_locations.size(); ++i)
        {
            // Если еда еще не съедена и находится в текущей клетке
            if (!food_eaten[i] &&
                food_locations[i].first == x &&
                food_locations[i].second == y)
            {
                food_eaten[i] = true;
                agent_food_count[0]++;
                food_eaten_by[i] = 0;
                food_eaten_at_step[i] = static_cast<int>(t);

                std::cout << "Step " << t << ": " << agent_names[0]
                    << " takes food at (" << x << "," << y << ")";

                // Проверяем, предназначалась ли эта еда Агенту 2
                if ((food_to_leave >> i) & 1)
                {
                    std::cout << " (was supposed to leave it!)";
                    // Агент 1 случайно взял еду, которую хотел оставить
                }
                std::cout << "\n";
            }
        }
    }

    // Фаза 2: Агент 2 движется после Агента 1
    for (size_t t = 0; t < path2.size(); ++t)
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
                // Шаг учитывает, что Агент 2 начал позже
                food_eaten_at_step[i] = static_cast<int>(t + path1_cooperative.size());

                std::cout << "Step " << (t + path1_cooperative.size())
                    << ": " << agent_names[1]
                    << " takes food at (" << x << "," << y << ")";

                // Отмечаем, предназначалась ли эта еда Агенту 2
                if ((food_to_leave >> i) & 1)
                {
                    std::cout << " (left by Agent 1 intentionally)";
                }
                std::cout << "\n";
            }
        }
    }

    // ----- ФИНАЛЬНЫЙ АНАЛИЗ -----
    std::cout << "\n=== COOPERATIVE RESULTS ===\n";
    std::cout << "Agent 1: " << (path1_cooperative.size() - 1)
        << " steps, ate " << agent_food_count[0] << " food\n";
    std::cout << "Agent 2: " << (path2.size() - 1)
        << " steps, ate " << agent_food_count[1] << " food\n";
    std::cout << "Total food eaten: " << (agent_food_count[0] + agent_food_count[1])
        << "/" << FOOD_COUNT << "\n";

    // Расчет итогового уровня голода для каждого агента
    int hunger1 = INITIAL_HUNGER + agent_food_count[0] * FOOD_RESTORE - (int)(path1_cooperative.size() - 1);
    int hunger2 = INITIAL_HUNGER + agent_food_count[1] * FOOD_RESTORE - (int)(path2.size() - 1);

    // Анализ выживания
    std::cout << "\nSurvival analysis:\n";
    std::cout << "Agent 1 final hunger: " << hunger1
        << " (" << (hunger1 > 0 ? "SURVIVES" : "DIES") << ")\n";
    std::cout << "Agent 2 final hunger: " << hunger2
        << " (" << (hunger2 > 0 ? "SURVIVES" : "DIES") << ")\n";

    // Оценка эффективности кооперации
    bool both_survive = (hunger1 > 0) && (hunger2 > 0);
    std::cout << "\nCooperation effectiveness: ";
    if (both_survive)
    {
        std::cout << "SUCCESS! Both agents survive through cooperation!\n";
    }
    else if (hunger1 > 0 && hunger2 <= 0)
    {
        std::cout << "PARTIAL: Agent 1 survives, but Agent 2 dies\n";
    }
    else if (hunger1 <= 0 && hunger2 > 0)
    {
        std::cout << "PARTIAL: Agent 2 survives, but Agent 1 dies\n";
    }
    else
    {
        std::cout << "FAILURE: Both agents die despite cooperation\n";
    }

    // Возвращаем найденные пути
    std::vector<std::vector<std::pair<int, int>>> result;
    result.push_back(path1_cooperative);
    result.push_back(path2);
    return result;
}

#endif