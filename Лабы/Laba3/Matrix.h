#pragma once
#include "DynamicArray.hpp"
#include <stdexcept>
#include <cmath>
#include <complex>
#include <type_traits>

template<typename T>
class Matrix 
{
protected:
    size_t rows, cols;
    DynamicArray<DynamicArray<T>> data;

    static double complexNorm(const std::complex<double>& c) 
    {
        return std::norm(c);
    }

    static double numberNorm(const T& value) 
    {
        return static_cast<double>(value) * static_cast<double>(value);
    }

public:
    Matrix(size_t r, size_t c) : rows(r), cols(c) 
    {
        for (size_t i = 0; i < r; ++i) 
        {
            DynamicArray<T> row(c); // Создаем строку нужного размера
            for (size_t j = 0; j < c; ++j)
            {
                row.push_back(T()); // Явно добавляем нулевые элементы
            }
            data.push_back(std::move(row)); // Безопасно добавляем
        }
    }

    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }

    DynamicArray<T>& operator[](size_t row)
    {
        if (row >= rows) throw std::out_of_range("Row index out of range");
        return data.get(row);
    }

    const DynamicArray<T>& operator[](size_t row) const 
    {
        if (row >= rows) throw std::out_of_range("Row index out of range");
        return data.get(row);
    }

    Matrix<T> operator+(const Matrix<T>& other) const 
    {
        if (rows != other.rows || cols != other.cols)
        {
            throw std::invalid_argument("Matrix dimensions don't match");
        }

        Matrix<T> result(rows, cols);
        for (size_t i = 0; i < rows; ++i) 
        {
            for (size_t j = 0; j < cols; ++j) 
            {
                result[i][j] = (*this)[i][j] + other[i][j];
            }
        }
        return result;
    }

    Matrix<T> operator*(const T& scalar) const
    {
        Matrix<T> result(rows, cols);
        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < cols; ++j)
            {
                result[i][j] = (*this)[i][j] * scalar;
            }
        }
        return result;
    }

    double norm() const
    {
        double sum = 0;
        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < cols; ++j) 
            {
                if constexpr (std::is_same_v<T, std::complex<double>>) 
                {
                    sum += complexNorm((*this)[i][j]);
                }
                else
                {
                    sum += numberNorm((*this)[i][j]);
                }
            }
        }
        return std::sqrt(sum);
    }

    // Добавим конструктор перемещения для эффективности
    Matrix(Matrix&& other) noexcept
        : rows(other.rows), cols(other.cols), data(std::move(other.data))
    {
        other.rows = 0;
        other.cols = 0;
    }

    // И оператор присваивания перемещением
    Matrix& operator=(Matrix&& other) noexcept
    {
        if (this != &other) 
        {
            rows = other.rows;
            cols = other.cols;
            data = std::move(other.data);
            other.rows = 0;
            other.cols = 0;
        }
        return *this;
    }
};