#ifndef GENERATE_MAZE_HPP
#define GENERATE_MAZE_HPP

#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>

// Константы лабиринта
const int WIDTH = 20;                // Ширина лабиринта в клетках
const int HEIGHT = 20;               // Высота лабиринта в клетках
const int CELL_SIZE = 30;            // Размер клетки в пикселях (для отрисовки)
const int INITIAL_HUNGER = 20;       // Начальный уровень голода персонажа
const int HUNGER_COST_PER_STEP = 1;  // Голод за шаг
const int FOOD_RESTORE = 20;         // Восстановление голода от еды
const int FOOD_COUNT = 8;            // Количество еды на уровне

// Структура клетки лабиринта
struct Cell
{
    bool north = true;   // Стена сверху
    bool south = true;   // Стена снизу
    bool east = true;    // Стена справа
    bool west = true;    // Стена слева
    int cost = 1;        // Стоимость прохода (для алгоритмов поиска пути)
    bool has_food = false; // Наличие еды в клетке
};

// Глобальные переменные (определяются в другом месте)
extern std::vector<std::vector<Cell>> maze;            // Двумерный массив клеток
extern std::vector<std::pair<int, int>> food_locations; // Координаты еды
extern std::pair<int, int> enemy_start;                // Стартовая позиция врага

/**
 * Генерирует случайный лабиринт с использованием алгоритма Краскала.
 * Асимптотика: O(N^2) где N = WIDTH*HEIGHT (в худшем случае обработка всех стен)
 * Реальная сложность ближе к O(N) благодаря системе непересекающихся множеств.
 */
void generate_maze()
{
    // Инициализация лабиринта
    maze = std::vector<std::vector<Cell>>(HEIGHT, std::vector<Cell>(WIDTH));

    // Инициализация генератора случайных чисел
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Система непересекающихся множеств (DSU) для алгоритма Краскала
    std::vector<int> parent(WIDTH * HEIGHT);
    for (int i = 0; i < WIDTH * HEIGHT; ++i)
    {
        parent[i] = i;  // Каждая клетка - отдельное множество
    }

    // Лямбда-функция для нахождения корня множества (сжатие путей)
    auto find = [&](auto&& self, int x) -> int
        {
            if (parent[x] == x) return x;
            parent[x] = self(self, parent[x]);  // Рекурсивное сжатие пути
            return parent[x];
        };

    // Лямбда-функция для объединения двух множеств
    auto union_sets = [&](int x, int y)
        {
            int root_x = find(find, x);
            int root_y = find(find, y);
            parent[root_x] = root_y;  // Присоединяем одно множество к другому
        };

    // Создание списка всех возможных стен между соседними клетками
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> walls;

    // Добавление вертикальных стен (между клетками по горизонтали)
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH - 1; ++x)
        {
            walls.push_back(std::make_pair(std::make_pair(x, y), std::make_pair(x + 1, y)));
        }
    }

    // Добавление горизонтальных стен (между клетками по вертикали)
    for (int y = 0; y < HEIGHT - 1; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            walls.push_back(std::make_pair(std::make_pair(x, y), std::make_pair(x, y + 1)));
        }
    }

    // Перемешивание стен для случайного порядка обработки
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(walls.begin(), walls.end(), g);

    // Основной цикл алгоритма Краскала
    for (size_t i = 0; i < walls.size(); ++i)
    {
        const auto& wall = walls[i];
        int x1 = wall.first.first, y1 = wall.first.second;
        int x2 = wall.second.first, y2 = wall.second.second;

        // Преобразование координат в индексы DSU
        int c1 = y1 * WIDTH + x1;
        int c2 = y2 * WIDTH + x2;

        // Если клетки в разных множествах - удаляем стену (объединяем множества)
        if (find(find, c1) != find(find, c2))
        {
            union_sets(c1, c2);

            // Удаление соответствующей стены в структуре лабиринта
            if (x1 == x2)  // Горизонтальная стена
            {
                maze[y1][x1].south = false;
                maze[y2][x2].north = false;
            }
            else  // Вертикальная стена
            {
                maze[y1][x1].east = false;
                maze[y2][x2].west = false;
            }
        }
        // С вероятностью 10% удаляем стену даже если множества уже соединены
        // Это создает дополнительные проходы в лабиринте
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

    // Назначение случайных стоимостей клеткам (от 1 до 10)
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            maze[y][x].cost = (std::rand() % 10) + 1;
        }
    }

    // Размещение еды в случайных клетках лабиринта
    food_locations.clear();
    int foods_placed = 0;
    while (foods_placed < FOOD_COUNT)
    {
        int rx = std::rand() % WIDTH;
        int ry = std::rand() % HEIGHT;

        // Еда не размещается в стартовой и конечной клетках
        if ((rx != 0 || ry != 0) &&
            (rx != WIDTH - 1 || ry != HEIGHT - 1) &&
            !maze[ry][rx].has_food)
        {
            maze[ry][rx].has_food = true;
            food_locations.push_back(std::make_pair(rx, ry));
            foods_placed++;
        }
    }

    // Гарантированное открытие входа (левый верхний угол) и выхода (правый нижний)
    maze[0][0].west = false;                    // Открываем вход слева
    maze[HEIGHT - 1][WIDTH - 1].east = false;   // Открываем выход справа

    // Установка стартовой позиции врага (правый нижний угол)
    enemy_start = std::make_pair(WIDTH - 1, HEIGHT - 1);
}

#endif