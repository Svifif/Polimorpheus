#pragma once
#ifndef MAZEGENERATOR_HPP
#define MAZEGENERATOR_HPP

#include "MazeData.hpp"
#include <vector>
#include <random>
#include <utility>

class MazeGenerator {
private:
    std::vector<std::vector<Cell>> grid;
    std::vector<std::pair<int, int>> startPoints;
    std::pair<int, int> endPoint;

    // Генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen;

    int width;
    int height;

    // Приватные методы
    void initializeMaze();
    void generateFromCell(int x, int y);

public:
    // Конструкторы
    MazeGenerator(int w = 31, int h = 21);

    // Основные методы
    void generateMaze();
    void generateNew();

    // Геттеры
    const std::vector<std::vector<Cell>>& getGrid() const { return grid; }
    const std::vector<std::pair<int, int>>& getStartPoints() const { return startPoints; }
    const std::pair<int, int>& getEndPoint() const { return endPoint; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    Cell getCell(int x, int y) const;

    // Поиск путей
    std::vector<MazePath> findAllPaths() const;
    MazePath findPathBFS(const std::pair<int, int>& start) const;
    MazePath findMinCostPath(const std::pair<int, int>& start) const;

    // Отметка путей на карте
    void markPaths(const std::vector<MazePath>& paths);
    void clearPaths();

    // Утилиты
    void printToConsole() const;
    void exportToFile(const std::string& filename) const;
};

#endif // MAZEGENERATOR_HPP