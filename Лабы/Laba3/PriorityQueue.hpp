#pragma once
#include "DynamicArray.hpp"  // ���������� ��� DynamicArray
#include <functional>      // ��� std::less
#include <stdexcept>       // ��� std::out_of_range

template<typename T, typename Compare = std::less<T>>
class PriorityQueue
{
private:
    DynamicArray<T> heap;
    Compare comp;
    void heapify_up(size_t index)
    {
        while (index > 0) 
        {
            size_t parent = (index - 1) / 2;
            if (comp(heap[parent], heap[index])) 
            {
                std::swap(heap[index], heap[parent]);
                index = parent;
            }
            else 
            {
                break;
            }
        }
    }

    // ����������� ���� (����� ��������)
    void heapify_down(size_t index) 
    {
        while (true)
        {
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;
            size_t largest = index;

            if (left < heap.get_size() && comp(heap[largest], heap[left])) 
            {
                largest = left;
            }
            if (right < heap.get_size() && comp(heap[largest], heap[right]))
            {
                largest = right;
            }
            if (largest == index) 
            {
                break;
            }
            std::swap(heap[index], heap[largest]);
            index = largest;
        }
    }

public:
    PriorityQueue() = default;

    // ���������� ��������
    void push(const T& value)
    {
        heap.push_back(value);
        heapify_up(heap.get_size() - 1);
    }

    // ���������� �������� (�����������)
    void push(T&& value) 
    {
        heap.push_back(std::move(value));
        heapify_up(heap.get_size() - 1);
    }

    // �������� �������
    void pop() 
    {
        if (empty()) 
        {
            throw std::out_of_range("PriorityQueue is empty");
        }
        heap[0] = std::move(heap[heap.get_size() - 1]);
        heap.pop_back();
        if (!empty())
        {
            heapify_down(0);
        }
    }

    // ������ � �������
    const T& top() const 
    {
        if (empty()) 
        {
            throw std::out_of_range("PriorityQueue is empty");
        }
        return heap[0];
    }

    // ������ �������
    size_t size() const 
    {
        return heap.get_size();
    }

    // �������� �� �������
    bool empty() const
    {
        return heap.get_size() == 0;
    }

    // ������� �������
    void clear() 
    {
        while (!empty()) 
        {
            pop();
        }
    }
};