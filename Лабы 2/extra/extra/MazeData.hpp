#pragma once
#ifndef MAZEDATA_HPP
#define MAZEDATA_HPP

#include <vector>
#include <utility>

// Структура для представления клетки лабиринта
struct Cell {
    bool visited = false;
    bool wall = true;           // true - стена, false - проход
    bool isPath = false;
    int cost = 1;               // стоимость прохождения клетки (1-9)

    // Для отслеживания путей
    bool onSolutionPath = false;
    bool isStart = false;
    bool isEnd = false;

    Cell() = default;
    Cell(bool w, int c) : wall(w), cost(c) {}
};

// Структура для хранения пути в лабиринте
struct MazePath {
    std::vector<std::pair<int, int>> path;  // координаты клеток пути
    int totalCost = 0;                      // общая стоимость пути
    int length = 0;                         // длина пути в клетках
};

#endif // MAZEDATA_HPP