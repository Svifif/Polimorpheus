#pragma once
#include "Sequence.hpp"
#include "DynamicArray.hpp"

template<typename ElementType>
class ArraySequence : public Sequence<ElementType>
{
protected:
    DynamicArray<ElementType> array;

    explicit ArraySequence(DynamicArray<ElementType>&& otherArray) noexcept : array(std::move(otherArray)) {}

    virtual Sequence<ElementType>* GetThis() = 0;

public:
    ArraySequence() = default;

    ArraySequence(ElementType* items, int count) : array(items, count) 
    {
        if (items == nullptr && count > 0)
        {
            throw std::invalid_argument("Items pointer is null but count is positive!");
        }
    }

    ArraySequence(const ArraySequence& other) : array(other.array) {}

    ArraySequence(ArraySequence&& other) noexcept : array(std::move(other.array)) {}

    ~ArraySequence() override = default;

    ElementType GetFirst() const override
    {
        if (array.get_size() == 0)
            throw std::out_of_range("Sequence is empty");
        return array[0];
    }

    ElementType& GetLast() const override
    {
        if (array.get_size() == 0)
            throw std::out_of_range("Sequence is empty");
        return array[array.get_size() - 1];
    }

    ElementType& Get(int index) const override
    {
        if (index < 0 || index >= array.get_size())
            throw std::out_of_range("Index out of range");
        return array.get(index);
    }

    int GetLength() const override
    {
        return array.get_size();
    }

    ElementType& operator[](int index)
    {
        if (index < 0 || index >= array.get_size())
            throw std::out_of_range("Index out of range");
        return array[index];
    }

    const ElementType& operator[](int index) const
    {
        if (index < 0 || index >= array.get_size())
            throw std::out_of_range("Index out of range");
        return array.get(index);
    }

    bool operator==(const Sequence<ElementType>& other) const
    {
        if (GetLength() != other.GetLength())
            return false;

        for (int i = 0; i < GetLength(); ++i)
            if (Get(i) != other.Get(i))
                return false;
        return true;
    }

    bool operator!=(const Sequence<ElementType>& other) const
    {
        return !(*this == other);
    }

    ArraySequence& operator+=(const Sequence<ElementType>& other)
    {
        if (&other == nullptr)
            throw std::invalid_argument("Other sequence is null");

        for (int i = 0; i < other.GetLength(); ++i)
            array.push_back(other.Get(i));
        return *this;
    }

    ArraySequence& operator+=(Sequence<ElementType>&& other)
    {
        if (&other == nullptr)
            throw std::invalid_argument("Other sequence is null");

        if (auto* otherSeq = dynamic_cast<ArraySequence*>(&other))
            array.concat(std::move(otherSeq->array));
        else
            throw std::invalid_argument("Incompatible sequence type");
        return *this;
    }

    Sequence<ElementType>* Append(ElementType item) override
    {
        auto* result = (ArraySequence<ElementType> *)GetThis();
        result->array.push_back(item);
        return result;
    }

    Sequence<ElementType>* Prepend(ElementType item) override
    {
        auto* result = (ArraySequence<ElementType> *) GetThis();
        result->array.insert(item, 0);
        return result;
    }

    Sequence<ElementType>* InsertAt(ElementType item, int index) override
    {
        if (index < 0 || index > array.get_size())
            throw std::out_of_range("Index out of range");

        auto result = (ArraySequence<ElementType>*)GetThis();
        result->array.insert(item, index);
        return result;
    }

    //Sequence<ElementType>* GetSubsequence(int startIndex, int endIndex) const override
    //{
    //    if (startIndex < 0 || endIndex >= array.get_size() || startIndex > endIndex)
    //        throw std::out_of_range("Invalid subsequence indices");

    //    DynamicArray<ElementType> subArray(endIndex - startIndex + 1);
    //    for (int i = startIndex; i <= endIndex; ++i)
    //        subArray[i - startIndex] = array[i];
    //    return new ArraySequence(std::move(subArray));
    //}

    //Sequence<ElementType>* Concat(Sequence<ElementType>* other) const override
    //{
    //    if (other == nullptr)
    //        throw std::invalid_argument("Other sequence is null");

    //    auto* result = new ArraySequence(*this);
    //    for (int i = 0; i < other->GetLength(); ++i)
    //        result->array.push_back(other->Get(i));
    //    return result;
    //}

    //template<typename ResultType>
    //Sequence<ResultType>* Map(ResultType(*mapper)(ElementType)) const
    //{
    //    auto* result = new ArraySequence<ResultType>();
    //    for (int i = 0; i < GetLength(); ++i)
    //        result->Append(mapper(Get(i)));
    //    return result;
    //}

    template<typename ResultType>
    ResultType Reduce(ResultType(*reducer)(ResultType, ElementType), ResultType initial) const
    {
        ResultType accumulator = initial;
        for (int i = 0; i < GetLength(); ++i)
            accumulator = reducer(accumulator, Get(i));
        return accumulator;
    }

protected:

};

// Mutable ������
template<typename ElementType>
class MArraySequence : public ArraySequence<ElementType>
{
protected:
    Sequence<ElementType>* GetThis() override { return this; }
    Sequence<ElementType>* CreateEmpty()  override
    {
        return new MArraySequence<ElementType>(); // ������ ������� ����� ������
    }

public:
    MArraySequence() : ArraySequence<ElementType>() {};
    using ArraySequence<ElementType>::ArraySequence;

    MArraySequence(const MArraySequence& other) : ArraySequence<ElementType>(other) {};
    MArraySequence(MArraySequence&& other) noexcept = default;
    MArraySequence& operator=(MArraySequence&& other) noexcept = default;
    MArraySequence& operator=(const MArraySequence& other)
    {
        if (this != &other)
            this->array = other.array;
        return *this;
    }
};

// Immutable ������
template<typename ElementType>
class IArraySequence : public ArraySequence<ElementType>
{
protected:
    Sequence<ElementType>* GetThis() override { return new IArraySequence(*this); }

    Sequence<ElementType>* CreateEmpty()  override
    {
        return new IArraySequence<ElementType>(); // ������ ������� ����� ������
    }

public:
    using ArraySequence<ElementType>::ArraySequence;

    IArraySequence(IArraySequence&& other) noexcept = default;
    IArraySequence& operator=(IArraySequence&& other) noexcept = default;
    IArraySequence& operator=(const IArraySequence& other)
    {
        if (this != &other)
            this->array = other.array;
        return *this;
    }

};
