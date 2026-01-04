// tests.hpp
// Тесты для алгоритмов на графах

#ifndef TESTS_HPP
#define TESTS_HPP

#include "GraphAlgorithms.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace GraphAlgorithms;

// Вспомогательная функция для сравнения double в тестах
inline bool testDoubleEquals(double a, double b)
{
    return std::fabs(a - b) < 1e-9;
}

// Тест 1: Пустой граф
inline bool testEmptyGraph()
{
    std::cout << "Тест 1: Пустой граф... ";

    Graph g(0);
    auto dist = g.shortestPaths(0);
    auto components = g.connectedComponents();

    assert(dist.size() == 0);
    assert(components.size() == 0);

    std::cout << "OK\n";
    return true;
}

// Тест 2: Граф с одной вершиной
inline bool testSingleVertex()
{
    std::cout << "Тест 2: Граф с одной вершиной... ";

    Graph g(1);
    auto dist = g.shortestPaths(0);
    auto components = g.connectedComponents();

    assert(dist.size() == 1);
    assert(testDoubleEquals(dist[0], 0.0));
    assert(components.size() == 1);
    assert(components[0].size() == 1);
    assert(components[0][0] == 0);

    std::cout << "OK\n";
    return true;
}

// Тест 3: Базовый тест Дейкстры
inline bool testDijkstraBasic()
{
    std::cout << "Тест 3: Базовый тест Дейкстры... ";

    Graph g(5);
    g.addDirectedEdge(0, 1, 4);
    g.addDirectedEdge(0, 2, 1);
    g.addDirectedEdge(2, 1, 2);
    g.addDirectedEdge(1, 3, 1);
    g.addDirectedEdge(2, 3, 5);
    g.addDirectedEdge(3, 4, 3);

    auto dist = g.shortestPaths(0);

    assert(testDoubleEquals(dist[0], 0.0));
    assert(testDoubleEquals(dist[1], 3.0));  // 0->2->1
    assert(testDoubleEquals(dist[2], 1.0));  // 0->2
    assert(testDoubleEquals(dist[3], 4.0));  // 0->2->1->3
    assert(testDoubleEquals(dist[4], 7.0));  // 0->2->1->3->4

    std::cout << "OK\n";
    return true;
}

// Тест 4: Дейкстра с несвязным графом
inline bool testDijkstraDisconnected()
{
    std::cout << "Тест 4: Дейкстра с несвязным графом... ";

    Graph g(4);
    g.addDirectedEdge(0, 1, 2);
    g.addDirectedEdge(1, 2, 3);
    // Вершина 3 не связана

    auto dist = g.shortestPaths(0);

    assert(testDoubleEquals(dist[0], 0.0));
    assert(testDoubleEquals(dist[1], 2.0));
    assert(testDoubleEquals(dist[2], 5.0));
    assert(!std::isfinite(dist[3]));  // Бесконечность

    std::cout << "OK\n";
    return true;
}

// Тест 5: Компоненты связности
inline bool testConnectedComponents()
{
    std::cout << "Тест 5: Компоненты связности... ";

    Graph g(6, false);
    g.addUndirectedEdge(0, 1);
    g.addUndirectedEdge(1, 2);
    g.addUndirectedEdge(3, 4);
    // Вершина 5 - изолированная

    auto components = g.connectedComponents();

    assert(components.size() == 3);

    // Сортируем компоненты для удобства проверки
    for (auto& comp : components)
    {
        std::sort(comp.begin(), comp.end());
    }

    // Ищем компоненты
    bool found1 = false, found2 = false, found3 = false;
    for (const auto& comp : components)
    {
        if (comp.size() == 3 && comp[0] == 0 && comp[1] == 1 && comp[2] == 2)
        {
            found1 = true;
        }
        else if (comp.size() == 2 && comp[0] == 3 && comp[1] == 4)
        {
            found2 = true;
        }
        else if (comp.size() == 1 && comp[0] == 5)
        {
            found3 = true;
        }
    }

    assert(found1 && found2 && found3);

    std::cout << "OK\n";
    return true;
}

// Тест 6: Одна компонента связности
inline bool testConnectedComponentsSingle()
{
    std::cout << "Тест 6: Одна компонента связности... ";

    Graph g(4, false);
    g.addUndirectedEdge(0, 1);
    g.addUndirectedEdge(1, 2);
    g.addUndirectedEdge(2, 3);
    g.addUndirectedEdge(3, 0);

    auto components = g.connectedComponents();

    assert(components.size() == 1);
    assert(components[0].size() == 4);

    std::sort(components[0].begin(), components[0].end());
    assert(components[0][0] == 0);
    assert(components[0][1] == 1);
    assert(components[0][2] == 2);
    assert(components[0][3] == 3);

    std::cout << "OK\n";
    return true;
}

