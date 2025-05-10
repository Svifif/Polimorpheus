#pragma once
#include "Vector.h"

template<typename T>
class DiagonalMatrix
{
private:
    DynamicArray<T> diag;

public:
    explicit DiagonalMatrix(size_t n) : diag(0) // Сначала пустой
    {
        for (size_t i = 0; i < n; ++i) {
            diag.push_back(T()); // Явно добавляем N элементов
        }
    }

    size_t size() const { return diag.get_size(); }

    T& operator()(size_t i) { return diag.get(i); }

    const T& operator()(size_t i) const { return diag.get(i); }

    // Умножение на вектор
    Vector<T> operator*(const Vector<T>& v) const
    {
        if (size() != v.size())
        {
            throw std::invalid_argument("Size mismatch");
        }

        Vector<T> result(size());
        for (size_t i = 0; i < size(); ++i)
        {
            result[i] = diag.get(i) * v[i];
        }
        return result;
    }

    // Норма матрицы
    double norm() const
    {
        double sum = 0;
        for (size_t i = 0; i < size(); ++i)
        {
            sum += std::norm(diag.get(i));
        }
        return sqrt(sum);
    }
};