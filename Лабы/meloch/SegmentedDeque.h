#pragma once
#include "LinkedList.hpp"
#include "DynamicArray.hpp"
#include <stdexcept>

template <typename T>
class SegmentedDeque
{
private:
    LinkedList<DynamicArray<T>> segments;

public:
    // 1. Добавление нового пустого сегмента
    void add_segment()
    {
        segments.push_back(DynamicArray<T>());
    }

    // 2. Добавление готового DynamicArray как сегмента
    void add_segment(DynamicArray<T>&& ready_segment)
    {
        segments.push_back(std::move(ready_segment));
    }

    // 3. Удаление сегмента по индексу
    void remove_segment(size_t segment_index)
    {
        if (segment_index >= segments.get_size())
        {
            throw std::out_of_range("Segment index out of range");
        }
        segments.pop(segment_index);
    }

    // 4. Доступ к сегменту по индексу
    DynamicArray<T>& get_segment(size_t segment_index)
    {
        if (segment_index >= segments.get_size())
        {
            throw std::out_of_range("Segment index out of range");
        }
        return segments[segment_index];
    }

    // 5. Вставка элемента в конкретную позицию
    void insert_element(size_t segment_index, size_t element_index, const T& value)
    {
        DynamicArray<T>& segment = get_segment(segment_index);
        if (element_index > segment.get_size())
        {
            throw std::out_of_range("Element index out of range in segment");
        }
        segment.insert(value, element_index);
    }

    // 6. Получение элемента по индексам
    T& get_element(size_t segment_index, size_t element_index)
    {
        DynamicArray<T>& segment = get_segment(segment_index);
        if (element_index >= segment.get_size())
        {
            throw std::out_of_range("Element index out of range in segment");
        }
        return segment[element_index];
    }

    // 7. Удаление элемента по индексам
    void remove_element(size_t segment_index, size_t element_index)
    {
        DynamicArray<T>& segment = get_segment(segment_index);
        if (element_index >= segment.get_size())
        {
            throw std::out_of_range("Element index out of range in segment");
        }
        segment.remove(element_index);
    }

    // 8. Общее количество сегментов
    size_t segments_count() const
    {
        return segments.get_size();
    }

    // 9. Размер конкретного сегмента
    size_t segment_size(size_t segment_index) const
    {
        if (segment_index >= segments.get_size())
        {
            throw std::out_of_range("Segment index out of range");
        }
        return segments[segment_index].get_size();
    }

    // 10. Очистка всего дека
    void clear()
    {
        segments.clear();
    }

    // 11. Обмен двух сегментов местами
    void swap_segments(size_t seg1, size_t seg2)
    {
        if (seg1 >= segments.get_size() || seg2 >= segments.get_size())
        {
            throw std::out_of_range("Segment index out of range");
        }
        segments[seg1].swap(segments[seg2]);
    }

    // 12. Проверка на пустоту (все сегменты пусты)
    bool is_empty() const
    {
        for (size_t i = 0; i < segments.get_size(); ++i)
        {
            if (segments[i].get_size() > 0)
            {
                return false;
            }
        }
        return true;
    }
};