#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <queue>
#include <limits>
#include <algorithm>  // Для std::shuffle
#include <unordered_map>
#include <SFML/Graphics.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

// ================== КОНСТАНТЫ И НАСТРОЙКИ ==================
const int WIDTH = 20;              // Ширина лабиринта в клетках
const int HEIGHT = 20;             // Высота лабиринта в клетках
const int CELL_SIZE = 30;          // Размер клетки в пикселях для отрисовки

const int INITIAL_HUNGER = 20;     // Начальный уровень сытости
const int HUNGER_COST_PER_STEP = 1;// Голод тратится за каждый шаг
const int FOOD_RESTORE = 20;       // Сколько восстанавливает еда
const int FOOD_COUNT = 5;          // Сколько кусков еды генерировать

// ================== СТРУКТУРА КЛЕТКИ ЛАБИРИНТА ==================
struct Cell
{
    bool north = true;   // Стена сверху (север)
    bool south = true;   // Стена снизу (юг)
    bool east = true;    // Стена справа (восток)
    bool west = true;    // Стена слева (запад)
    int cost = 1;        // Стоимость прохода через клетку (накапливаемые очки)
    bool has_food = false; // Есть ли еда в этой клетке
};

// Глобальные переменные (лабиринт, найденный путь, координаты еды)
std::vector<std::vector<Cell>> maze(HEIGHT, std::vector<Cell>(WIDTH));
std::vector<std::pair<int, int>> path;  // Найденный оптимальный путь (координаты клеток)
std::vector<std::pair<int, int>> food_locations;  // Список всех координат еды

// ================== ГЕНЕРАЦИЯ ЛАБИРИНТА (Алгоритм Крускала) ==================
void generate_maze()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Для алгоритма Крускала: массив родителей для системы непересекающихся множеств
    std::vector<int> parent(WIDTH * HEIGHT);
    for (int i = 0; i < WIDTH * HEIGHT; ++i)
    {
        parent[i] = i;
    }

    // Рекурсивная функция поиска корня множества (Find)
    auto find = [&](auto&& self, int x) -> int
        {
            return parent[x] == x ? x : parent[x] = self(self, parent[x]);
        };

    // Объединение двух множеств (Union)
    auto union_sets = [&](int x, int y)
        {
            parent[find(find, x)] = find(find, y);
        };

    // Создаём список всех возможных стен между соседними клетками
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> walls;
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH - 1; ++x)
        {
            walls.push_back(std::make_pair(std::make_pair(x, y), std::make_pair(x + 1, y)));
        }
    }
    for (int y = 0; y < HEIGHT - 1; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            walls.push_back(std::make_pair(std::make_pair(x, y), std::make_pair(x, y + 1)));
        }
    }

    // Перемешиваем стены для случайного порядка обработки
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(walls.begin(), walls.end(), g);

    // Обрабатываем каждую стену
    for (size_t i = 0; i < walls.size(); ++i)
    {
        const auto& wall = walls[i];
        int x1 = wall.first.first, y1 = wall.first.second;
        int x2 = wall.second.first, y2 = wall.second.second;
        int c1 = y1 * WIDTH + x1;
        int c2 = y2 * WIDTH + x2;

        // Если клетки в разных множествах - убираем стену
        if (find(find, c1) != find(find, c2))
        {
            union_sets(c1, c2);
            if (x1 == x2)
            {
                // Вертикальная стена
                maze[y1][x1].south = false;
                maze[y2][x2].north = false;
            }
            else
            {
                // Горизонтальная стена
                maze[y1][x1].east = false;
                maze[y2][x2].west = false;
            }
        }
        // Иногда оставляем лишние стены для сложности лабиринта
        else if (std::rand() % 10 == 0)
        {
            if (x1 == x2)
            {
                maze[y1][x1].south = false;
                maze[y2][x2].north = false;
            }
            else
            {
                maze[y1][x1].east = false;
                maze[y2][x2].west = false;
            }
        }
    }

    // Назначаем случайные стоимости клеткам (от 1 до 10)
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            maze[y][x].cost = (std::rand() % 10) + 1;
        }
    }

    // Генерируем еду в случайных местах (не на старте и не на финише)
    food_locations.clear();
    int foods_placed = 0;
    while (foods_placed < FOOD_COUNT)
    {
        int rx = std::rand() % WIDTH;
        int ry = std::rand() % HEIGHT;
        if ((rx != 0 || ry != 0) && (rx != WIDTH - 1 || ry != HEIGHT - 1) && !maze[ry][rx].has_food)
        {
            maze[ry][rx].has_food = true;
            food_locations.push_back(std::make_pair(rx, ry));
            foods_placed++;
        }
    }

    // Открываем вход и выход
    maze[0][0].west = false;
    maze[HEIGHT - 1][WIDTH - 1].east = false;
}

