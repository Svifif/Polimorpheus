#include "LazySequence.hpp"
#include <stdexcept>

// Конструктор по умолчанию
template <class T>
LazySequence<T>::LazySequence()
    : cash(new MArraySequence<T>()), ExCntr(0), rule(nullptr)
{
}

// Конструктор из массива
template <class T>
LazySequence<T>::LazySequence(T* items, int count)
    : cash(new MArraySequence<T>(items, count)), ExCntr(0), rule(nullptr)
{
}

// Конструктор с правилом генерации
template <class T>
LazySequence<T>::LazySequence(std::function<T(int)> genFunc, int k)
    : cash(new MArraySequence<T>()), ExCntr(0), rule(new std::function<T(int)>(std::move(genFunc)))
{
    if (!rule) {
        throw std::invalid_argument("rule is empty");
    }

    for (int i = 0; i < k; ++i) {
        cash->Append((*rule)(i));
        ExCntr++;
    }
}

// Конструктор из MArraySequence
template <class T>
LazySequence<T>::LazySequence(MArraySequence<T>* seq)
    : cash(seq ? seq : new MArraySequence<T>()),
    ExCntr(cash->GetLength()),
    rule(nullptr)
{
}

// Копирующий конструктор
template <class T>
LazySequence<T>::LazySequence(const LazySequence<T>& other)
    : cash(new MArraySequence<T>(*other.cash)),
    ExCntr(other.ExCntr)
{
    if (other.rule) {
        rule = new std::function<T(int)>(*other.rule);
    }
    else {
        rule = nullptr;
    }
}

// Move конструктор
template <class T>
LazySequence<T>::LazySequence(LazySequence<T>&& other) noexcept
    : cash(other.cash), ExCntr(other.ExCntr), rule(other.rule)
{
    other.cash = nullptr;
    other.ExCntr = 0;
    other.rule = nullptr;
}

// Оператор присваивания
template <class T>
LazySequence<T>& LazySequence<T>::operator=(const LazySequence<T>& other)
{
    if (this != &other) {
        delete cash;
        delete rule;

        cash = new MArraySequence<T>(*other.cash);
        ExCntr = other.ExCntr;

        if (other.rule) {
            rule = new std::function<T(int)>(*other.rule);
        }
        else {
            rule = nullptr;
        }
    }
    return *this;
}

// Move оператор присваивания
template <class T>
LazySequence<T>& LazySequence<T>::operator=(LazySequence<T>&& other) noexcept
{
    if (this != &other) {
        delete cash;
        delete rule;

        cash = other.cash;
        rule = other.rule;
        ExCntr = other.ExCntr;

        other.cash = nullptr;
        other.rule = nullptr;
        other.ExCntr = 0;
    }
    return *this;
}

// Деструктор
template <class T>
LazySequence<T>::~LazySequence()
{
    delete cash;
    delete rule;
}

// Метод Get с ленивой генерацией
template <class T>
T LazySequence<T>::Get(int index)
{
    if (index < 0) {
        throw std::out_of_range("Negative index");
    }

    if (rule && index >= cash->GetLength()) {
        for (int i = cash->GetLength(); i <= index; ++i) {
            cash->Append((*rule)(i));
            ExCntr++;
        }
    }

    if (index >= cash->GetLength()) {
        throw std::out_of_range("Index out of range");
    }

    return cash->Get(index);
}

template <class T>
T LazySequence<T>::GetFirst()
{
    return Get(0);
}

template <class T>
T LazySequence<T>::GetLast()
{
    if (cash->GetLength() == 0) {
        throw std::out_of_range("Sequence is empty");
    }
    return cash->Get(cash->GetLength() - 1);
}

template <class T>
Cardinal LazySequence<T>::GetLength() const
{
    return Cardinal(0, cash->GetLength());
}

template <class T>
size_t LazySequence<T>::GetExCntr() const
{
    return ExCntr;
}

template <class T>
LazySequence<T>* LazySequence<T>::Append(T item)
{
    cash->Append(item);
    return this;
}

template <class T>
LazySequence<T>* LazySequence<T>::Prepend(T item)
{
    cash->Prepend(item);
    return this;
}

template <class T>
LazySequence<T>* LazySequence<T>::InsertAt(T item, int index)
{
    cash->InsertAt(item, index);
    return this;
}

template <class T>
LazySequence<T>* LazySequence<T>::GetSubsequence(int startIndex, int endIndex)
{
    MArraySequence<T>* subArray = cash->GetSubsequence(startIndex, endIndex);
    return new LazySequence<T>(subArray);
}

// Используем готовые методы из Sequence
template <class T>
template <class T2>
LazySequence<T2>* LazySequence<T>::Map(T2(*func)(T))
{
    Sequence<T2>* result = cash->Map(func);
    return new LazySequence<T2>(dynamic_cast<MArraySequence<T2>*>(result));
}

template <class T>
template <class T2>
T2 LazySequence<T>::Reduce(T2(*func)(T2, T), T2 initial)
{
    return cash->Reduce(func, initial);
}

template <class T>
LazySequence<T>* LazySequence<T>::Where(bool (*predicate)(T))
{
    Sequence<T>* result = cash->Where(predicate);
    return new LazySequence<T>(dynamic_cast<MArraySequence<T>*>(result));
}

template <class T>
LazySequence<T>* LazySequence<T>::Zip(LazySequence<T>* seq)
{
    Sequence<T>* result = cash->Zip(seq->cash);
    return new LazySequence<T>(dynamic_cast<MArraySequence<T>*>(result));
}

// Новые методы для работы с MixedLazySequence
template <class T>
MixedLazySequence<T>* LazySequence<T>::Concat(LazySequence<T>* list)
{
    return new MixedLazySequence<T>(this, Cardinal(0, cash->GetLength()), list);
}

template <class T>
MixedLazySequence<T>* LazySequence<T>::InsertLSeq(LazySequence<T>* seq, Cardinal position)
{
    return new MixedLazySequence<T>(this, position, seq);
}

// Явные инстанциации
template class LazySequence<int>;
template class LazySequence<double>;
template class LazySequence<float>;