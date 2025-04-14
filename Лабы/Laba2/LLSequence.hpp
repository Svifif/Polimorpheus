#pragma once
#include "Sequence.hpp"
#include "LinkedList.hpp"

template<typename ElementType>
class LLSequence : public Sequence<ElementType>
{
protected:
    LinkedList<ElementType> list;

    explicit LLSequence(LinkedList<ElementType>&& otherList) noexcept : list(std::move(otherList))
    {
    }

    //virtual LLSequence<ElementType>* GetThis() = 0;
    //virtual const LLSequence<ElementType>* GetThis() const = 0;

public:
    LLSequence() = default;
    using Sequence<ElementType>::Zip;
    LLSequence(ElementType* items, int count)
    {
        if (count < 0)
        {
            throw std::invalid_argument("Count cannot be negative");
        }
        if (items == nullptr && count > 0)
        {
            throw std::invalid_argument("Items pointer cannot be nullptr with positive count");
        }

        for (int i = 0; i < count; ++i)
        {
            list.push_back(items[i]);
        }
    }

    LLSequence(const LLSequence& other) : list(other.list)
    {
    }

    LLSequence(LLSequence&& other) noexcept : list(std::move(other.list))
    {
    }

    ~LLSequence() override = default;

    ElementType GetFirst() const override
    {
        if (list.get_size() == 0)
        {
            throw std::out_of_range("Sequence is empty");
        }
        return list.front();
    }

    ElementType& GetLast() const override
    {
        if (list.get_size() == 0)
        {
            throw std::out_of_range("Sequence is empty");
        }
        return list.back();
    }

    ElementType& Get(int index) const override
    {
        if (index < 0 || index >= list.get_size())
        {
            throw std::out_of_range("Index out of range");
        }
        return list.get_data(index);
    }

    int GetLength() const override
    {
        return list.get_size();
    }

    ElementType& operator[](int index)
    {
        if (index < 0 || index >= list.get_size())
        {
            throw std::out_of_range("Index out of range");
        }
        return list.get_data(index);
    }

    const ElementType& operator[](int index) const
    {
        if (index < 0 || index >= list.get_size())
        {
            throw std::out_of_range("Index out of range");
        }
        return list.get_data(index);
    }

    bool operator==(const Sequence<ElementType>& other) const
    {
        if (GetLength() != other.GetLength())
        {
            return false;
        }

        for (int i = 0; i < GetLength(); ++i)
        {
            if (Get(i) != other.Get(i))
            {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Sequence<ElementType>& other) const
    {
        return !(*this == other);
    }

    LLSequence& operator+=(const Sequence<ElementType>& other)
    {
        if (this == &other)
        {
            throw std::invalid_argument("Cannot concatenate sequence with itself");
        }

        for (int i = 0; i < other.GetLength(); ++i)
        {
            Append(other.Get(i));
        }
        return *this;
    }

    LLSequence& operator+=(Sequence<ElementType>&& other)
    {
        if (this == &other)
        {
            throw std::invalid_argument("Cannot concatenate sequence with itself");
        }

        if (auto* otherSeq = dynamic_cast<LLSequence*>(&other))
        {
            list.concat(std::move(otherSeq->list));
        }
        return *this;
    }

    template<typename T>
    friend LLSequence<T> operator+(const LLSequence<T>& lhs, const Sequence<T>& rhs);

    template<typename T>
    friend LLSequence<T> operator+(LLSequence<T>&& lhs, Sequence<T>&& rhs);

    Sequence<ElementType>* Append(ElementType item) override
    {
        auto* result = (LLSequence<ElementType>*) this->GetThis();
        result->list.push_back(item);
        return result;
    }

    // Специальная перегрузка для строковых литералов
    Sequence<ElementType>* Append(const char* item)
    {
        return Append(std::string(item));
    }

    Sequence<ElementType>* Prepend(ElementType item) override
    {
        auto* result = (LLSequence<ElementType>*)this->GetThis();
        result->list.push_front(item);
        return result;
    }

    // Универсальный Prepend
    template<typename T>
    Sequence<ElementType>* Prepend(T&& item)
    {
        auto* result = this->GetThis();
        result->list.push_front(std::forward<T>(item));
        return result;
    }

    // Специальная перегрузка для строковых литералов
    Sequence<ElementType>* Prepend(const char* item)
    {
        return Prepend(std::string(item));
    }

    Sequence<ElementType>* InsertAt(ElementType item, int index) override
    {
        if (index < 0 || index > list.get_size())
        {
            throw std::out_of_range("Index out of range");
        }

        auto result = (LLSequence<ElementType>*)this->GetThis();
        result->list.insert(index, item);
        return result;
    }



    template<typename ResultType>
    ResultType Reduce(ResultType(*reducer)(ResultType, ElementType), ResultType initial) const
    {
        if (!reducer)
        {
            throw std::invalid_argument("Reducer function cannot be null");
        }

        ResultType accumulator = initial;
        for (int i = 0; i < GetLength(); ++i)
        {
            accumulator = reducer(accumulator, Get(i));
        }
        return accumulator;
    }


};

template<typename ElementType>
class MLLSequence : public LLSequence<ElementType>
{
protected:
    LLSequence<ElementType>* GetThis() override
    {
        return this; // Возвращает текущий объект
    }
    Sequence<ElementType>* CreateEmpty()  override
    {
        return new MLLSequence<ElementType>(); // Просто создаем новый объект
    }


public:
    MLLSequence(const MLLSequence& other) : LLSequence<ElementType>(other) {}
    MLLSequence() : LLSequence<ElementType>() {} // Конструктор по умолчанию
    // Явно определяем конструктор с параметрами
    MLLSequence(ElementType* items, int count) : LLSequence<ElementType>(items, count) {}


    using LLSequence<ElementType>::LLSequence; // Используем остальные конструкторы базового класса

    MLLSequence(MLLSequence&& other) noexcept = default;

    MLLSequence& operator=(MLLSequence&& other) noexcept = default;

    MLLSequence& operator=(const MLLSequence& other)
    {
        if (this != &other)
        {
            this->list = other.list;
        }
        return *this;
    }


};


template<typename ElementType>
class ILLSequence : public LLSequence<ElementType>
{
protected:
    LLSequence<ElementType>* GetThis() override
    {
        return new ILLSequence(*this);
    }
    //const LLSequence<ElementType>* GetThis() const  { return new ILLSequence(*this); }
    Sequence<ElementType>* CreateEmpty()  override
    {
        return new ILLSequence<ElementType>(); // Просто создаем новый объект
    }

public:
    // Явно определяем конструктор с параметрами
    ILLSequence(ElementType* items, int count) : LLSequence<ElementType>(items, count) {}

    using LLSequence<ElementType>::LLSequence; // Используем остальные конструкторы базового класса

    ILLSequence(ILLSequence&& other) noexcept = default;

    ILLSequence& operator=(ILLSequence&& other) noexcept = default;

    ILLSequence& operator=(const ILLSequence& other)
    {
        if (this != &other)
        {
            this->list = other.list;
        }
        return *this;
    }

};
