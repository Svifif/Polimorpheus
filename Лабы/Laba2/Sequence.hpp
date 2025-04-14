#pragma once
#include <stdexcept>
#include <algorithm>
#include <utility>

template<typename ElementType>
class Sequence
{
public:
    virtual ~Sequence() = default;

    virtual ElementType GetFirst() const = 0;
    virtual ElementType& GetLast() const = 0;
    virtual ElementType& Get(int index) const = 0;
    //virtual Sequence<ElementType>* GetSubsequence(int startIndex, int endIndex)  = 0;
    virtual int GetLength() const = 0;

    virtual Sequence<ElementType>* Append(ElementType item) = 0;
    virtual Sequence<ElementType>* Prepend(ElementType item) = 0;
    virtual Sequence<ElementType>* InsertAt(ElementType item, int index) = 0;
    //virtual Sequence<ElementType>* Concat(const Sequence<ElementType>& other) = 0;

    template<typename OtherType, typename ResultType, typename ResultSequenceType>
    ResultSequenceType Zip(Sequence<OtherType>* other,
        ResultType(*zipper)(ElementType, OtherType)) const
    {
        if (other == nullptr) {
            throw std::invalid_argument("Other sequence cannot be null");
        }
        if (zipper == nullptr) {
            throw std::invalid_argument("Zipper function cannot be null");
        }

        // ���������� ��������� CreateEmpty
        ResultSequenceType result;

        const int minLength = std::min(this->GetLength(), other->GetLength());
        for (int i = 0; i < minLength; ++i) {
            result.Append(zipper(this->Get(i), other->Get(i)));
        }
        return result;
    }

    template<typename T1, typename T2, typename ResultSequenceType1, typename ResultSequenceType2>
    static std::pair<ResultSequenceType1, ResultSequenceType2> Unzip(Sequence<std::pair<T1, T2>>* zipped) {
        if (zipped == nullptr) {
            throw std::invalid_argument("Zipped sequence cannot be null");
        }

        // ���������� ��������� CreateEmpty
        ResultSequenceType1 firstSeq;
        ResultSequenceType2 secondSeq;

        for (int i = 0; i < zipped->GetLength(); ++i) {
            auto pair = zipped->Get(i);
            firstSeq.Append(pair.first);
            secondSeq.Append(pair.second);
        }
        //return std::pair<ResultSequenceType1, ResultSequenceType2>(firstSeq, secondSeq);
        return std::make_pair(firstSeq, secondSeq);
    }

    Sequence<Sequence<ElementType>*>* Split(bool (*splitCondition)(ElementType)) const
    {
        if (splitCondition == nullptr)
        {
            throw std::invalid_argument("Split condition cannot be null");
        }

        // ������� �������������� ������������������ �������������������
        Sequence<Sequence<ElementType>*>* result =
            Sequence<Sequence<ElementType>*>::template CreateEmpty<Sequence<ElementType>*>();

        // ������� ������� ���� ���� �� ����, ��� � �������� ������������������
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

        // ��������� ��������� ����, ���� �� �� ������
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

    Sequence<ElementType>* Where(bool (*predicate)(ElementType))
    {
        if (!predicate)
        {
            throw std::invalid_argument("Predicate function cannot be null");
        }

        auto result = this->CreateEmpty();
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

    virtual Sequence<ElementType>* GetSubsequence(int startIndex, int endIndex) 
    {
        if (startIndex < 0 || endIndex >= GetLength() || startIndex > endIndex)
        {
            throw std::out_of_range("Invalid subsequence indices");
        }

        // �������� ����� ������ ����� GetThis() (��� MLL - this, ��� ILL - �����)
        auto result = this->CreateEmpty();

        // ��������� ����������������������
        for (int i = startIndex; i <= endIndex; ++i)
        {
            result->Append(Get(i));
        }

        return result;
    }

    virtual Sequence<ElementType>* Concat(const Sequence<ElementType>& other)
    {
        if (this == &other)
        {
            throw std::invalid_argument("Cannot concatenate sequence with itself");
        }

        // ������� ����� ������������������ ����� CreateEmpty()
        Sequence<ElementType>* result = GetThis();

        // ��������� �������� �� ������ ������������������
        for (int i = 0; i < other.GetLength(); ++i)
        {
            result->Append(other.Get(i));
        }

        return result;
    }

    template<typename ResultType, typename ResultSequenceType>
    ResultSequenceType Map(ResultType(*mapper)(ElementType))
    {
        if (!mapper)
        {
            throw std::invalid_argument("Mapper function cannot be null");
        }

        ResultSequenceType result;
        for (int i = 0; i < GetLength(); ++i)
        {
            result.Append(mapper(Get(i)));
        }
        return result;
    }

protected:

    virtual Sequence<ElementType>* CreateEmpty()  = 0;
    virtual Sequence<ElementType>* GetThis() = 0;

};