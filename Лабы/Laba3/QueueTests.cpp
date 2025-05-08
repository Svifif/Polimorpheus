#include "QueueTests.hpp"

void testQueueEnqueue()
{
    Queue<int> q;
    q.enqueue(10);
    assert(q.front() == 10);
    assert(q.size() == 1);

    q.enqueue(20);
    assert(q.front() == 10); // Первый элемент остаётся тем же
    assert(q.size() == 2);

    std::cout << "testQueueEnqueue passed!" << std::endl;
}

void testQueueDequeue()
{
    Queue<int> q;
    q.enqueue(10);
    q.enqueue(20);

    q.dequeue();
    assert(q.front() == 20);
    assert(q.size() == 1);

    q.dequeue();
    assert(q.empty());

    std::cout << "testQueueDequeue passed!" << std::endl;
}

void testQueueFront()
{
    Queue<std::string> q;
    q.enqueue("first");
    q.enqueue("second");

    assert(q.front() == "first");
    q.dequeue();
    assert(q.front() == "second");

    std::cout << "testQueueFront passed!" << std::endl;
}

void testQueueEmpty()
{
    Queue<int> q;
    assert(q.empty());

    q.enqueue(10);
    assert(!q.empty());

    q.dequeue();
    assert(q.empty());

    std::cout << "testQueueEmpty passed!" << std::endl;
}

void testQueueSize()
{
    Queue<int> q;
    assert(q.size() == 0);

    q.enqueue(10);
    assert(q.size() == 1);

    q.enqueue(20);
    assert(q.size() == 2);

    q.clear();
    assert(q.size() == 0);

    std::cout << "testQueueSize passed!" << std::endl;
}

void testQueueClear()
{
    Queue<int> q;
    q.enqueue(10);
    q.enqueue(20);
    q.clear();

    assert(q.empty());
    assert(q.size() == 0);

    std::cout << "testQueueClear passed!" << std::endl;
}

void runAllQueueTests()
{
    testQueueEnqueue();
    testQueueDequeue();
    testQueueFront();
    testQueueEmpty();
    testQueueSize();
    testQueueClear();

    std::cout << "All Queue tests passed!" << std::endl;
}