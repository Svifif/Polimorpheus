#pragma once
#include <stdexcept> 
#include <algorithm>
#include <utility>
template<typename ElementType>
class LinkedList 
{
private:
    struct Node
    {
        ElementType data;
        Node* prev = nullptr;
        Node* next = nullptr;

        Node(const ElementType& value) : data(value) {}
        Node(ElementType&& value) : data(std::move(value)) {}
    };

    Node* head = nullptr;
    Node* tail = nullptr;
    size_t size = 0;

public:
    // O(1)
    LinkedList() = default;

    // O(n) 
    LinkedList(const LinkedList& other)
    {
        for (const auto& item : other)
        {
            push_back(item);
        }
    }

    // O(1) 
    //cpy
    LinkedList(LinkedList&& other) noexcept : head(other.head), tail(other.tail), size(other.size)
    {
        other.head = nullptr;
        other.tail = nullptr;
        other.size = 0;
    }
    // O(n)
    LinkedList& operator=(const LinkedList& other) 
    {
        if (this != &other)
        {
            LinkedList temp(other);
            swap(temp);
        }
        return *this;
    }

    // O(1)
    LinkedList& operator=(LinkedList&& other) noexcept 
    {
        if (this != &other)
        {
            clear();
            head = other.head;
            tail = other.tail;
            size = other.size;
            other.head = nullptr;
            other.tail = nullptr;
            other.size = 0;
        }
        return *this;
    }

    LinkedList& operator+=(const LinkedList& other)
    {
        concat(other);
        return *this;
    }
    LinkedList& operator+=(LinkedList&& other)
    {
        concat(std::move(other));
        return *this;
    }
    // Добавляем в публичную секцию класса LinkedList
    ElementType& operator[](size_t index) 
    {
        if (index >= size) 
        {
            throw std::out_of_range("Index out of range");
        }
        Node* current = head;
        for (size_t i = 0; i < index; ++i) 
        {
            current = current->next;
        }
        return current->data;
    }

    const ElementType& operator[](size_t index) const 
    {
        if (index >= size) 
        {
            throw std::out_of_range("Index out of range");
        }
        const Node* current = head;
        for (size_t i = 0; i < index; ++i)
        {
            current = current->next;
        }
        return current->data;
    }

