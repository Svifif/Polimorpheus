#pragma once
#include "LinkedList.hpp"
template<typename T>
class Queue 
{
private:
    LinkedList<T> list;

public:
    // Добавление элемента в очередь (в конец)
    void enqueue(const T& value)
    {
        list.push_back(value);
    }

    // Удаление элемента из очереди (из начала)
    void dequeue()
    {
        list.pop_front();
    }
    T& front() 
    {
        return list.front();
    }
    bool empty() const 
    {
        return list.empty();
    }
    size_t size() const 
    {
        return list.get_size();
    }
    void clear() 
    {
        list.clear();
    }
};