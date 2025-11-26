#pragma once
#include "ASequence.hpp"
#include "Cardinal.hpp"
#include <stdexcept> 
#include <algorithm>
#include <utility>
#include <functional>

template <class T>
class LazySequence
{
private:
    MArraySequence<T>* cash;
    size_t ExCntr;
    std::function<T(int)>* rule;

public:
    // Конструкторы
    LazySequence() : cash(new MArraySequence<T>()), ExCntr(0), rule(nullptr)
    {
    }

    explicit LazySequence(T* items, int count) : cash(new MArraySequence<T>(items, count)), ExCntr(0), rule(nullptr)
    {
    }

    explicit LazySequence(MArraySequence<T>* seq) : cash(seq ? seq : new MArraySequence<T>()), ExCntr(cash ? cash->GetLength() : 0), rule(nullptr)
    {
    }

    explicit LazySequence(std::function<T(int)> genFunc, int k) : cash(new MArraySequence<T>()), ExCntr(0), rule(new std::function<T(int)>(std::move(genFunc)))
    {
        if (!rule)
        {
            throw std::invalid_argument("rule is empty");
        }
        for (int i = 0; i < k; ++i)
        {
            cash->Append((*rule)(i));
            ExCntr++;
        }
    }

    LazySequence(const LazySequence& other) : cash(other.cash ? new MArraySequence<T>(*other.cash) : nullptr), ExCntr(other.ExCntr)
    {
        if (other.rule)
        {
            rule = new std::function<T(int)>(*other.rule);
        }
        else
        {
            rule = nullptr;
        }
    }

    LazySequence(LazySequence&& other) noexcept : cash(other.cash), ExCntr(other.ExCntr), rule(other.rule)
    {
        other.cash = nullptr;
        other.ExCntr = 0;
        other.rule = nullptr;
    }

    LazySequence& operator=(const LazySequence& other)
    {
        if (this != &other)
        {
            delete cash;
            delete rule;
            cash = other.cash ? new MArraySequence<T>(*other.cash) : nullptr;
            ExCntr = other.ExCntr;
            if (other.rule)
            {
                rule = new std::function<T(int)>(*other.rule);
            }
            else
            {
                rule = nullptr;
            }
        }
        return *this;
    }

