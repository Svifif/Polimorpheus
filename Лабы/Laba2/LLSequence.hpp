#pragma once
#include "Sequence.hpp"
#include "LinckedList.hpp"

template<typename ElementType>
class LLSequence : public Sequence<ElementType>
{
protected:
    LinkedList<ElementType> list;

    explicit LLSequence(LinkedList<ElementType>&& otherList) noexcept : list(std::move(otherList))
    {
    }

    virtual Sequence<ElementType>* GetThis() = 0;

public:
    LLSequence() = default;

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

    ElementType GetLast() const override
    {
        if (list.get_size() == 0)
        {
            throw std::out_of_range("Sequence is empty");
        }
        return list.back();
    }

    ElementType Get(int index) const override
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
        return list.get_data_ref(index);
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
        auto* result = GetThis();
        result->list.push_back(item);
        return result;
    }

    Sequence<ElementType>* Append(ElementType&& item) override
    {
        auto* result = GetThis();
        result->list.push_back(std::move(item));
        return result;
    }

    Sequence<ElementType>* Prepend(ElementType item) override
    {
        auto* result = GetThis();
        result->list.push_front(item);
        return result;
    }

    Sequence<ElementType>* Prepend(ElementType&& item) override
    {
        auto* result = GetThis();
        result->list.push_front(std::move(item));
        return result;
    }

    Sequence<ElementType>* InsertAt(ElementType item, int index) override
    {
        if (index < 0 || index > list.get_size())
        {
            throw std::out_of_range("Index out of range");
        }

        auto* result = GetThis();
        result->list.insert(index, item);
        return result;
    }

    Sequence<ElementType>* InsertAt(ElementType&& item, int index) override
    {
        if (index < 0 || index > list.get_size())
        {
            throw std::out_of_range("Index out of range");
        }

        auto* result = GetThis();
        result->list.insert(index, std::move(item));
        return result;
    }

    Sequence<ElementType>* GetSubsequence(int startIndex, int endIndex) const override
    {
        if (startIndex < 0 || endIndex >= list.get_size() || startIndex > endIndex)
        {
            throw std::out_of_range("Invalid subsequence indices");
        }

        auto subList = list.GetSubList(startIndex, endIndex);
        return new LLSequence(std::move(subList));
    }

    Sequence<ElementType>* Concat(Sequence<ElementType>* other) const override
    {
        if (!other)
        {
            throw std::invalid_argument("Other sequence cannot be null");
        }
        if (this == other)
        {
            throw std::invalid_argument("Cannot concatenate sequence with itself");
        }

        auto* result = new LLSequence(*this);
        for (int i = 0; i < other->GetLength(); ++i)
        {
            result->list.push_back(other->Get(i));
        }
        return result;
    }

    Sequence<ElementType>* Concat(Sequence<ElementType>&& other) override
    {
        if (this == &other)
        {
            throw std::invalid_argument("Cannot concatenate sequence with itself");
        }

        if (auto* otherSeq = dynamic_cast<LLSequence*>(&other))
        {
            auto* result = GetThis();
            result->list.concat(std::move(otherSeq->list));
            return result;
        }
        throw std::invalid_argument("Incompatible sequence type for concatenation");
    }

    template<typename ResultType>
    Sequence<ResultType>* Map(ResultType(*mapper)(ElementType)) const
    {
        if (!mapper)
        {
            throw std::invalid_argument("Mapper function cannot be null");
        }

        auto* result = new LLSequence<ResultType>();
        for (int i = 0; i < GetLength(); ++i)
        {
            result->Append(mapper(Get(i)));
        }
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

    Sequence<ElementType>* Where(bool (*predicate)(ElementType)) const
    {
        if (!predicate)
        {
            throw std::invalid_argument("Predicate function cannot be null");
        }

        auto* result = new LLSequence<ElementType>();
        for (int i = 0; i < GetLength(); ++i)
        {
            ElementType current = Get(i);
            if (predicate(current))
            {
                result->Append(current);
            }
        }
        return result;
    }
};

template<typename ElementType>
class MLLSequence : public LLSequence<ElementType>
{
protected:
    Sequence<ElementType>* GetThis() override
    {
        return this;
    }

public:
    using LLSequence<ElementType>::LLSequence;

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
    Sequence<ElementType>* GetThis() override
    {
        return new ILLSequence(*this);
    }

public:
    using LLSequence<ElementType>::LLSequence;

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