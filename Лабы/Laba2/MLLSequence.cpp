

template<typename ElementType>
class MLLSequence : public Sequence<ElementType>
{
private:
    //LinkedList<ElementType> list;

    MLLSequence(const LinkedList<ElementType>& otherList) : list(otherList) {}

public:
    MLLSequence() = default;

    MLLSequence(ElementType* items, int count)
    {
        for (int i = 0; i < count; ++i)
        {
            list.push_back(items[i]);
        }
    }

    MLLSequence(const MLLSequence& other) : list(other.list) {}

    MLLSequence(MLLSequence&& other) noexcept : list(std::move(other.list)) {}

    MLLSequence& operator=(MLLSequence&& other) noexcept
    {
        if (this != &other)
        {
            list = std::move(other.list);
        }
        return *this;
    }

    ~MLLSequence() override = default;

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

    Sequence<ElementType>* Append(ElementType item) override
    {
        list.push_back(item);
        return this;
    }

    Sequence<ElementType>* Append(ElementType&& item) override
    {
        list.push_back(std::move(item));
        return this;
    }

    Sequence<ElementType>* Prepend(ElementType item) override
    {
        list.push_front(item);
        return this;
    }

    Sequence<ElementType>* Prepend(ElementType&& item) override
    {
        list.push_front(std::move(item));
        return this;
    }

    Sequence<ElementType>* InsertAt(ElementType item, int index) override
    {
        list.insert(index, item);
        return this;
    }

    Sequence<ElementType>* InsertAt(ElementType&& item, int index) override
    {
        list.insert(index, std::move(item));
        return this;
    }

    Sequence<ElementType>* GetSubsequence(int startIndex, int endIndex) const override
    {
        auto subList = list.GetSubList(startIndex, endIndex);
        return new MLLSequence(subList);
    }

    Sequence<ElementType>* Concat(Sequence<ElementType>* other) const override
    {
        if (!other) throw std::invalid_argument("Other sequence cannot be null");

        auto* result = new MLLSequence(*this);
        for (int i = 0; i < other->GetLength(); ++i)
        {
            result->list.push_back(other->Get(i));
        }
        return result;
    }

    Sequence<ElementType>* Concat(Sequence<ElementType>&& other) override
    {
        if (auto* otherList = dynamic_cast<MLLSequence*>(&other))
        {
            list.concat(std::move(otherList->list));
            return this;
        }
        throw std::invalid_argument("Invalid sequence type");
    }

    template<typename ResultType>
    Sequence<ResultType>* Map(ResultType(*mapper)(ElementType)) const
    {
        auto* result = new MLLSequence<ResultType>();
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
        auto* result = new MLLSequence<ElementType>();
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
        return new MLLSequence<ElementType>();
    }
};