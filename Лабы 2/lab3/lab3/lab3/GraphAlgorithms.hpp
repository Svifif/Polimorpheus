// GraphAlgorithms.hpp
// Лабораторная работа №3 — Алгоритмы на графах
// Реализация: кратчайшие пути (Дейкстра), компоненты связности (DFS), топологическая сортировка

#ifndef GRAPH_ALGORITHMS_HPP
#define GRAPH_ALGORITHMS_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <cassert>
#include <cmath>

namespace GraphAlgorithms
{
    // Константа для сравнения вещественных чисел
    const double EPSILON = 1e-9;

    // Вспомогательная функция для сравнения double
    inline bool doubleEquals(double a, double b)
    {
        return std::fabs(a - b) < EPSILON;
    }

    // Класс для представления графа
    class Graph
    {
    private:
        int V; // Количество вершин
        std::vector<std::vector<std::pair<int, double>>> adj; // Список смежности для взвешенного графа
        std::vector<std::vector<int>> adjUnweighted; // Список смежности для невзвешенного графа

    public:
        // Конструктор
        Graph(int vertices, bool weighted = true) : V(vertices)
        {
            if (weighted)
            {
                adj.resize(V);
            }
            else
            {
                adjUnweighted.resize(V);
            }
        }

        // Метод добавления ориентированного ребра с весом
        void addDirectedEdge(int from, int to, double weight = 1.0)
        {
            adj[from].push_back(std::make_pair(to, weight));
        }

        // Метод добавления неориентированного ребра (без веса)
        void addUndirectedEdge(int u, int v)
        {
            adjUnweighted[u].push_back(v);
            adjUnweighted[v].push_back(u);
        }

        // Алгоритм Дейкстры для поиска кратчайших путей от стартовой вершины
        std::vector<double> shortestPaths(int start) const
        {
            assert(start >= 0 && start < V);

            std::vector<double> dist(V, std::numeric_limits<double>::infinity());
            dist[start] = 0.0;

            typedef std::pair<double, int> pq_element;
            std::priority_queue<pq_element, std::vector<pq_element>, std::greater<pq_element>> pq;
            pq.push(std::make_pair(0.0, start));

            while (!pq.empty())
            {
                double currentDist = pq.top().first;
                int u = pq.top().second;
                pq.pop();

                if (currentDist > dist[u])
                {
                    continue;
                }

                for (size_t i = 0; i < adj[u].size(); ++i)
                {
                    int v = adj[u][i].first;
                    double weight = adj[u][i].second;

                    if (dist[u] + weight < dist[v])
                    {
                        dist[v] = dist[u] + weight;
                        pq.push(std::make_pair(dist[v], v));
                    }
                }
            }
            return dist;
        }

        // Поиск компонент связности в неориентированном графе с помощью DFS
        std::vector<std::vector<int>> connectedComponents() const
        {
            std::vector<bool> visited(V, false);
            std::vector<std::vector<int>> components;

            std::function<void(int, std::vector<int>&)> dfs = [&](int node, std::vector<int>& comp)
                {
                    visited[node] = true;
                    comp.push_back(node);

                    for (size_t i = 0; i < adjUnweighted[node].size(); ++i)
                    {
                        int neighbor = adjUnweighted[node][i];
                        if (!visited[neighbor])
                        {
                            dfs(neighbor, comp);
                        }
                    }
                };

            for (int i = 0; i < V; ++i)
            {
                if (!visited[i])
                {
                    std::vector<int> comp;
                    dfs(i, comp);
                    components.push_back(comp);
                }
            }
            return components;
        }

        // Топологическая сортировка для ориентированного ациклического графа (DAG)
        std::vector<int> topologicalSort() const
        {
            std::vector<int> inDegree(V, 0);
            for (int u = 0; u < V; ++u)
            {
                for (size_t i = 0; i < adj[u].size(); ++i)
                {
                    int v = adj[u][i].first;
                    inDegree[v]++;
                }
            }

            std::queue<int> q;
            for (int i = 0; i < V; ++i)
            {
                if (inDegree[i] == 0)
                {
                    q.push(i);
                }
            }

            std::vector<int> order;

            while (!q.empty())
            {
                int u = q.front();
                q.pop();
                order.push_back(u);

                for (size_t i = 0; i < adj[u].size(); ++i)
                {
                    int v = adj[u][i].first;
                    inDegree[v]--;
                    if (inDegree[v] == 0)
                    {
                        q.push(v);
                    }
                }
            }

            if (order.size() != (size_t)V)
            {
                throw std::runtime_error("Граф содержит цикл, топологическая сортировка невозможна");
            }
            return order;
        }
    };
}

#endif // GRAPH_ALGORITHMS_HPP