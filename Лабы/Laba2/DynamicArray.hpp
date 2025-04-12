#pragma once
#include <stdexcept> 
#include <algorithm>
#include <utility>
template<typename ElementType> //� ������������ �������� typename ��������������� �������� ����������� � ���, ��� ����������� ������������� �������� �����.� ������� ���������� ������� ������������ ��� �������� ��������� ����.
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
    ElementType* data=nullptr;
    size_t capacity=0;
    size_t size=0;
    //O(n)
    void resize(size_t new_capacity) // ���������� ���������
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
    //������������
    //�������
    //O(1)
    explicit /*��������� ������*/ DynamicArray(size_t asumed_capcity = 0 /*�� ���������*/) :capacity(asumed_capcity), size(0)
    {
        data = new ElementType[capacity];
    }
    //����������� �����������
    //O(n)
    DynamicArray(const DynamicArray& other): capacity(other.capacity), size(other.size)
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
    // move �����������
    // O(1)
    DynamicArray(DynamicArray&& other) noexcept: data(other.data), capacity(other.capacity), size(other.size)
    {
        other.data = nullptr;
        other.capacity = 0;
        other.size = 0;
    }

    // ���������
    // O(n)
    DynamicArray& operator=(const DynamicArray&array1) 
    {
        if (this != &array1)
        {
            DynamicArray temp (array1);
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
    ElementType operator[](size_t index)
    {
        return get(index);
    }
    //������ 1 �� ������� 2-�� ��� r-value
    // O(1), � ������ ������ O(n) 
    void push_back(const ElementType& value)
    {
        if (size >= capacity) 
        {
            resize(capacity == 0 ? 1 : capacity * 2);// ������ ��� 0...
        }
        data[size++] = value;
    }
    // O(1), � ������ ������ O(n) 
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
    ElementType get(size_t index) 
    {
        if (index >= size) 
        {
            throw std::out_of_range("List index out of range");
            return ElementType{};
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
    ~DynamicArray() /*capacity  � size ��������� �������������*/
    {
        delete[] data;
    }
};