#pragma once
#include <cstddef>

template<typename T>
class SmartPointer
{
private:
    T* ptr;
    size_t* refCount;
    bool isArray;

    // Приватные методы для управления счетчиком ссылок
    void addReference() 
    {
        if (refCount) 
        {
            (*refCount)++;
        }
    }

    void removeReference() 
    {
        if (refCount && --(*refCount) == 0) 
        {
            if (isArray)
            {
                delete[] ptr;  // Удаляем весь массив
            }
            else
            {
                delete ptr;    // Удаляем одиночный объект
            }
            delete refCount;
            ptr = nullptr;
            refCount = nullptr;
        }
    }

public:
    // Конструктор для одиночного объекта
    explicit SmartPointer(T* p = nullptr) : ptr(p), isArray(false) 
    {
        if (ptr) 
        {
            refCount = new size_t(1);
        }
        else 
        {
            refCount = nullptr;
        }
    }

    // Конструктор для массива
    class ArrayTag {};
    static constexpr ArrayTag ARRAY = ArrayTag();

    SmartPointer(T* p, ArrayTag) : ptr(p), isArray(true) 
    {
        if (ptr) 
        {
            refCount = new size_t(1);
        }
        else 
        {
            refCount = nullptr;
        }
    }

    // Копирующий конструктор
    SmartPointer(const SmartPointer& other): ptr(other.ptr), refCount(other.refCount), isArray(other.isArray) 
    {
        addReference();
    }

    // Move конструктор
    SmartPointer(SmartPointer&& other) noexcept
        : ptr(other.ptr), refCount(other.refCount), isArray(other.isArray) 
    {
        other.ptr = nullptr;
        other.refCount = nullptr;
    }

    // Оператор присваивания
    SmartPointer& operator=(const SmartPointer& other) 
    {
        if (this != &other) 
        {
            removeReference();
            ptr = other.ptr;
            refCount = other.refCount;
            isArray = other.isArray;
            addReference();
        }
        return *this;
    }

    // Move оператор присваивания
    SmartPointer& operator=(SmartPointer&& other) noexcept 
    {
        if (this != &other) 
        {
            removeReference();
            ptr = other.ptr;
            refCount = other.refCount;
            isArray = other.isArray;
            other.ptr = nullptr;
            other.refCount = nullptr;
        }
        return *this;
    }

    // Деструктор
    ~SmartPointer() 
    {
        removeReference();
    }

    // Операторы доступа
    T& operator*() const 
    {
        if (!ptr) throw std::runtime_error("Dereferencing null pointer");
        return *ptr;
    }

    T* operator->() const 
    {
        if (!ptr) throw std::runtime_error("Accessing null pointer");
        return ptr;
    }

    // Оператор индексации (только для массивов)
    T& operator[](size_t index) const 
    {
        if (!ptr) throw std::runtime_error("Accessing null array");
        if (!isArray) throw std::runtime_error("Not an array pointer");
        return ptr[index];
    }

    // Получение сырого указателя
    T* get() const { return ptr; }

    // Проверка на валидность
    explicit operator bool() const { return ptr != nullptr; }

    // Получение количества ссылок
    size_t use_count() const { return refCount ? *refCount : 0; }

    // Проверка является ли указателем на массив
    bool is_array() const { return isArray; }

    // Создание умного указателя на массив
    static SmartPointer make_array(size_t size) 
    {
        return SmartPointer(new T[size], ARRAY);
    }

    // Создание умного указателя на одиночный объект
    static SmartPointer make_single() 
    {
        return SmartPointer(new T());
    }

    template<typename... Args>
    static SmartPointer make_single(Args&&... args) 
    {
        return SmartPointer(new T(std::forward<Args>(args)...));
    }
};