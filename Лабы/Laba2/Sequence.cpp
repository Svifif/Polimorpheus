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
    template<typename OtherType, typename ResultType>
    Sequence<ResultType>* Zip(Sequence<OtherType>* other,
        ResultType(*zipper)(ElementType, OtherType)) const
    {
        if (other == nullptr)
        {
            throw std::invalid_argument("Other sequence cannot be null");
        }

        Sequence<ResultType>* result = this->CreateEmpty<ResultType>();
        const int minLength = std::min(this->GetLength(), other->GetLength());

        for (int i = 0; i < minLength; ++i)
        {
            result->Append(zipper(this->Get(i), other->Get(i)));
        }

        return result;
    }

    template<typename T1, typename T2>
    static std::pair<Sequence<T1>*, Sequence<T2>*> Unzip(Sequence<std::pair<T1, T2>>* zipped)
    {
        if (zipped == nullptr)
        {
            throw std::invalid_argument("Zipped sequence cannot be null");
        }

        Sequence<T1>* firstSeq = Sequence<T1>::CreateEmpty();
        Sequence<T2>* secondSeq = Sequence<T2>::CreateEmpty();

        for (int i = 0; i < zipped->GetLength(); ++i)
        {
            auto pair = zipped->Get(i);
            firstSeq->Append(pair.first);
            secondSeq->Append(pair.second);
        }

        return { firstSeq, secondSeq };
    }

    Sequence<Sequence<ElementType>*>* Split(bool (*splitCondition)(ElementType)) const
    {
        Sequence<Sequence<ElementType>*>* result = this->template CreateEmpty<Sequence<ElementType>*>();
        Sequence<ElementType>* currentChunk = this->CreateEmpty();

        for (int i = 0; i < this->GetLength(); ++i)
        {
            ElementType item = this->Get(i);
            if (splitCondition(item)) 
            {
                if (currentChunk->GetLength() > 0)
                {
                    result->Append(currentChunk);
                    currentChunk = this->CreateEmpty();
                }
            }
            else 
            {
                currentChunk->Append(item);
            }
        }

        if (currentChunk->GetLength() > 0) 
        {
            result->Append(currentChunk);
        }
        else 
        {
            delete currentChunk;
        }

        return result;
    }

protected:
    virtual Sequence<ElementType>* CreateEmpty() const = 0;

    template<typename T>
    static Sequence<T>* CreateEmpty() 
    {
        return new ArraySequence<T>(); 
    }
};


