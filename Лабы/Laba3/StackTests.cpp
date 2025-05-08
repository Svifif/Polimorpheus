#include "StackTests.hpp"

void testStackPush()
{
    Stack<int> s;
    s.push(10);
    assert(s.top() == 10);
    assert(s.size() == 1);

    s.push(20);
    assert(s.top() == 20);
    assert(s.size() == 2);

    std::cout << "testStackPush passed!\n";
}

void testStackPop()
{
    Stack<int> s;
    s.push(10);
    s.push(20);

    s.pop();
    assert(s.top() == 10);
    assert(s.size() == 1);

    s.pop();
    assert(s.empty());

    std::cout << "testStackPop passed!\n";
}

void testStackTop() 
{
    Stack<std::string> s;
    s.push("first");
    s.push("second");

    assert(s.top() == "second");
    s.pop();
    assert(s.top() == "first");

    std::cout << "testStackTop passed!\n";
}

void testStackEmpty() 
{
    Stack<int> s;
    assert(s.empty());

    s.push(10);
    assert(!s.empty());

    s.pop();
    assert(s.empty());

    std::cout << "testStackEmpty passed!\n";
}

void testStackSize() 
{
    Stack<int> s;
    assert(s.size() == 0);

    s.push(10);
    assert(s.size() == 1);

    s.push(20);
    assert(s.size() == 2);

    s.clear();
    assert(s.size() == 0);

    std::cout << "testStackSize passed!\n";
}

void testStackClear()
{
    Stack<int> s;
    s.push(10);
    s.push(20);
    s.clear();

    assert(s.empty());
    assert(s.size() == 0);

    std::cout << "testStackClear passed!\n";
}

void testStackExceptions()
{
    Stack<int> s;
    bool exception_thrown = false;

    try 
    {
        s.pop();
    }
    catch (const std::out_of_range&) 
    {
        exception_thrown = true;
    }
    assert(exception_thrown);

    exception_thrown = false;
    try
    {
        s.top();
    }
    catch (const std::out_of_range&) 
    {
        exception_thrown = true;
    }
    assert(exception_thrown);

    std::cout << "testStackExceptions passed!\n";
}

void runAllStackTests() 
{
    testStackPush();
    testStackPop();
    testStackTop();
    testStackEmpty();
    testStackSize();
    testStackClear();
    testStackExceptions();

    std::cout << "All Stack tests passed!\n";
}