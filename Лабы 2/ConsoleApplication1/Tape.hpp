#pragma once
#include "LazySequence.hpp"
#include <stdexcept>

template<typename T>
class Tape
{
private:
    LazySequence<T> right;
    LazySequence<T> left;
    T current;
    T blankSymbol;

public:
    explicit Tape(T blank = T()) : blankSymbol(blank), current(blank)
    {
    }

    void initialize(const T* data, int size, int startPos = 0)
    {
        if (startPos < 0 || startPos >= size)
        {
            throw std::out_of_range("Invalid start position");
        }

        current = data[startPos];

        // Правая часть
        if (startPos + 1 < size)
        {
            T* rightData = new T[size - startPos - 1];
            for (int i = 0; i < size - startPos - 1; ++i)
            {
                rightData[i] = data[startPos + 1 + i];
            }
            right = LazySequence<T>(rightData, size - startPos - 1);
            delete[] rightData;
        }
        else
        {
            right = LazySequence<T>();
        }

        // Левая часть
        if (startPos > 0)
        {
            T* leftData = new T[startPos];
            for (int i = 0; i < startPos; ++i)
            {
                leftData[i] = data[startPos - 1 - i];
            }
            left = LazySequence<T>(leftData, startPos);
            delete[] leftData;
        }
        else
        {
            left = LazySequence<T>();
        }
    }

    T read() const
    {
        return current;
    }

    void write(T symbol)
    {
        current = symbol;
    }

    void moveRight()
    {
        // Добавляем текущий символ в левую часть
        T leftItem[1] = { current };
        LazySequence<T> newLeftPart(leftItem, 1);

        if (left.GetLength().index > 0)
        {
            LazySequence<T>* concatResult = newLeftPart.Concat(&left);
            left = *concatResult;
            delete concatResult;
        }
        else
        {
            left = newLeftPart;
        }

        // Берем следующий символ из правой части
        if (right.GetLength().index > 0)
        {
            current = right.GetFirst();
            if (right.GetLength().index > 1)
            {
                LazySequence<T>* sub = right.GetSubsequence(1, right.GetLength().index - 1);
                right = *sub;
                delete sub;
            }
            else
            {
                right = LazySequence<T>();
            }
        }
        else
        {
            current = blankSymbol;
        }
    }

    void moveLeft()
    {
        // Добавляем текущий символ в правую часть
        T rightItem[1] = { current };
        LazySequence<T> newRightPart(rightItem, 1);

        if (right.GetLength().index > 0)
        {
            LazySequence<T>* concatResult = newRightPart.Concat(&right);
            right = *concatResult;
            delete concatResult;
        }
        else
        {
            right = newRightPart;
        }

        // Берем предыдущий символ из левой части
        if (left.GetLength().index > 0)
        {
            current = left.GetFirst();
            if (left.GetLength().index > 1)
            {
                LazySequence<T>* sub = left.GetSubsequence(1, left.GetLength().index - 1);
                left = *sub;
                delete sub;
            }
            else
            {
                left = LazySequence<T>();
            }
        }
        else
        {
            current = blankSymbol;
        }
    }

    void getContents(T*& output, int& size) const
    {
        int leftSize = left.GetLength().index;
        int rightSize = right.GetLength().index;
        size = leftSize + 1 + rightSize;

        output = new T[size];

        for (int i = 0; i < leftSize; ++i)
        {
            output[leftSize - 1 - i] = left.Get(i);
        }

        output[leftSize] = current;

        for (int i = 0; i < rightSize; ++i)
        {
            output[leftSize + 1 + i] = right.Get(i);
        }
    }

    T getBlankSymbol() const
    {
        return blankSymbol;
    }
};