#pragma once
#include <stdexcept> 
#include <algorithm>
#include <utility>
template<typename ElementType> //В определениях шаблонов typename предоставляется указание компилятору о том, что неизвестный идентификатор является типом.В списках параметров шаблона используется для указания параметра типа.
/// <summary>
///  void resize(size_t new_capcity)
/// operator=
/// operator[]
/// push_back(const ElementType& value)
/// set(const ElementType& object, size_t index)
/// get(size_t index)
/// swap(DynamicArray& other)
/// get_size()
/// get_capacity()
/// </summary>
class DynamicArray
{
private:
    ElementType* data = nullptr;
    size_t capacity = 0;
    size_t size = 0;
    //O(n)
    void resize(size_t new_capacity) // Исправлено написание
    {
        if (new_capacity < size)
        {
            throw std::invalid_argument("Capacity can't be < size");
        }
        ElementType* new_data = new ElementType[new_capacity];
        for (size_t i = 0; i < size; ++i) {
            new_data[i] = std::move(data[i]);
        }
        delete[] data;
        data = new_data;
        capacity = new_capacity;
    }

public:
    //конструкторы
    //обычный
    //O(1)
    explicit /*запрещено неявно*/ DynamicArray(size_t asumed_capcity = 0 /*по умолчанию*/) :capacity(asumed_capcity), size(0)
    {
        data = new ElementType[capacity];
    }
    //O(n)
    DynamicArray(ElementType* items, size_t count) : capacity(count), size(count)
    {
        if (items == nullptr)
        {
            throw std::invalid_argument("Items pointer is null!");
        }

        if (capacity > 0)
        {
            data = new ElementType[capacity];
            for (size_t i = 0; i < size; ++i)
            {
                data[i] = items[i];
            }
        }
    }

    //конструктор копирования
    //O(n)
    DynamicArray(const DynamicArray& other) : capacity(other.capacity), size(other.size)
    {
        if (capacity > 0)
        {
            data = new ElementType[capacity];
            for (size_t i = 0; i < size; ++i)
            {
                data[i] = other.data[i];
            }
        }
    }
    // move конструктор
    // O(1)
    DynamicArray(DynamicArray&& other) noexcept : data(other.data), capacity(other.capacity), size(other.size)
    {
        other.data = nullptr;
        other.capacity = 0;
        other.size = 0;
    }

    // операторы
    // O(n)
    DynamicArray& operator=(const DynamicArray& array1)
    {
        if (this != &array1)
        {
            DynamicArray temp(array1);
            swap(temp);
        }
        return *this;
    }
    // O(1) 
    DynamicArray& operator=(DynamicArray<ElementType>&& other)
    {
        if (this == &other)
        {
            return *this;
        }
        if (data)
        {
            delete[]data;
        }

        data = other.data;
        size = other.size;

        other.data = nullptr;
        other.size = 0;

        return *this;
    }
    // O(1)
    ElementType& operator[](size_t index) const
    {
        return get(index);
    }
    //методы 1 ая обычная 2-ая для r-value
    // O(1), в худшем случае O(n) 
    void push_back(const ElementType& value)
    {
        if (size >= capacity)
        {
            resize(capacity == 0 ? 1 : capacity * 2);// учесть что 0...
        }
        data[size++] = value;
    }
    // O(1), в худшем случае O(n) 
    void push_back(ElementType&& value)
    {
        if (size >= capacity)
        {
            resize(capacity == 0 ? 1 : capacity * 2);
        }
        data[size++] = std::move(value);
    }
    // O(1)
    void set(const ElementType& object, size_t index)
    {
        if (index >= size)
        {
            throw std::out_of_range("List index out of range");
            return;
        }
        data[index] = object;
    }
    // O(1)
    void set(ElementType&& object, size_t index)
    {
        if (index >= size)
        {
            throw std::out_of_range("List index out of range");
            return;
        }
        data[index] = std::move(object);
    }
    // O(1)
    ElementType& get(size_t index) const
    {
        if (index >= size)
        {
            throw std::out_of_range("List index out of range");
        }
        return data[index];
    }
    // O(1)
    void swap(DynamicArray& other) noexcept
    {
        std::swap(data, other.data);
        std::swap(capacity, other.capacity);
        std::swap(size, other.size);
    }
    // O(n)
    size_t get_size() const noexcept
    {
        return size;
    }
    size_t get_capacity() const noexcept
    {
        return capacity;
    }
    // O(n)
    void insert(const ElementType& value, size_t index)
    {
        if (index > size)
        {  // Можно вставить после последнего элемента (index == size)
            throw std::out_of_range("Index out of range");
        }
        // Увеличиваем capacity при необходимости
        if (size >= capacity)
        {
            resize(capacity == 0 ? 1 : capacity * 2);
        }

        // Сдвигаем элементы вправо начиная с index
        for (size_t i = size; i > index; --i)
        {
            data[i] = std::move(data[i - 1]);
        }

        // Вставляем новый элемент
        data[index] = value;
        ++size;
    }

    // Версия для r-value ссылок
    void insert(ElementType&& value, size_t index)
    {
        if (index > size)
        {
            throw std::out_of_range("Index out of range");
        }

        if (size >= capacity)
        {
            resize(capacity == 0 ? 1 : capacity * 2);
        }

        for (size_t i = size; i > index; --i)
        {
            data[i] = std::move(data[i - 1]);
        }

        data[index] = std::move(value);
        ++size;
    }

    // O(1)
    void pop_back()
    {
        if (size == 0)
        {
            throw std::out_of_range("Array is empty");
        }
        --size;
    }

    ~DynamicArray() /*capacity  и size удаляются автоматически*/
    {
        delete[] data;
    }
};