    LazySequence& operator=(LazySequence&& other) noexcept
    {
        if (this != &other)
        {
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

    ~LazySequence()
    {
        delete cash;
        delete rule;
    }

    // Методы доступа
    T Get(int index)
    {
        if (!cash)
        {
            throw std::runtime_error("Accessing destroyed sequence");
        }
        if (index < 0)
        {
            throw std::out_of_range("Negative index");
        }
        if (rule && index >= cash->GetLength())
        {
            for (int i = cash->GetLength(); i <= index; ++i)
            {
                cash->Append((*rule)(i));
                ExCntr++;
            }
        }
        if (index >= cash->GetLength())
        {
            throw std::out_of_range("Index out of range");
        }
        return cash->Get(index);
    }

    T GetFirst()
    {
        return Get(0);
    }

    T GetLast()
    {
        if (!cash || cash->GetLength() == 0)
        {
            throw std::out_of_range("Sequence is empty");
        }
        return cash->Get(cash->GetLength() - 1);
    }

    Cardinal GetLength() const
    {
        return cash ? Cardinal(0, cash->GetLength()) : Cardinal(0, 0);
    }

    size_t GetExCntr() const
    {
        return ExCntr;
    }

    // Методы модификации
    LazySequence* Append(T item)
    {
        if (cash)
        {
            cash->Append(item);
        }
        return this;
    }

    LazySequence* Prepend(T item)
    {
        if (cash)
        {
            cash->Prepend(item);
        }
        return this;
    }

    LazySequence* InsertAt(T item, int index)
    {
        if (cash)
        {
            cash->InsertAt(item, index);
        }
        return this;
    }

    LazySequence* GetSubsequence(int startIndex, int endIndex)
    {
        if (!cash)
        {
            throw std::runtime_error("Accessing destroyed sequence");
        }

        // Создаем подпоследовательность вручную (безопаснее)
        MArraySequence<T>* newArray = new MArraySequence<T>();
        for (int i = startIndex; i <= endIndex; ++i)
        {
            newArray->Append(this->Get(i));
        }
        return new LazySequence(newArray);
    }

    // Композиционные методы
    LazySequence* Concat(LazySequence* list)
    {
        if (!cash || !list || !list->cash)
        {
            throw std::invalid_argument("Invalid sequences for concatenation");
        }

        // Безопасная ручная реализация конкатенации
        MArraySequence<T>* newArray = new MArraySequence<T>();

        // Копируем элементы из текущей последовательности
        for (int i = 0; i < this->GetLength().index; ++i)
        {
            newArray->Append(this->Get(i));
        }

        // Копируем элементы из второй последовательности
        for (int i = 0; i < list->GetLength().index; ++i)
        {
            newArray->Append(list->Get(i));
        }

        return new LazySequence(newArray);
    }

    // Функциональные методы
    template<class T2>
    LazySequence<T2>* Map(T2(*func)(T))
    {
        if (!cash)
        {
            throw std::runtime_error("Accessing destroyed sequence");
        }

        // Ручная реализация Map
        MArraySequence<T2>* resultArray = new MArraySequence<T2>();
        for (int i = 0; i < cash->GetLength(); ++i)
        {
            resultArray->Append(func(cash->Get(i)));
        }
        return new LazySequence<T2>(resultArray);
    }

    template<class T2>
    T2 Reduce(T2(*func)(T2, T), T2 initial)
    {
        if (!cash)
        {
            throw std::runtime_error("Accessing destroyed sequence");
        }

        // Ручная реализация Reduce
        T2 result = initial;
        for (int i = 0; i < cash->GetLength(); ++i)
        {
            result = func(result, cash->Get(i));
        }
        return result;
    }

    LazySequence* Where(bool (*predicate)(T))
    {
        if (!cash)
        {
            throw std::runtime_error("Accessing destroyed sequence");
        }

        // Ручная реализация Where
        MArraySequence<T>* resultArray = new MArraySequence<T>();
        for (int i = 0; i < cash->GetLength(); ++i)
        {
            T current = cash->Get(i);
            if (predicate(current))
            {
                resultArray->Append(current);
            }
        }
        return new LazySequence(resultArray);
    }

    LazySequence* Zip(LazySequence* seq, T(*zipper)(T, T))
    {
        if (!zipper)
        {
            throw std::invalid_argument("Zipper function cannot be null");
        }
        if (!cash || !seq || !seq->cash)
        {
            throw std::invalid_argument("Invalid sequences for zip");
        }

        // Ручная реализация Zip
        MArraySequence<T>* resultArray = new MArraySequence<T>();
        int minLength = std::min(this->GetLength().index, seq->GetLength().index);

        for (int i = 0; i < minLength; ++i)
        {
            resultArray->Append(zipper(this->Get(i), seq->Get(i)));
        }

        return new LazySequence(resultArray);
    }
};
/*
template<class T>
class LasyPointer
{
public:
    LazySequence<T>* parent;
    Cardinal breakPoint;  // Граничный индекс, ДО которого работает этот сегмент

    LasyPointer() : parent(nullptr), breakPoint(0, 0) {}

    explicit LasyPointer(LazySequence<T>* seq, Cardinal breakPt)
        : parent(seq), breakPoint(breakPt) {
    }

    bool isValid() const { return parent != nullptr; }
    Cardinal GetBreakPoint() const { return breakPoint; }
    LazySequence<T>* GetParent() const { return parent; }

    // Проверяем, попадает ли индекс в этот сегмент
    bool containsIndex(const Cardinal& index) const {
        return index < breakPoint;  // Используем наш оператор <
    }
};

template<class T>
class MixedLazySequence
{
private:
    MArraySequence<LasyPointer<T>> segments;

public:
    MixedLazySequence() = default;

    MixedLazySequence(LazySequence<T>* seq1, Cardinal index, LazySequence<T>* seq2)
    {
        // Первый сегмент: работает ДО index
        segments.Append(LasyPointer<T>(seq1, index));

        // Второй сегмент: работает ДО (index + длина seq2)
        Cardinal seq2Length = seq2->GetLength();
        Cardinal seq2End = index + Cardinal(0, seq2Length.index); // Исправил GetLength()
        segments.Append(LasyPointer<T>(seq2, seq2End));

        // Третий сегмент: работает ДО бесконечности (максимальное значение)
        segments.Append(LasyPointer<T>(seq1, Cardinal(INT_MAX, INT_MAX)));
    }

    MixedLazySequence(const MixedLazySequence<T>& other)
        : segments(other.segments)
    {
    }

    MixedLazySequence(MixedLazySequence<T>&& other)
        : segments(std::move(other.segments))
    {
    }

    T Get(const Cardinal& globalIndex)
    {
        Cardinal accumulated(0, 0);

        for (int i = 0; i < segments.GetLength(); i++) {
            auto& segment = segments.Get(i);
            Cardinal segmentEnd = segment.GetBreakPoint();

            // Если order совпадает И индекс попадает в сегмент
            if (globalIndex.order == accumulated.order && globalIndex < segmentEnd) {
                Cardinal localIndex = Cardinal(0, globalIndex.index - accumulated.index);
                return segment.GetParent()->Get(localIndex.index);
            }

            // Если это сегмент продолжения (order>0) И мы ищем элемент с таким же order
            if (segmentEnd.order > accumulated.order && globalIndex.order == segmentEnd.order) {
                // ДЛЯ ПРОДОЛЖЕНИЯ: accumulated обнуляется для нового order!
                Cardinal localIndex = Cardinal(0, globalIndex.index); // accumulated = 0 для нового order
                return segment.GetParent()->Get(localIndex.index + accumulated.index);
            }

            // Увеличиваем accumulated только если order совпадает
            if (segmentEnd.order == accumulated.order) {
                accumulated.index = segmentEnd.index;
            }
            else {
                // При переходе на новый order accumulated сбрасывается в 0 для этого order
                accumulated = Cardinal(segmentEnd.order, 0);
            }
        }

        throw std::out_of_range("Index out of range");
    }

    MixedLazySequence<T>* InsertLSeq(LazySequence<T>* seq, Cardinal position)
    {
        auto* result = new MixedLazySequence<T>();

        bool inserted = false;

        for (int i = 0; i < segments.GetLength(); i++) {
            auto& currentSegment = segments.Get(i);
            Cardinal segmentStart = (i == 0) ? Cardinal(0, 0) : segments.Get(i - 1).GetBreakPoint();
            Cardinal segmentEnd = currentSegment.GetBreakPoint();

            // Если позиция вставки находится внутри этого сегмента
            if (!inserted && position < segmentEnd) {
                // Разбиваем текущий сегмент на части

                // 1. Часть ДО вставки
                if (position > segmentStart) {
                    result->segments.Append(LasyPointer<T>(
                        currentSegment.GetParent(),
                        position
                    ));
                }

                // 2. Вставляемая последовательность
                Cardinal insertEnd = position + Cardinal(0, seq->GetLength().index);
                result->segments.Append(LasyPointer<T>(seq, insertEnd));

                // 3. Часть ПОСЛЕ вставки
                result->segments.Append(LasyPointer<T>(
                    currentSegment.GetParent(),
                    segmentEnd
                ));

                inserted = true;
            }
            else {
                // Просто копируем сегмент
                result->segments.Append(currentSegment);
            }
        }

        // Если не вставили (позиция больше всех сегментов), добавляем в конец
        if (!inserted) {
            Cardinal insertEnd = segments.Get(segments.GetLength() - 1).GetBreakPoint() +
                Cardinal(0, seq->GetLength().index);
            result->segments.Append(LasyPointer<T>(seq, insertEnd));
        }

        return result;
    }

    ~MixedLazySequence() = default;

    // Дополнительные методы
    size_t GetSegmentCount() const { return segments.GetLength(); }

    Cardinal GetTotalLength() const
    {
        if (segments.GetLength() == 0) return Cardinal(0, 0);

        // Берем breakPoint последнего сегмента (самый большой)
        return segments.Get(segments.GetLength() - 1).GetBreakPoint();
    }
};
*/

