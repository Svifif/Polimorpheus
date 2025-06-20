#pragma once
#include "LinkedList.hpp"
#include <stdexcept>

template<typename T>
class Queue
{
private:
    LinkedList<T> list;

public:
    void enqueue(const T& value)
    {
        try {
            list.push_back(value);
        }
        catch (const std::bad_alloc&) {
            throw std::runtime_error("Memory allocation failed in enqueue");
        }
    }

    void dequeue()
    {
        if (list.empty())
            throw std::out_of_range("Cannot dequeue from empty queue");

        try {
            list.pop_front();
        }
        catch (...) {
            throw std::runtime_error("Failed to dequeue element");
        }
    }

    T& front()
    {
        if (list.empty())
            throw std::out_of_range("Queue is empty");
        return list.front();
    }

    const T& front() const
    {
        if (list.empty())
            throw std::out_of_range("Queue is empty");
        return list.front();
    }

    T& back()
    {
        if (list.empty())
            throw std::out_of_range("Queue is empty");
        return list.back();
    }

    const T& back() const
    {
        if (list.empty())
            throw std::out_of_range("Queue is empty");
        return list.back();
    }

    bool empty() const noexcept
    {
        return list.empty();
    }

    size_t size() const noexcept
    {
        return list.get_size();
    }

    void clear() noexcept
    {
        list.clear();
    }
};