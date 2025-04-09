#include "DynamicArray.cpp"

template<typename ElementType>
class Sequence
{
public:
    virtual ~Sequence() = default;
    virtual ElementType GetFirst() const = 0;
    virtual ElementType GetLast() const = 0;
    virtual ElementType Get(int index) const = 0;
    virtual Sequence<ElementType>* GetSubsequence(int startIndex, int endIndex) const = 0;
    virtual int GetLength() const = 0;
    virtual Sequence<ElementType>* Append(ElementType item) = 0;
    virtual Sequence<ElementType>* Prepend(ElementType item) = 0;
    virtual Sequence<ElementType>* InsertAt(ElementType item, int index) = 0;
    virtual Sequence<ElementType>* Concat(Sequence<ElementType>* other) const = 0;
};

template<typename ElementType>
class ArraySequence : public Sequence<ElementType>
{
private:
    DynamicArray<ElementType>* array;

public:
    ArraySequence() : array(new DynamicArray<ElementType>()) {}

    ArraySequence(ElementType* items, int count) : array(new DynamicArray<ElementType>())
    {
        for (int i = 0; i < count; ++i)
        {
            array->push_back(items[i]);
        }
    }

    ArraySequence(const ArraySequence<ElementType>& other) : array(new DynamicArray<ElementType>())
    {
        for (int i = 0; i < other.GetLength(); ++i)
        {
            array->push_back(other.Get(i));
        }
    }

    ~ArraySequence() override
    {
        delete array;
    }

    ElementType GetFirst() const override
    {
        if (array->get_size() == 0) throw std::out_of_range("Empty sequence");
        return array->get(0);
    }

    ElementType GetLast() const override
    {
        if (array->get_size() == 0) throw std::out_of_range("Empty sequence");
        return array->get(array->get_size() - 1);
    }

    ElementType Get(int index) const override
    {
        if (index < 0 || index >= array->get_size())
        {
            throw std::out_of_range("Index out of range");
        }
        return array->get(index);
    }

    int GetLength() const override
    {
        return array->get_size();
    }

    Sequence<ElementType>* Append(ElementType item) override
    {
        array->push_back(item);
        return this;
    }

    Sequence<ElementType>* Prepend(ElementType item) override
    {
        DynamicArray<ElementType>* new_array = new DynamicArray<ElementType>();
        new_array->push_back(item);

        for (int i = 0; i < array->get_size(); ++i)
        {
            new_array->push_back(array->get(i));
        }

        delete array;
        array = new_array;
        return this;
    }

    Sequence<ElementType>* InsertAt(ElementType item, int index) override
    {
        if (index < 0 || index > array->get_size())
        {
            throw std::out_of_range("Index out of range");
        }

        DynamicArray<ElementType>* new_array = new DynamicArray<ElementType>();

        for (int i = 0; i < index; ++i)
        {
            new_array->push_back(array->get(i));
        }

        new_array->push_back(item);

        for (int i = index; i < array->get_size(); ++i)
        {
            new_array->push_back(array->get(i));
        }

        delete array;
        array = new_array;
        return this;
    }

    Sequence<ElementType>* GetSubsequence(int startIndex, int endIndex) const override
    {
        if (startIndex < 0 || endIndex >= array->get_size() || startIndex > endIndex)
        {
            throw std::out_of_range("Invalid index range");
        }

        ArraySequence<ElementType>* subseq = new ArraySequence<ElementType>();
        for (int i = startIndex; i <= endIndex; ++i)
        {
            subseq->array->push_back(array->get(i));
        }

        return subseq;
    }

    Sequence<ElementType>* Concat(Sequence<ElementType>* other) const override
    {
        ArraySequence<ElementType>* result = new ArraySequence<ElementType>(*this);
        for (int i = 0; i < other->GetLength(); ++i)
        {
            result->array->push_back(other->Get(i));
        }
        return result;
    }
    template<typename ResultType>
    Sequence<ResultType>* Map(ResultType(*mapper)(ElementType)) const
    {
        Sequence<ResultType>* result = new ArraySequence<ResultType>();
        for (int i = 0; i < GetLength(); ++i)
        {
            result->Append(mapper(Get(i)));
        }
        return result;
    }
    template<typename ResultType>
    ResultType Reduce(ResultType(*reducer)(ResultType, ElementType), ResultType initial) const
    {
        ResultType accumulator = initial;
        for (int i = 0; i < GetLength(); ++i)
        {
            accumulator = reducer(accumulator, Get(i));
        }
        return accumulator;
    }
    Sequence<ElementType>* Where(bool (*predicate)(ElementType)) const
    {
        Sequence<ElementType>* result = new ArraySequence<ElementType>();
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
    template<typename OtherType, typename ResultType>
    Sequence<ResultType>* Zip(Sequence<OtherType>* other, ResultType(*zipper)(ElementType, OtherType)) const
    {
        Sequence<ResultType>* result = new ArraySequence<ResultType>();
        int minLength = std::min(GetLength(), other->GetLength());

        for (int i = 0; i < minLength; ++i)
        {
            result->Append(zipper(Get(i), other->Get(i)));
        }
        return result;
    }
};