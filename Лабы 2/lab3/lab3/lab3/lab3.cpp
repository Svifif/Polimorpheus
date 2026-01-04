// main.cpp
// Лабораторная работа №3 - Алгоритмы на графах

#include "GraphAlgorithms.hpp"
#include "tests.hpp"
#include <iostream>
#include <iomanip>

// Демонстрация работы алгоритмов
void demonstrateAlgorithms()
{
    std::cout << "\n=== Демонстрация работы алгоритмов ===\n\n";

    // Пример 1: Кратчайшие пути
    std::cout << "Пример 1: Поиск кратчайших путей (Дейкстра)\n";
    Graph g1(6);

    g1.addDirectedEdge(0, 1, 7);
    g1.addDirectedEdge(0, 2, 9);
    g1.addDirectedEdge(0, 5, 14);
    g1.addDirectedEdge(1, 2, 10);
    g1.addDirectedEdge(1, 3, 15);
    g1.addDirectedEdge(2, 3, 11);
    g1.addDirectedEdge(2, 5, 2);
    g1.addDirectedEdge(3, 4, 6);
    g1.addDirectedEdge(4, 5, 9);

    auto dist1 = g1.shortestPaths(0);
    std::cout << "Кратчайшие пути от вершины 0:\n";
    for (size_t i = 0; i < dist1.size(); ++i)
    {
        std::cout << "  до вершины " << i << ": ";
        if (std::isfinite(dist1[i]))
        {
            std::cout << std::fixed << std::setprecision(2) << dist1[i];
        }
        else
        {
            std::cout << "нет пути";
        }
        std::cout << "\n";
    }

    // Пример 2: Компоненты связности
    std::cout << "\nПример 2: Поиск компонент связности\n";
    Graph g2(8, false);

    g2.addUndirectedEdge(0, 1);
    g2.addUndirectedEdge(1, 2);
    g2.addUndirectedEdge(3, 4);
    g2.addUndirectedEdge(4, 5);
    g2.addUndirectedEdge(6, 7);

    auto components = g2.connectedComponents();
    std::cout << "Найдено " << components.size() << " компонент связности:\n";
    for (size_t i = 0; i < components.size(); ++i)
    {
        std::cout << "  Компонента " << i << ": ";
        for (size_t j = 0; j < components[i].size(); ++j)
        {
            std::cout << components[i][j] << " ";
        }
        std::cout << "\n";
    }

    // Пример 3: Топологическая сортировка
    std::cout << "\nПример 3: Топологическая сортировка\n";
    Graph g3(7);

    g3.addDirectedEdge(0, 1);
    g3.addDirectedEdge(0, 2);
    g3.addDirectedEdge(1, 3);
    g3.addDirectedEdge(1, 4);
    g3.addDirectedEdge(2, 4);
    g3.addDirectedEdge(3, 5);
    g3.addDirectedEdge(4, 5);
    g3.addDirectedEdge(5, 6);

    try
    {
        auto order = g3.topologicalSort();
        std::cout << "Топологический порядок вершин: ";
        for (size_t i = 0; i < order.size(); ++i)
        {
            std::cout << order[i] << " ";
        }
        std::cout << "\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "Ошибка: " << e.what() << "\n";
    }
}

int main()
{

    // Запуск тестов
    bool testsPassed = runAllTests();

    if (testsPassed)
    {
        // Демонстрация работы алгоритмов
        demonstrateAlgorithms();

        std::cout << "Работа программы завершена успешно!\n";

        return 0;
    }
    else
    {
        std::cout << "\nТесты не пройдены. \n";
        return 1;
    }
}