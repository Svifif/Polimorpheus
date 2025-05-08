#pragma once
#include <stdexcept>

template<typename T>
class Stack {
private:
    struct Node 
    {
        T data;
        Node* next;
        Node(const T& value) : data(value), next(nullptr) {}
    };

    Node* top_node = nullptr;
    size_t stack_size = 0;

public:
    ~Stack() { clear(); }

    void push(const T& value)
    {
        Node* new_node = new Node(value);
        new_node->next = top_node;
        top_node = new_node;
        ++stack_size;
    }

    void pop()
    {
        if (!top_node) throw std::out_of_range("Stack is empty");
        Node* temp = top_node;
        top_node = top_node->next;
        delete temp;
        --stack_size;
    }

    T& top()
    {
        if (!top_node) throw std::out_of_range("Stack is empty");
        return top_node->data;
    }

    bool empty() const { return stack_size == 0; }
    size_t size() const { return stack_size; }

    void clear()
    {
        while (!empty()) pop();
    }
};