    // O(1)
    void push_front(const ElementType& value) 
    {
        Node* newNode = new Node(value);
        if (empty())
        {
            head = tail = newNode;
        }
        else
        {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        size++;
    }

    // O(1)
    void push_front(ElementType&& value) 
    {
        Node* newNode = new Node(std::move(value));
        if (empty())
        {
            head = tail = newNode;
        }
        else
        {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        size++;
    }

    // O(1)
    void push_back(const ElementType& value)
    {
        Node* newNode = new Node(value);
        if (empty())
        {
            head = tail = newNode;
        }
        else
        {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        size++;
    }

    // O(1)
    void push_back(ElementType&& value)
    {
        Node* newNode = new Node(std::move(value));
        if (empty()) 
        {
            head = tail = newNode;
        }
        else
        {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        size++;
    }

    // O(1) 
    void pop_front() 
    {
        if (empty())
        {
            throw std::out_of_range("List is empty");
        }

        Node* temp = head;
        head = head->next;
        if (head)
        {
            head->prev = nullptr;
        }
        else
        {
            tail = nullptr;
        }
        delete temp;
        size--;
    }

    // O(1) 
    void pop_back()
    {
        if (empty())
        {
            throw std::out_of_range("List is empty");
        }

        Node* temp = tail;
        tail = tail->prev;
        if (tail)
        {
            tail->next = nullptr;
        }
        else 
        {
            head = nullptr;
        }
        delete temp;
        size--;
    }

    // O(n)
    void insert(size_t index, const ElementType& value) 
    {
        if (index > size)
        {
            throw std::out_of_range("Index out of range");
        }

        if (index == 0)
        {
            push_front(value);
        }
        else if (index == size)
        {
            push_back(value);
        }
        else
        {
            Node* current = get_node(index);
            Node* newNode = new Node(value);

            newNode->prev = current->prev;
            newNode->next = current;
            current->prev->next = newNode;
            current->prev = newNode;

            size++;
        }
    }

    // O(n) 
    void pop(size_t index)
    {
        if (index >= size)
        {
            throw std::out_of_range("Index out of range");
        }

        if (index == 0)
        {
            pop_front();
        }
        else if (index == size - 1)
        {
            pop_back();
        }
        else 
        {
            Node* current = get_node(index);
            current->prev->next = current->next;
            current->next->prev = current->prev;
            delete current;
            size--;
        }
    }

    // O(n) 
    void clear() noexcept
    {
        while (!empty())
        {
            pop_front();
        }
    }

    // O(1)
    void swap(LinkedList& other) noexcept
    {
        std::swap(head, other.head);
        std::swap(tail, other.tail);
        std::swap(size, other.size);
    }

    // O(1) 
    bool empty() const noexcept
    {
        return size == 0;
    }

    // O(1) 
    size_t get_size() const noexcept
    {
        return size;
    }

    // O(n) 
    ElementType& get_data(size_t index) const
    {
        return get_node(index)->data;
    }

    // O(1) 
    ElementType& front()
    {
        if (empty())
        {
            throw std::out_of_range("List is empty");
        }
        return head->data;
    }

    // O(1) 
    const ElementType& front() const
    {
        if (empty())
        {
            throw std::out_of_range("List is empty");
        }
        return head->data;
    }

    // O(1) 
    ElementType& back() const
    {
        if (empty())
        {
            throw std::out_of_range("List is empty");
        }
        return tail->data;
    }

    // O(n) 
    void concat(const LinkedList& other) 
    {
        for (const auto& item : other)
        {
            push_back(item);
        }
    }
    //O(1)
    void concat(LinkedList&& other) noexcept
    {
        if (other.empty()) return;

        if (empty())
        {
            head = other.head;
            tail = other.tail;
            size = other.size;
        }
        else 
        {
            tail->next = other.head;
            other.head->prev = tail;
            tail = other.tail;
            size += other.size;
        }
        other.head = nullptr;
        other.tail = nullptr;
        other.size = 0;
    }
    //O(n)
    LinkedList<ElementType> GetSubList(size_t index1, size_t index2) const
    {
        if (index1 >= size || index2 >= size || index1 > index2)
        {
            throw std::out_of_range("Index out of range");
        }

        LinkedList<ElementType> subList;
        const Node* current = get_node(index1);

        for (size_t i = index1; i <= index2; ++i)
        {
            subList.push_back(current->data);
            current = current->next;
        }

        return subList;
    }
    // O(n)
    ~LinkedList()
    {
        clear();
    }

private:
    // O(n) 
    Node* get_node(size_t index) const
    {
        if (index >= size)
        {
            throw std::out_of_range("Index out of range");
        }

        Node* current = head;
        for (size_t i = 0; i < index; ++i)
        {
            current = current->next;
        }
        return current;
    }

public:
    class Iterator
    {
    private:
        Node* current;

    public:
        explicit Iterator(Node* node) : current(node) {}

        ElementType& operator*() const
        {
            return current->data;
        }

        Iterator& operator++()
        {
            current = current->next;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const Iterator& other) const
        {
            return current == other.current;
        }

        bool operator!=(const Iterator& other) const
        {
            return !(*this == other);
        }
    };

    Iterator begin()
    {
        return Iterator(head);
    }

    Iterator end() 
    {
        return Iterator(nullptr);
    }

    class ConstIterator
    {
    private:
        const Node* current;

    public:
        explicit ConstIterator(const Node* node) : current(node) {}

        const ElementType& operator*() const
        {
            return current->data;
        }

        ConstIterator& operator++()
        {
            current = current->next;
            return *this;
        }

        ConstIterator operator++(int)
        {
            ConstIterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const ConstIterator& other) const 
        {
            return current == other.current;
        }

        bool operator!=(const ConstIterator& other) const
        {
            return !(*this == other);
        }
    };

    ConstIterator begin() const 
    {
        return ConstIterator(head);
    }

    ConstIterator end() const
    {
        return ConstIterator(nullptr);
    }
};