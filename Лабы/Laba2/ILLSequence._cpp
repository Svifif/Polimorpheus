#include "Sequence.cpp"
#include "LinckedList.cpp"

template<typename ElementType>
class ILLSequence : public Sequence<ElementType>
{
private:
    LinkedList<ElementType> list;

    explicit ILLSequence(LinkedList<ElementType>&& otherList) noexcept : list(std::move(otherList)) {}

public:
    ILLSequence() = default;

    ILLSequence(ElementType* items, int count)
    {
        for (int i = 0; i < count; ++i)
        {
            list.push_back(items[i]);
        }
    }

    ILLSequence(const ILLSequence& other) : list(other.list) {}

    ILLSequence(ILLSequence&& other) noexcept : list(std::move(other.list)) {}

    ~ILLSequence() override = default;

    ElementType GetFirst() const override
    {
        return list.front();
    }

    ElementType GetLast() const override
    {
        return list.back();
    }

    ElementType Get(int index) const override
    {
        return list.get_data(index);
    }

    int GetLength() const override
    {
        return list.get_size();
    }

    Sequence<ElementType>* Append(ElementType item) const override
    {
        auto newList = list;
        newList.push_back(item);
        return new ILLSequence(std::move(newList));
    }

    Sequence<ElementType>* Append(ElementType&& item) const override
    {
        auto newList = list;
        newList.push_back(std::move(item));
        return new ILLSequence(std::move(newList));
    }

    Sequence<ElementType>* Prepend(ElementType item) const override
    {
        auto newList = list;
        newList.push_front(item);
        return new ILLSequence(std::move(newList));
    }

    Sequence<ElementType>* Prepend(ElementType&& item) const override
    {
        auto newList = list;
        newList.push_front(std::move(item));
        return new ILLSequence(std::move(newList));
    }

    Sequence<ElementType>* InsertAt(ElementType item, int index) const override
    {
        auto newList = list;
        newList.insert(index, item);
        return new ILLSequence(std::move(newList));
    }

    Sequence<ElementType>* InsertAt(ElementType&& item, int index) const override
    {
        auto newList = list;
        newList.insert(index, std::move(item));
        return new ILLSequence(std::move(newList));
    }

    Sequence<ElementType>* GetSubsequence(int startIndex, int endIndex) const override
    {
        auto subList = list.GetSubList(startIndex, endIndex);
        return new ILLSequence(std::move(subList));
    }

    Sequence<ElementType>* Concat(Sequence<ElementType>* other) const override
    {
        if (!other) throw std::invalid_argument("Other sequence cannot be null");

        auto newList = list;
        for (int i = 0; i < other->GetLength(); ++i)
        {
            newList.push_back(other->Get(i));
        }
        return new ILLSequence(std::move(newList));
    }

    Sequence<ElementType>* Concat(Sequence<ElementType>&& other) const override
    {
        if (auto* otherList = dynamic_cast<ILLSequence*>(&other))
        {
            auto newList = list;
            newList.concat(std::move(otherList->list));
            return new ILLSequence(std::move(newList));
        }
        throw std::invalid_argument("Invalid sequence type");
    }

    template<typename ResultType>
    Sequence<ResultType>* Map(ResultType(*mapper)(ElementType)) const
    {
        auto* result = new ILLSequence<ResultType>();
        for (auto it = list.begin(); it != list.end(); ++it)
        {
            result->Append(mapper(*it));
        }
        return result;
    }

    template<typename ResultType>
    ResultType Reduce(ResultType(*reducer)(ResultType, ElementType), ResultType initial) const
    {
        ResultType accumulator = initial;
        for (auto it = list.begin(); it != list.end(); ++it)
        {
            accumulator = reducer(accumulator, *it);
        }
        return accumulator;
    }

    Sequence<ElementType>* Where(bool (*predicate)(ElementType)) const
    {
        auto* result = new ILLSequence<ElementType>();
        for (auto it = list.begin(); it != list.end(); ++it)
        {
            if (predicate(*it))
            {
                result->Append(*it);
            }
        }
        return result;
    }

protected:
    Sequence<ElementType>* CreateEmpty() const override
    {
        return new ILLSequence<ElementType>();
    }
};