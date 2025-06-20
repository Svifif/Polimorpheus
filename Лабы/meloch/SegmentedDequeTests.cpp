#include "SegmentedDequeTests.hpp"

void SegmentedDequeTests::runAllTests()
{
    testConstructor();
    testSegmentOperations();
    testElementOperations();
    testEdgeCases();
    testExceptions();
    std::cout << "All SegmentedDeque tests passed successfully!\n";
}

void SegmentedDequeTests::testConstructor()
{
    SegmentedDeque<int> deque;
    assert(deque.segments_count() == 0);
    assert(deque.is_empty());
}

void SegmentedDequeTests::testSegmentOperations()
{
    SegmentedDeque<int> deque;

    // Тест добавления сегментов
    deque.add_segment();
    assert(deque.segments_count() == 1);
    assert(deque.segment_size(0) == 0);

    // Тест добавления готового DynamicArray
    DynamicArray<int> arr;
    arr.push_back(1);
    arr.push_back(2);
    deque.add_segment(std::move(arr));
    assert(deque.segments_count() == 2);
    assert(deque.segment_size(1) == 2);
    assert(deque.get_element(1, 0) == 1);
    assert(deque.get_element(1, 1) == 2);

    // Тест удаления сегмента
    deque.remove_segment(0);
    assert(deque.segments_count() == 1);
    assert(deque.segment_size(0) == 2);
}

void SegmentedDequeTests::testElementOperations()
{
    SegmentedDeque<std::string> deque;
    deque.add_segment();
    deque.add_segment();

    // Тест вставки элементов
    deque.insert_element(0, 0, "first");
    deque.insert_element(0, 1, "second");
    deque.insert_element(1, 0, "third");

    assert(deque.segment_size(0) == 2);
    assert(deque.segment_size(1) == 1);
    assert(deque.get_element(0, 0) == "first");
    assert(deque.get_element(0, 1) == "second");
    assert(deque.get_element(1, 0) == "third");

    // Тест удаления элементов
    deque.remove_element(0, 0);
    assert(deque.segment_size(0) == 1);
    assert(deque.get_element(0, 0) == "second");
}

void SegmentedDequeTests::testEdgeCases()
{
    SegmentedDeque<int> deque;

    // Тест с одним сегментом
    deque.add_segment();
    for (int i = 0; i < 100; i++) {
        deque.insert_element(0, i, i);
    }
    assert(deque.segment_size(0) == 100);

    // Тест с множеством сегментов
    for (int i = 1; i < 10; i++) {
        deque.add_segment();
        deque.insert_element(i, 0, i * 100);
    }
    assert(deque.segments_count() == 10);

    // Проверка содержимого
    assert(deque.get_element(0, 99) == 99);
    assert(deque.get_element(1, 0) == 100);
}

void SegmentedDequeTests::testExceptions()
{
    SegmentedDeque<double> deque;

    try {
        deque.get_element(0, 0);
        assert(false);
    }
    catch (const std::out_of_range&) {}

    try {
        deque.remove_element(0, 0);
        assert(false);
    }
    catch (const std::out_of_range&) {}

    deque.add_segment();
    try {
        deque.insert_element(0, 1, 3.14);
        assert(false);
    }
    catch (const std::out_of_range&) {}

    try {
        deque.remove_segment(1);
        assert(false);
    }
    catch (const std::out_of_range&) {}
}

template<typename T>
void SegmentedDequeTests::printDeque(const SegmentedDeque<T>& deque)
{
    std::cout << "SegmentedDeque contents:\n";
    for (size_t seg = 0; seg < deque.segments_count(); ++seg) {
        std::cout << "Segment " << seg << ": ";
        for (size_t elem = 0; elem < deque.segment_size(seg); ++elem) {
            std::cout << deque.get_element(seg, elem) << " ";
        }
        std::cout << "\n";
    }
}