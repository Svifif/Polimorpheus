#include "PriorityQueueTests.hpp"

void testPriorityQueuePush()
{
    PriorityQueue<int> pq;
    pq.push(10);
    assert(pq.top() == 10);
    assert(pq.size() == 1);

    pq.push(30);
    pq.push(20);
    assert(pq.top() == 30); // Максимальный элемент наверху (по умолчанию std::less — max-heap)
    assert(pq.size() == 3);

    std::cout << "testPriorityQueuePush passed!" << std::endl;
}

void testPriorityQueuePop()
{
    PriorityQueue<int> pq;
    pq.push(10);
    pq.push(30);
    pq.push(20);

    pq.pop();
    assert(pq.top() == 20);
    assert(pq.size() == 2);

    pq.pop();
    assert(pq.top() == 10);
    assert(pq.size() == 1);

    pq.pop();
    assert(pq.empty());

    std::cout << "testPriorityQueuePop passed!" << std::endl;
}

void testPriorityQueueTop()
{
    PriorityQueue<int> pq;
    pq.push(50);
    pq.push(10);
    pq.push(40);

    assert(pq.top() == 50);
    pq.pop();
    assert(pq.top() == 40);
    pq.pop();
    assert(pq.top() == 10);

    std::cout << "testPriorityQueueTop passed!" << std::endl;
}

void testPriorityQueueSize()
{
    PriorityQueue<int> pq;
    assert(pq.size() == 0);

    pq.push(10);
    assert(pq.size() == 1);

    pq.push(20);
    assert(pq.size() == 2);

    pq.clear();
    assert(pq.size() == 0);

    std::cout << "testPriorityQueueSize passed!" << std::endl;
}

void testPriorityQueueEmpty()
{
    PriorityQueue<int> pq;
    assert(pq.empty());

    pq.push(10);
    assert(!pq.empty());

    pq.pop();
    assert(pq.empty());

    std::cout << "testPriorityQueueEmpty passed!" << std::endl;
}

void testPriorityQueueClear()
{
    PriorityQueue<int> pq;
    pq.push(10);
    pq.push(20);
    pq.clear();

    assert(pq.empty());
    assert(pq.size() == 0);

    std::cout << "testPriorityQueueClear passed!" << std::endl;
}

void testPriorityQueueCustomComparator()
{
    // Тест с кастомным компаратором (min-heap)
    PriorityQueue<int, std::greater<int>> minHeap;
    minHeap.push(10);
    minHeap.push(30);
    minHeap.push(20);

    assert(minHeap.top() == 10); // Минимальный элемент наверху
    minHeap.pop();
    assert(minHeap.top() == 20);

    std::cout << "testPriorityQueueCustomComparator passed!" << std::endl;
}

void runAllPriorityQueueTests()
{
    testPriorityQueuePush();
    testPriorityQueuePop();
    testPriorityQueueTop();
    testPriorityQueueSize();
    testPriorityQueueEmpty();
    testPriorityQueueClear();
    testPriorityQueueCustomComparator();

    std::cout << "All PriorityQueue tests passed!" << std::endl;
}