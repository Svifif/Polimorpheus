#include "DynamicArray.cpp"
template<typename ElementType>
class Sequence
{
	virtual ElementType GetFirst() {};
	virtual ElementType GetLast() {};
	virtual ElementType Get(int index) {};
	virtual Sequence<ElementType>* GetSubsequence(int index1, int index2) {};
	virtual int GetLength() {};
	virtual Sequence<ElementType>* Append(ElementType item) {};
	virtual Sequence<ElementType>* Prepend(ElementType item) {};
	virtual Sequence<ElementType>* InsertAt(ElementType item, int index) {};
	virtual Sequence <ElementType>* Concat(Sequence <ElementType>* list) {};
};


template<typename ElementType>
class ArraySequence : public Sequence<ElementType>
{
protected:
    DynamicArray<ElementType>* array;

public:

    ArraySequence() : array(new DynamicArray<ElementType>()) {}
    ArraySequence(T* items, int count) : array(new DynamicArray<ElementType>())
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

    virtual ~ArraySequence() 
    {
        delete array;
    }


    ElementType GetFirst() override
    {
        if (array->get_size() == 0) throw std::out_of_range("Empty sequence");
        return array->get(0);
    }

    ElementType GetLast() override
    {
        if (array->get_size() == 0) throw std::out_of_range("Empty sequence");
        return array->get(array->get_size() - 1);
    }

    ElementType Get(int index) override 
    {
        return array->get(index);
    }

    int GetLength() override
    {
        return array->get_size();
    }

    virtual Sequence<ElementType>* Append(ElementType item) override
    {
        array->push_back(item);
        return this;
    }

    virtual Sequence<ElementType>* Prepend(T item) override
    {
        DynamicArray<ElementType>* new_array = new DynamicArray<ElementType>(array->get_size() + 1);
        new_array->push_back(item);

        for (int i = 0; i < array->get_size(); ++i)
        {
            new_array->push_back(array->get(i));
        }

        delete array;
        array = new_array;
        return this;
    }

    virtual Sequence<ElementType>* InsertAt(ElementType item, int index) override
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

    virtual Sequence<ElementType>* GetSubsequence(int startIndex, int endIndex) override 
    {
        if (startIndex < 0 || endIndex >= array->get_size() || startIndex > endIndex) 
        {
            throw std::out_of_range("Invalid index range");
        }

        ArraySequence<ElementType>* subseq = new ArraySequence<ElementType>();
        for (int i = startIndex; i <= endIndex; ++i)
        {
            subseq->Append(array->get(i));
        }

        return subseq;
    }

    virtual Sequence<ElementType>* Concat(Sequence<ElementType>* other) override
    {
        ArraySequence<ElementType>* result = new ArraySequence<ElementType>(*this);
        for (int i = 0; i < other->GetLength(); ++i)
        {
            result->Append(other->Get(i));
        }
        return result;
    }
};