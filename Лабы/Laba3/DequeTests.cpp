#include "DequeTests.hpp"

void testDequePushFront()
{
    Deque<int> deque;
    deque.push_front(10);
    assert(deque.front() == 10);
    assert(deque.size() == 1);

    deque.push_front(20);
    assert(deque.front() == 20);
    assert(deque.size() == 2);

    std::cout << "testDequePushFront passed!" << std::endl;
}

void testDequePushBack()
{
    Deque<int> deque;
    deque.push_back(10);
    assert(deque.back() == 10);
    assert(deque.size() == 1);

    deque.push_back(20);
    assert(deque.back() == 20);
    assert(deque.size() == 2);

    std::cout << "testDequePushBack passed!" << std::endl;
}

void testDequePopFront()
{
    Deque<int> deque;
    deque.push_back(10);
    deque.push_back(20);

    deque.pop_front();
    assert(deque.front() == 20);
    assert(deque.size() == 1);

    deque.pop_front();
    assert(deque.empty());

    std::cout << "testDequePopFront passed!" << std::endl;
}

void testDequePopBack()
{
    Deque<int> deque;
    deque.push_back(10);
    deque.push_back(20);

    deque.pop_back();
    assert(deque.back() == 10);
    assert(deque.size() == 1);

    deque.pop_back();
    assert(deque.empty());

    std::cout << "testDequePopBack passed!" << std::endl;
}

void testDequeFront()
{
    Deque<int> deque;
    deque.push_front(30);
    assert(deque.front() == 30);

    deque.push_front(40);
    assert(deque.front() == 40);

    std::cout << "testDequeFront passed!" << std::endl;
}

void testDequeBack()
{
    Deque<int> deque;
    deque.push_back(30);
    assert(deque.back() == 30);

    deque.push_back(40);
    assert(deque.back() == 40);

    std::cout << "testDequeBack passed!" << std::endl;
}

void testDequeSize()
{
    Deque<int> deque;
    assert(deque.size() == 0);

    deque.push_back(10);
    assert(deque.size() == 1);

    deque.push_front(20);
    assert(deque.size() == 2);

    deque.clear();
    assert(deque.size() == 0);

    std::cout << "testDequeSize passed!" << std::endl;
}

void testDequeEmpty()
{
    Deque<int> deque;
    assert(deque.empty());

    deque.push_back(10);
    assert(!deque.empty());

    deque.pop_back();
    assert(deque.empty());

    std::cout << "testDequeEmpty passed!" << std::endl;
}

void testDequeClear()
{
    Deque<int> deque;
    deque.push_back(10);
    deque.push_front(20);
    deque.clear();

    assert(deque.empty());
    assert(deque.size() == 0);

    std::cout << "testDequeClear passed!" << std::endl;
}

void runAllDequeTests()
{
    testDequePushFront();
    testDequePushBack();
    testDequePopFront();
    testDequePopBack();
    testDequeFront();
    testDequeBack();
    testDequeSize();
    testDequeEmpty();
    testDequeClear();

    std::cout << "All Deque tests passed!" << std::endl;
}