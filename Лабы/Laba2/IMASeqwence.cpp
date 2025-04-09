#include "Sequence.cpp"
#include "DynamicArray.cpp"

template<typename ElementType>
class ImArraySequence : public Sequence<ElementType>
{
private:
    DynamicArray<ElementType>* array;

    explicit ImArraySequence(DynamicArray<ElementType>* arr) : array(arr) {}

public:
    ImArraySequence() : array(new DynamicArray<ElementType>()) {}

    ImArraySequence(ElementType* items, int count) : array(new DynamicArray<ElementType>())
    {
        for (int i = 0; i < count; ++i)
        {
            array->push_back(items[i]);
        }
    }

    ImArraySequence(const ImArraySequence& other) : array(new DynamicArray<ElementType>())
    {
        for (int i = 0; i < other.GetLength(); ++i)
        {
            array->push_back(other.Get(i));
        }
    }

    ImArraySequence(ImArraySequence&& other) noexcept : array(other.array)
    {
        other.array = nullptr;
    }

    ~ImArraySequence() override
    {
        delete array;
    }

    const ElementType& operator[](size_t index) const
    {
        if (index >= array->get_size()) throw std::out_of_range("Index out of range");
        return (*array)[index];
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

    Sequence<ElementType>* Append(ElementType item) const override
    {
        DynamicArray<ElementType>* new_array = new DynamicArray<ElementType>(*array);
        new_array->push_back(item);
        return new ImArraySequence(new_array);
    }

    Sequence<ElementType>* Append(ElementType&& item) const override
    {
        DynamicArray<ElementType>* new_array = new DynamicArray<ElementType>(*array);
        new_array->push_back(std::move(item));
        return new ImArraySequence(new_array);
    }

    Sequence<ElementType>* Prepend(ElementType item) const override
    {
        DynamicArray<ElementType>* new_array = new DynamicArray<ElementType>();
        new_array->push_back(item);
        for (int i = 0; i < array->get_size(); ++i)
        {
            new_array->push_back(array->get(i));
        }
        return new ImArraySequence(new_array);
    }

    Sequence<ElementType>* InsertAt(ElementType item, int index) const override
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
        return new ImArraySequence(new_array);
    }

    Sequence<ElementType>* GetSubsequence(int startIndex, int endIndex) const override
    {
        if (startIndex < 0 || endIndex >= array->get_size() || startIndex > endIndex)
        {
            throw std::out_of_range("Invalid index range");
        }

        DynamicArray<ElementType>* new_array = new DynamicArray<ElementType>();
        for (int i = startIndex; i <= endIndex; ++i)
        {
            new_array->push_back(array->get(i));
        }
        return new ImArraySequence(new_array);
    }

    Sequence<ElementType>* Concat(Sequence<ElementType>* other) const override
    {
        if (!other) throw std::invalid_argument("Other sequence cannot be null");

        DynamicArray<ElementType>* new_array = new DynamicArray<ElementType>(*array);
        for (int i = 0; i < other->GetLength(); ++i)
        {
            new_array->push_back(other->Get(i));
        }
        return new ImArraySequence(new_array);
    }

    Sequence<ElementType>* Concat(Sequence<ElementType>&& other) const override
    {
        if (auto* otherSeq = dynamic_cast<ImArraySequence*>(&other))
        {
            DynamicArray<ElementType>* new_array = new DynamicArray<ElementType>(*array);
            for (int i = 0; i < otherSeq->GetLength(); ++i)
            {
                new_array->push_back(std::move((*otherSeq)[i]));
            }
            return new ImArraySequence(new_array);
        }
        throw std::invalid_argument("Invalid sequence type");
    }

    template<typename ResultType>
    Sequence<ResultType>* Map(ResultType(*mapper)(ElementType)) const
    {
        Sequence<ResultType>* result = new ImArraySequence<ResultType>();
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
        Sequence<ElementType>* result = new ImArraySequence<ElementType>();
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
        return new ImArraySequence<ElementType>();
    }
};