// ================== АЛГОРИТМ ДЕЙКСТРЫ С РАСШИРЕННЫМ СОСТОЯНИЕМ ==================

// Состояние агента: позиция + голод + стоимость + маска съеденной еды
struct State
{
    int x, y;               // Координаты клетки
    int hunger;             // Текущий уровень сытости
    int cost;               // Накопленная стоимость (очки)
    unsigned int food_mask; // Битовая маска: какие куски еды уже съедены (до 32)

    // Для приоритетной очереди: сравниваем по стоимости
    bool operator>(const State& o) const
    {
        return cost > o.cost;
    }
};

// Хэш-функция для использования State как ключа в unordered_map
struct StateHash
{
    size_t operator()(const State& s) const
    {
        return ((s.x << 20) ^ (s.y << 15) ^ (s.hunger << 7) ^ s.food_mask);
    }
};

// Функция сравнения состояний для unordered_map
struct StateEqual
{
    bool operator()(const State& a, const State& b) const
    {
        return a.x == b.x && a.y == b.y &&
            a.hunger == b.hunger && a.food_mask == b.food_mask;
    }
};

// Информация о предыдущем состоянии для восстановления пути
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

// Основная функция поиска оптимального пути с учётом голода и еды
void find_shortest_path()
{
    // Максимально возможный уровень голода (чтобы не выйти за границы)
    const int MAX_H = INITIAL_HUNGER + FOOD_RESTORE * FOOD_COUNT + 50;

    // Расстояния и предыдущие состояния храним в хэш-таблицах
    std::unordered_map<State, int, StateHash, StateEqual> dist;
    std::unordered_map<State, PrevInfo, StateHash, StateEqual> prev;

    // Приоритетная очередь для алгоритма Дейкстры
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;

    // Начальное состояние: позиция (0,0), начальный голод, нулевая стоимость, маска пустая
    State start{ 0, 0, INITIAL_HUNGER, 0, 0 };
    dist[start] = 0;
    pq.push(start);

    // Направления движения: вниз, вправо, вверх, влево
    const int dx[4] = { 0, 1, 0, -1 };
    const int dy[4] = { 1, 0, -1, 0 };

    State best_final_state;  // Лучшее конечное состояние
    bool found = false;      // Нашли ли хоть один путь до выхода
    int best_cost = INT_MAX; // Лучшая (минимальная) стоимость

    // Основной цикл алгоритма Дейкстры
    while (!pq.empty())
    {
        State s = pq.top();
        pq.pop();

        // Если это состояние устарело (уже нашли лучше) - пропускаем
        if (dist.find(s) == dist.end() || s.cost > dist[s])
        {
            continue;
        }

        // Если достигли выхода (правый нижний угол)
        if (s.x == WIDTH - 1 && s.y == HEIGHT - 1)
        {
            if (s.cost < best_cost)
            {
                best_cost = s.cost;
                best_final_state = s;
                found = true;
            }
            continue; // Может быть есть путь с той же стоимостью но другим голодом
        }

        // Проверяем, есть ли на текущей клетке еда, и если есть - получаем её индекс
        int food_index = -1;
        unsigned int new_mask = s.food_mask;
        int new_hunger = s.hunger;

        for (size_t i = 0; i < food_locations.size(); ++i)
        {
            if (food_locations[i].first == s.x && food_locations[i].second == s.y)
            {
                food_index = static_cast<int>(i);
                break;
            }
        }

        // Если еда есть и ещё не съедена в этом состоянии
        if (food_index != -1 && ((new_mask >> food_index) & 1) == 0)
        {
            new_mask |= (1u << food_index); // Помечаем еду как съеденную
            new_hunger = std::min(new_hunger + FOOD_RESTORE, MAX_H);
        }

        // Пробуем все 4 возможных направления движения
        for (int d = 0; d < 4; ++d)
        {
            int nx = s.x + dx[d];
            int ny = s.y + dy[d];
            if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT)
            {
                continue; // Выход за границы
            }

            // Проверяем, можно ли пройти в этом направлении (нет стены)
            bool can = (d == 0 && !maze[s.y][s.x].south) ||
                (d == 1 && !maze[s.y][s.x].east) ||
                (d == 2 && !maze[s.y][s.x].north) ||
                (d == 3 && !maze[s.y][s.x].west);

            if (!can)
            {
                continue;
            }

            // Вычисляем голод после шага
            int next_hunger = new_hunger - HUNGER_COST_PER_STEP;
            if (next_hunger <= 0)
            {
                continue; // Умрём от голода - недопустимое состояние
            }

            // Новая стоимость = старая + стоимость клетки, куда переходим
            int new_cost = s.cost + maze[ny][nx].cost;

            // Создаём новое состояние
            State next_state{ nx, ny, next_hunger, new_cost, new_mask };

            // Если нашли путь короче или первый раз видим это состояние
            if (dist.find(next_state) == dist.end() || new_cost < dist[next_state])
            {
                dist[next_state] = new_cost;
                prev[next_state] = PrevInfo(s.hunger, s.x, s.y, s.food_mask);
                pq.push(next_state);
            }
        }
    }

    // Если путь не найден
    if (!found)
    {
        std::cout << "Нет пути — умрёшь от голода!\n";
        path.clear();
        return;
    }

    // ВОССТАНОВЛЕНИЕ ПУТИ ИЗ КОНЕЧНОГО СОСТОЯНИЯ
    path.clear();
    State current = best_final_state;

    while (true)
    {
        // Добавляем текущую клетку в путь
        path.push_back(std::make_pair(current.x, current.y));

        // Если нет предыдущего состояния - достигли начала
        if (prev.find(current) == prev.end() || !prev[current].valid)
        {
            break;
        }

        // Получаем информацию о предыдущем состоянии
        PrevInfo p = prev[current];
        State prev_state{ p.prev_x, p.prev_y, p.prev_hunger, 0, p.prev_mask };

        // Ищем реальное предыдущее состояние в таблице расстояний
        bool found_prev = false;
        for (auto it = dist.begin(); it != dist.end(); ++it)
        {
            const State& key = it->first;
            if (key.x == prev_state.x && key.y == prev_state.y &&
                key.hunger == prev_state.hunger && key.food_mask == prev_state.food_mask)
            {
                current = key;
                found_prev = true;
                break;
            }
        }

        if (!found_prev)
        {
            break;
        }
    }

    // Путь был восстановлен от конца к началу, нужно развернуть
    std::reverse(path.begin(), path.end());

    // Удаляем возможные дубликаты (на всякий случай)
    std::vector<std::pair<int, int>> cleaned_path;
    for (size_t i = 0; i < path.size(); ++i)
    {
        if (i == 0 || path[i] != path[i - 1])
        {
            cleaned_path.push_back(path[i]);
        }
    }
    path = cleaned_path;

    // Вывод информации о найденном пути
    std::cout << "Оптимальный путь найден: " << (path.size() - 1)
        << " шагов, стоимость " << best_cost << "\n";

    // Вывод информации о том, какая еда была использована
    std::cout << "Использованная еда: ";
    for (size_t i = 0; i < food_locations.size(); ++i)
    {
        if ((best_final_state.food_mask >> i) & 1)
        {
            std::cout << "(" << food_locations[i].first << ","
                << food_locations[i].second << ") ";
        }
    }
    std::cout << "\n";
}

