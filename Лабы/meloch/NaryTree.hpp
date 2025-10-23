#pragma once
#include "DynamicArray.hpp"
#include "Queue.hpp"
#include <iostream>
#include <stdexcept>

template <typename T>
class NaryTree
{
private:
    struct Node
    {
        T data;
        DynamicArray<Node*> children;

        Node(const T& value, size_t arity): data(value), children(arity)
        {
            if (arity == 0)
                throw std::invalid_argument("Arity cannot be zero");
        }
    };

    Node* root;
    size_t arity;

    Node* findNode(Node* current, const T& value) const
    {
        if (!current) return nullptr;
        if (current->data == value) return current;

        for (size_t i = 0; i < current->children.get_size(); ++i)
        {
            if (!current->children[i])
                throw std::logic_error("Child node is null");

            Node* found = findNode(current->children[i], value);
            if (found) return found;
        }
        return nullptr;
    }

    void deleteSubtree(Node* node)
    {
        if (!node) return;
        for (size_t i = 0; i < node->children.get_size(); ++i)
        {
            deleteSubtree(node->children[i]);
        }
        delete node;
    }

public:
    NaryTree(size_t n)
        : root(nullptr), arity(n)
    {
        if (arity == 0)
            throw std::invalid_argument("Arity cannot be zero");
    }

    ~NaryTree()
    {
        clear();
    }

    bool insert(const T& parentValue, const T& newValue)
    {
        if (isEmpty())
        {
            if (parentValue != newValue)
            {
                throw std::invalid_argument("Root value must match initial value");
            }
            root = new Node(newValue, arity);
            return true;
        }

        Node* parent = findNode(root, parentValue);
        if (!parent)
        {
            return false; // Возвращаем false вместо исключения
        }

        if (parent->children.get_size() >= arity)
        {
            throw std::out_of_range("Cannot add more children - arity limit reached");
        }

        parent->children.push_back(new Node(newValue, arity));
        return true;
    }

    bool contains(const T& value) const
    {
        return findNode(root, value) != nullptr;
    }

    bool remove(const T& value)
    {
        if (!root)
            throw std::logic_error("Tree is empty");

        if (root->data == value)
        {
            deleteSubtree(root);
            root = nullptr;
            return true;
        }

        Queue<Node*> q;
        q.enqueue(root);

        while (!q.empty())
        {
            Node* current = q.front();
            q.dequeue();

            for (size_t i = 0; i < current->children.get_size(); ++i)
            {
                if (!current->children[i])
                    throw std::logic_error("Child node is null");

                if (current->children[i]->data == value)
                {
                    deleteSubtree(current->children[i]);
                    current->children.remove(i);
                    return true;
                }
                q.enqueue(current->children[i]);
            }
        }
        throw std::invalid_argument("Value not found in tree");
    }

    void clear()
    {
        deleteSubtree(root);
        root = nullptr;
    }

    void printLevelOrder() const
    {
        if (!root)
            throw std::logic_error("Tree is empty");

        Queue<Node*> q;
        q.enqueue(root);

        while (!q.empty())
        {
            size_t levelSize = q.size();
            for (size_t i = 0; i < levelSize; ++i)
            {
                Node* current = q.front();
                q.dequeue();
                std::cout << current->data << " ";

                for (size_t j = 0; j < current->children.get_size(); ++j)
                {
                    if (!current->children[j])
                        throw std::logic_error("Child node is null");
                    q.enqueue(current->children[j]);
                }
            }
            std::cout << std::endl;
        }
    }

    bool isEmpty() const
    {
        return root == nullptr;
    }

    size_t getArity() const
    {
        if (arity == 0)
            throw std::logic_error("Invalid arity value");
        return arity;
    }
};