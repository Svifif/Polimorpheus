#pragma once
#include "LinkedList.hpp"
template<typename T>
class Deque 
{
private:
    LinkedList<T> list;

public:
    void push_front(const T& value)
    {
        list.push_front(value);
    }
    void push_back(const T& value) 
    {
        list.push_back(value);
    }

    void pop_front()
    {
        list.pop_front();
    }

    void pop_back() 
    {
        list.pop_back();
    }

    T& front()
    {
        return list.front();
    }

    T& back() 
    {
        return list.back();
    }

    size_t size() const
    {
        return list.get_size();
    }

    bool empty() const 
    {
        return list.empty();
    }
    void clear() 
    {
        list.clear();
    }
};