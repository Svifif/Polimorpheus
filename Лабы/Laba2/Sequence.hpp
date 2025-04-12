#pragma once
#include <utility>
#include <stdexcept>
#include <algorithm>

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
        if (zipper == nullptr) 
        {
            throw std::invalid_argument("Zipper function cannot be null");
        }

        Sequence<ResultType>* result = CreateEmpty<ResultType>();
        const int minLength = std::min(this->GetLength(), other->GetLength());

        for (int i = 0; i < minLength; ++i) 
        {
            result->Append(zipper(this->Get(i), other->Get(i)));
        }
        return result;
    }

    template<typename T1, typename T2>
    static std::pair<Sequence<T1>*, Sequence<T2>*> Unzip(Sequence<std::pair<T1, T2>>* zipped) {
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
        return std::make_pair(firstSeq, secondSeq);
    }

    Sequence<Sequence<ElementType>*>* Split(bool (*splitCondition)(ElementType)) const {
        if (splitCondition == nullptr)
        {
            throw std::invalid_argument("Split condition cannot be null");
        }

        Sequence<Sequence<ElementType>*>* result = CreateEmpty<Sequence<ElementType>*>();
        Sequence<ElementType>* currentChunk = CreateEmpty<ElementType>();

        for (int i = 0; i < this->GetLength(); ++i)
        {
            ElementType item = this->Get(i);
            if (splitCondition(item))
            {
                if (currentChunk->GetLength() > 0)
                {
                    result->Append(currentChunk);
                    currentChunk = CreateEmpty<ElementType>();
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
    template<typename T>
    static Sequence<T>* CreateEmpty()
    {
        throw std::runtime_error("CreateEmpty not implemented");
    }
};