// ================== ВЫВОД КОНФИГУРАЦИИ ЛАБИРИНТА В КОНСОЛЬ ==================
void print_maze_config()
{
    std::cout << "=== КОНФИГУРАЦИЯ ЛАБИРИНТА ===\n";
    std::cout << "Еда:\n";
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            if (maze[y][x].has_food)
            {
                std::cout << "(" << x << "," << y << ") ";
            }
        }
    }
    std::cout << "\n\nСтоимости клеток:\n";
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            std::cout << maze[y][x].cost << " ";
        }
        std::cout << "\n";
    }
    std::cout << "===============================\n\n";
}

// ================== ГЛАВНАЯ ФУНКЦИЯ ==================
int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);  // Для корректного вывода русского текста в Windows
#endif

    // Генерируем лабиринт и находим путь
    generate_maze();
    find_shortest_path();
    print_maze_config();

    // Создаём окно для отрисовки
    sf::RenderWindow window(sf::VideoMode(WIDTH * CELL_SIZE, HEIGHT * CELL_SIZE),
        "Лабиринт с голодом — нажми 1");
    window.setFramerateLimit(60);

    // Загружаем шрифт для отображения стоимости клеток
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        font.loadFromFile("C:/Windows/Fonts/calibri.ttf");
    }

    // Переменные для анимации
    bool animating = false;       // Идёт ли анимация
    float progress = 0.0f;        // Прогресс анимации (текущий шаг)
    int current_step = 0;         // Текущий номер шага
    int current_hunger = INITIAL_HUNGER;  // Текущий голод во время анимации
    int total_points = 0;         // Накопленные очки во время анимации
    unsigned int current_food_mask = 0;   // Маска съеденной еды во время анимации

    // Сохраняем оригинальные данные для перезапуска
    auto original_maze = maze;
    auto original_food_locations = food_locations;

    // Главный цикл программы
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            // Нажатие 1 - перезапуск алгоритма и анимации
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num1)
            {
                maze = original_maze;
                food_locations = original_food_locations;
                find_shortest_path();
                animating = true;
                progress = 0.0f;
                current_step = 0;
                current_hunger = INITIAL_HUNGER;
                total_points = 0;
                current_food_mask = 0;
            }
        }

        window.clear(sf::Color::Black);

        // ОТРИСОВКА ЛАБИРИНТА
        for (int y = 0; y < HEIGHT; ++y)
        {
            for (int x = 0; x < WIDTH; ++x)
            {
                // Квадрат клетки
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                cell.setPosition(static_cast<float>(x * CELL_SIZE),
                    static_cast<float>(y * CELL_SIZE));

                // Цвет клетки в зависимости от содержимого
                if (maze[y][x].has_food)
                {
                    cell.setFillColor(sf::Color(139, 69, 19));  // Коричневый - еда
                }
                else if (x == 0 && y == 0)
                {
                    cell.setFillColor(sf::Color::Yellow);       // Жёлтый - старт
                }
                else if (x == WIDTH - 1 && y == HEIGHT - 1)
                {
                    cell.setFillColor(sf::Color::Red);          // Красный - финиш
                }
                else
                {
                    // Градиент от синего: чем выше стоимость, тем темнее
                    int intensity = 255 - (maze[y][x].cost - 1) * 20;
                    cell.setFillColor(sf::Color(50, 100, intensity));
                }
                window.draw(cell);

                // Отображение стоимости клетки
                sf::Text text(std::to_string(maze[y][x].cost), font, 16);
                text.setFillColor(sf::Color::Red);
                text.setStyle(sf::Text::Bold);
                sf::FloatRect bounds = text.getLocalBounds();
                text.setOrigin(bounds.left + bounds.width / 2,
                    bounds.top + bounds.height / 2);
                text.setPosition(static_cast<float>(x * CELL_SIZE + CELL_SIZE / 2),
                    static_cast<float>(y * CELL_SIZE + CELL_SIZE / 2));
                window.draw(text);

                // Отрисовка стен
                sf::RectangleShape wall;
                wall.setFillColor(sf::Color::White);

                // Верхняя стена (север)
                if (maze[y][x].north)
                {
                    wall.setSize(sf::Vector2f(CELL_SIZE, 4));
                    wall.setPosition(static_cast<float>(x * CELL_SIZE),
                        static_cast<float>(y * CELL_SIZE));
                    window.draw(wall);
                }

                // Нижняя стена (юг)
                if (maze[y][x].south)
                {
                    wall.setSize(sf::Vector2f(CELL_SIZE, 4));
                    wall.setPosition(static_cast<float>(x * CELL_SIZE),
                        static_cast<float>((y + 1) * CELL_SIZE - 4));
                    window.draw(wall);
                }

                // Левая стена (запад)
                if (maze[y][x].west)
                {
                    wall.setSize(sf::Vector2f(4, CELL_SIZE));
                    wall.setPosition(static_cast<float>(x * CELL_SIZE),
                        static_cast<float>(y * CELL_SIZE));
                    window.draw(wall);
                }

                // Правая стена (восток)
                if (maze[y][x].east)
                {
                    wall.setSize(sf::Vector2f(4, CELL_SIZE));
                    wall.setPosition(static_cast<float>((x + 1) * CELL_SIZE - 4),
                        static_cast<float>(y * CELL_SIZE));
                    window.draw(wall);
                }
            }
        }

        // ОТРИСОВКА НАЙДЕННОГО ПУТИ (зелёная линия)
        for (size_t i = 1; i < path.size(); ++i)
        {
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(
                    static_cast<float>(path[i - 1].first * CELL_SIZE + CELL_SIZE / 2),
                    static_cast<float>(path[i - 1].second * CELL_SIZE + CELL_SIZE / 2)),
                    sf::Color(0, 255, 0, 120)),

                sf::Vertex(sf::Vector2f(
                    static_cast<float>(path[i].first * CELL_SIZE + CELL_SIZE / 2),
                    static_cast<float>(path[i].second * CELL_SIZE + CELL_SIZE / 2)),
                    sf::Color(0, 255, 0, 120))
            };
            window.draw(line, 2, sf::Lines);
        }

        // АНИМАЦИЯ ДВИЖЕНИЯ ИГРОКА
        if (animating && path.size() > 1)
        {
            // Увеличиваем прогресс анимации (скорость)
            progress += 0.02f;

            // Если достигли конца пути
            if (progress >= path.size() - 1)
            {
                progress = path.size() - 1;
                animating = false;
                std::cout << "Финиш! Накоплено очков: " << total_points << "\n";
            }

            // Вычисляем текущую позицию игрока с учётом плавности
            size_t idx = static_cast<size_t>(progress);
            float frac = progress - idx;

            float pos_x = static_cast<float>(path[idx].first * CELL_SIZE + CELL_SIZE / 2);
            float pos_y = static_cast<float>(path[idx].second * CELL_SIZE + CELL_SIZE / 2);

            if (idx + 1 < path.size())
            {
                pos_x += frac * (path[idx + 1].first - path[idx].first) * CELL_SIZE;
                pos_y += frac * (path[idx + 1].second - path[idx].second) * CELL_SIZE;
            }

            // Отрисовка игрока (жёлтый круг)
            sf::CircleShape player(13);
            player.setFillColor(sf::Color::Yellow);
            player.setOutlineColor(sf::Color::Black);
            player.setOutlineThickness(2);
            player.setPosition(pos_x - 13, pos_y - 13);
            window.draw(player);

            // ОБРАБОТКА ШАГА АНИМАЦИИ
            int step = static_cast<int>(progress + 0.01f);
            if (step > current_step && step < static_cast<int>(path.size()))
            {
                current_step = step;
                int cx = path[step].first;
                int cy = path[step].second;

                // Проверяем, есть ли на этой клетке еда и не съели ли мы её уже
                bool food_found = false;
                int food_index = -1;
                for (size_t i = 0; i < food_locations.size(); ++i)
                {
                    if (food_locations[i].first == cx && food_locations[i].second == cy)
                    {
                        food_index = static_cast<int>(i);
                        break;
                    }
                }

                // Если нашли несъеденную еду
                if (food_index != -1 && ((current_food_mask >> food_index) & 1) == 0)
                {
                    current_hunger += FOOD_RESTORE;            // Восстанавливаем голод
                    current_food_mask |= (1u << food_index);   // Помечаем еду как съеденную
                    maze[cy][cx].has_food = false;             // Убираем визуально еду
                    std::cout << "Еда съедена на (" << cx << "," << cy << ")! +20 сытости\n";
                }

                // Тратим голод за шаг (кроме самого первого шага)
                if (step > 0)
                {
                    current_hunger -= HUNGER_COST_PER_STEP;
                    total_points += maze[cy][cx].cost;
                }

                // Проверяем, не умер ли от голода
                if (current_hunger <= 0)
                {
                    std::cout << "УМЕР ОТ ГОЛОДА НА ХОДУ " << current_step << "!\n";
                    animating = false;
                }
                else
                {
                    // Вывод информации о текущем шаге
                    std::cout << "Ход " << current_step << "/" << (path.size() - 1)
                        << " | Позиция (" << cx << ", " << cy
                        << ") | Сытость: " << current_hunger
                        << " | Очков: " << total_points << "\n";
                }
            }
        }

        window.display();
    }

    return 0;
}