// Тест 7: Топологическая сортировка
inline bool testTopologicalSort()
{
    std::cout << "Тест 7: Топологическая сортировка... ";

    Graph g(6);
    g.addDirectedEdge(5, 2);
    g.addDirectedEdge(5, 0);
    g.addDirectedEdge(4, 0);
    g.addDirectedEdge(4, 1);
    g.addDirectedEdge(2, 3);
    g.addDirectedEdge(3, 1);

    auto order = g.topologicalSort();

    assert(order.size() == 6);

    // Проверяем корректность топологического порядка
    std::vector<int> pos(6);
    for (size_t i = 0; i < order.size(); ++i)
    {
        pos[order[i]] = i;
    }

    // Проверяем все ребра
    assert(pos[5] < pos[2]);  // 5 -> 2
    assert(pos[5] < pos[0]);  // 5 -> 0
    assert(pos[4] < pos[0]);  // 4 -> 0
    assert(pos[4] < pos[1]);  // 4 -> 1
    assert(pos[2] < pos[3]);  // 2 -> 3
    assert(pos[3] < pos[1]);  // 3 -> 1

    std::cout << "OK\n";
    return true;
}

// Тест 8: Топологическая сортировка с циклом
inline bool testTopologicalSortCycle()
{
    std::cout << "Тест 8: Топологическая сортировка с циклом... ";

    Graph g(3);
    g.addDirectedEdge(0, 1);
    g.addDirectedEdge(1, 2);
    g.addDirectedEdge(2, 0);  // Цикл!

    bool hasException = false;
    try
    {
        auto order = g.topologicalSort();
    }
    catch (const std::runtime_error& e)
    {
        hasException = true;
        assert(std::string(e.what()) == "Граф содержит цикл, топологическая сортировка невозможна");
    }

    assert(hasException);

    std::cout << "OK\n";
    return true;
}

// Тест 9: Интеграционный тест всех алгоритмов
inline bool testAllAlgorithmsIntegration()
{
    std::cout << "Тест 9: Интеграционный тест всех алгоритмов... ";

    Graph g(8, false);

    // Компонента 1
    g.addUndirectedEdge(0, 1);
    g.addUndirectedEdge(1, 2);
    g.addUndirectedEdge(2, 0);

    // Компонента 2
    g.addUndirectedEdge(3, 4);
    g.addUndirectedEdge(4, 5);
    g.addUndirectedEdge(5, 3);

    auto components = g.connectedComponents();

    assert(components.size() == 4);  // 2 компоненты + 2 изолированные вершины

    std::vector<size_t> sizes;
    for (const auto& comp : components)
    {
        sizes.push_back(comp.size());
    }
    std::sort(sizes.begin(), sizes.end());

    assert(sizes[0] == 1);  // вершина 6
    assert(sizes[1] == 1);  // вершина 7
    assert(sizes[2] == 3);  // компонента 1
    assert(sizes[3] == 3);  // компонента 2

    std::cout << "OK\n";
    return true;
}

// Тест 10: Дейкстра только с положительными весами
inline bool testDijkstraWithPositiveWeights()
{
    std::cout << "Тест 10: Дейкстра с положительными весами... ";

    Graph g(4);
    g.addDirectedEdge(0, 1, 1.5);
    g.addDirectedEdge(0, 2, 2.0);
    g.addDirectedEdge(1, 3, 1.0);
    g.addDirectedEdge(2, 3, 0.5);

    auto dist = g.shortestPaths(0);

    assert(testDoubleEquals(dist[0], 0.0));
    assert(testDoubleEquals(dist[1], 1.5));
    assert(testDoubleEquals(dist[2], 2.0));
    assert(testDoubleEquals(dist[3], 2.5));

    std::cout << "OK\n";
    return true;
}

// Главная функция для запуска всех тестов
inline bool runAllTests()
{
    std::cout << "=== Запуск тестов алгоритмов на графах ===\n\n";

    bool allPassed = true;

    try
    {
        allPassed &= testEmptyGraph();
        allPassed &= testSingleVertex();
        allPassed &= testDijkstraBasic();
        allPassed &= testDijkstraDisconnected();
        allPassed &= testConnectedComponents();
        allPassed &= testConnectedComponentsSingle();
        allPassed &= testTopologicalSort();
        allPassed &= testTopologicalSortCycle();
        allPassed &= testAllAlgorithmsIntegration();
        allPassed &= testDijkstraWithPositiveWeights();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Ошибка при выполнении тестов: " << e.what() << "\n";
        return false;
    }
    catch (...)
    {
        std::cerr << "Неизвестная ошибка при выполнении тестов\n";
        return false;
    }

    if (allPassed)
    {
        std::cout << "\ Все тесты пройдены \n";
    }
    else
    {
        std::cout << "\n тесты не пройдены \n";
    }

    return allPassed;
}

#endif // TESTS_HPP