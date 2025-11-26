#pragma once
#include <iostream>

class Cardinal
{
public:
    int order;
    int index;

    // Конструктор по умолчанию - сразу реализация
    explicit Cardinal() : order(0), index(0) {}

    // Параметризованный конструктор
    explicit Cardinal(int order, int index) : order(order), index(index) {}

    // Копирующий конструктор
    Cardinal(const Cardinal& other) : order(other.order), index(other.index) {}

    // Move конструктор
    Cardinal(Cardinal&& other) noexcept: order(other.order), index(other.index)
    {
        other.order = 0;
        other.index = 0;
    }

    // Оператор больше
    bool operator>(const Cardinal& other) const
    {
        if (order > other.order) return true;
        if (order == other.order) return index > other.index;
        return false;
    }

    // Оператор меньше  
    bool operator<(const Cardinal& other) const
    {
        if (order < other.order) return true;
        if (order == other.order) return index < other.index;
        return false;
    }

    // Оператор присваивания
    Cardinal& operator=(const Cardinal& other)
    {
        if (this != &other) 
        {
            order = other.order;
            index = other.index;
        }
        return *this;
    }

    // Оператор сложения
    Cardinal operator+(const Cardinal& other) const
    {
        return Cardinal(order + other.order, index + other.index);
    }

    // Деструктор
    bool operator==(const Cardinal& other) const 
    {
        return order == other.order && index == other.index;
    }

    bool operator!=(const Cardinal& other) const 
    {
        return !(*this == other);
    }
    ~Cardinal() = default;
};