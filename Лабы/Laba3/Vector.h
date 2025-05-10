#pragma once
#include "DynamicArray.hpp"
#include <stdexcept>
#include <complex>
#include <type_traits>
#include <cmath>

template<typename T>
class Vector
{
private:
    DynamicArray<T> data;

    static double complexNorm(const std::complex<double>& c)
    {
        return std::norm(c);
    }

    static double numberNorm(const T& value)
    {
        return static_cast<double>(value * value);
    }

public:
    Vector() : data(0) {}  // Создаем пустой массив

    explicit Vector(size_t size) : data(size)
    {
        // Инициализируем нулями
        for (size_t i = 0; i < size; ++i) 
        {
            data.push_back(T());
        }
    }

    Vector(const T* items, size_t count) : data(count)
    {
        for (size_t i = 0; i < count; ++i) 
        {
            data.push_back(items[i]);
        }
    }

    Vector(std::initializer_list<T> init) : data(init.size())
    {
        size_t i = 0;
        for (const auto& item : init) 
        {
            data.push_back(item);
        }
    }

    size_t size() const { return data.get_size(); }

    T& operator[](size_t index) { return data.get(index); }

    const T& operator[](size_t index) const { return data.get(index); }

    Vector<T> operator+(const Vector<T>& other) const
    {
        if (size() != other.size())
            throw std::invalid_argument("Vector sizes don't match");

        Vector<T> result(size());
        for (size_t i = 0; i < size(); ++i)
            result[i] = data.get(i) + other.data.get(i);
        return result;
    }

    Vector<T> operator*(const T& scalar) const
    {
        Vector<T> result(size());
        for (size_t i = 0; i < size(); ++i)
            result[i] = data.get(i) * scalar;
        return result;
    }

    T dot(const Vector<T>& other) const
    {
        if (size() != other.size())
            throw std::invalid_argument("Vector sizes don't match");

        T result = 0;
        for (size_t i = 0; i < size(); ++i)
            result += data.get(i) * other.data.get(i);
        return result;
    }

    double norm() const
    {
        double sum = 0;
        for (size_t i = 0; i < size(); ++i)
        {
            if constexpr (std::is_same_v<T, std::complex<double>>)
                sum += complexNorm(data.get(i));
            else
                sum += numberNorm(data.get(i));
        }
        return std::sqrt(sum);
    }
};