#include "Sequence.cpp"
#include "DynamicArray.cpp"

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

    ArraySequence(ArraySequence&& other) noexcept
        : array(other.array)
    {
        other.array = nullptr;
    }

    ArraySequence& operator=(ArraySequence&& other) noexcept
    {
        if (this != &other)
        {
            delete array;
            array = other.array;
            other.array = nullptr;
        }
        return *this;
    }

    ElementType& operator[](size_t index) { return (*array)[index]; }
    const ElementType& operator[](size_t index) const { return (*array)[index]; }


    bool operator==(const Sequence<ElementType>& other) const override
    {
        if (GetLength() != other.GetLength()) return false;
        for (int i = 0; i < GetLength(); ++i)
        {
            if (Get(i) != other.Get(i)) return false;
        }
        return true;
    }

    bool operator!=(const Sequence<ElementType>& other) const override
    {
        return !(*this == other);
    }

    ArraySequence& operator+=(const Sequence<ElementType>& other)
    {
        for (int i = 0; i < other.GetLength(); ++i)
        {
            array->push_back(other.Get(i));
        }
        return *this;
    }

    ArraySequence& operator+=(ArraySequence&& other) noexcept
    {
        for (int i = 0; i < other.GetLength(); ++i)
        {
            array->push_back(std::move(other[i]));
        }
        return *this;
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

    Sequence<ElementType>* Append(ElementType&& item) override
    {
        array->push_back(std::move(item));
        return this;
    }

    Sequence<ElementType>* Prepend(ElementType item) override
    {
        DynamicArray<ElementType>* new_array = new DynamicArray<ElementType>();
        new_array->push_back(item);
        for (int i = 0; i < array->get_size(); ++i)
        {
            new_array->push_back(std::move((*array)[i]));
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
            new_array->push_back(std::move((*array)[i]));
        }
        new_array->push_back(item);
        for (int i = index; i < array->get_size(); ++i)
        {
            new_array->push_back(std::move((*array)[i]));
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
        if (!other) throw std::invalid_argument("Other sequence cannot be null");

        ArraySequence<ElementType>* result = new ArraySequence<ElementType>(*this);
        for (int i = 0; i < other->GetLength(); ++i)
        {
            result->array->push_back(other->Get(i));
        }
        return result;
    }

    Sequence<ElementType>* Concat(Sequence<ElementType>&& other) override
    {
        auto* otherArray = dynamic_cast<ArraySequence*>(&other);
        if (!otherArray) throw std::invalid_argument("Invalid sequence type");

        for (int i = 0; i < otherArray->GetLength(); ++i)
        {
            array->push_back(std::move((*otherArray)[i]));
        }
        return this;
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

 

protected:
    Sequence<ElementType>* CreateEmpty() const override
    {
        return new ArraySequence<ElementType>();
    }

    ~ArraySequence() override
    {
        delete array;
    }
};