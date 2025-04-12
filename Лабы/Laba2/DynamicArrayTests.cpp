#include "DynamicArrayTests.hpp"


void test_destructor()
{
    {
        DynamicArray<TrackedObject> array;
        array.push_back(TrackedObject{});
        array.push_back(TrackedObject{});
        assert(TrackedObject::counter == 2);
    }
    assert(TrackedObject::counter == 0);
}

void test_constructors()
{
    // Default constructor
    {
        DynamicArray<int> array;
        assert(array.get_size() == 0);
        assert(array.get_capacity() == 0);
    }

    // Constructor with capacity
    {
        DynamicArray<int> array(10);
        assert(array.get_size() == 0);
        assert(array.get_capacity() == 10);
    }

    // Copy constructor
    {
        DynamicArray<int> array1;
        array1.push_back(1);
        array1.push_back(2);

        DynamicArray<int> array2(array1);
        assert(array2.get_size() == 2);
        assert(array2[0] == 1);
        assert(array2[1] == 2);
    }

    // Move constructor
    {
        DynamicArray<int> array1;
        array1.push_back(1);
        array1.push_back(2);

        DynamicArray<int> array2(std::move(array1));
        assert(array2.get_size() == 2);
        assert(array2[0] == 1);
        assert(array2[1] == 2);
        assert(array1.get_size() == 0);
    }
}

void test_assignment()
{
    // Copy assignment
    {
        DynamicArray<int> array1;
        array1.push_back(1);
        array1.push_back(2);

        DynamicArray<int> array2;
        array2 = array1;
        assert(array2.get_size() == 2);
        assert(array2[0] == 1);
        assert(array2[1] == 2);
    }

    // Move assignment
    {
        DynamicArray<int> array1;
        array1.push_back(1);
        array1.push_back(2);

        DynamicArray<int> array2;
        array2 = std::move(array1);
        assert(array2.get_size() == 2);
        assert(array2[0] == 1);
        assert(array2[1] == 2);
        assert(array1.get_size() == 0);
    }
}

void test_accessors()
{
    DynamicArray<int> array;
    array.push_back(1);
    array.push_back(2);
    array.push_back(3);

    // operator[]
    assert(array[0] == 1);
    assert(array[1] == 2);
    assert(array[2] == 3);

    // get()
    assert(array.get(0) == 1);
    assert(array.get(1) == 2);
    assert(array.get(2) == 3);

    // get_size()/get_capacity()
    assert(array.get_size() == 3);
    assert(array.get_capacity() >= 3);

    // out of bounds checks
    try
    {
        array.get(3);
        assert(false);
    }
    catch (const std::out_of_range&)
    {
        assert(true);
    }
}

void test_modifiers()
{
    // push_back (lvalue)
    {
        DynamicArray<int> array;
        int value = 42;
        array.push_back(value);
        assert(array.get_size() == 1);
        assert(array[0] == 42);
    }

    // push_back (rvalue)
    {
        DynamicArray<std::string> array;
        array.push_back("test");
        assert(array.get_size() == 1);
        assert(array[0] == "test");
    }

    // set (lvalue)
    {
        DynamicArray<int> array;
        array.push_back(1);
        int value = 2;
        array.set(value, 0);
        assert(array[0] == 2);
    }

    // set (rvalue)
    {
        DynamicArray<std::string> array;
        array.push_back("old");
        array.set("new", 0);
        assert(array[0] == "new");
    }

    // set out of bounds
    try
    {
        DynamicArray<int> array;
        array.set(1, 0);
        assert(false);
    }
    catch (const std::out_of_range&)
    {
        assert(true);
    }

    // Test automatic resizing through push_back
    {
        DynamicArray<int> array;
        size_t initial_capacity = array.get_capacity();

        for (int i = 0; i < 100; ++i)
        {
            array.push_back(i);
        }

        assert(array.get_size() == 100);
        assert(array.get_capacity() > initial_capacity);
    }
}

void test_swap()
{
    DynamicArray<int> array1;
    array1.push_back(1);
    array1.push_back(2);

    DynamicArray<int> array2;
    array2.push_back(3);

    array1.swap(array2);

    assert(array1.get_size() == 1);
    assert(array1[0] == 3);
    assert(array2.get_size() == 2);
    assert(array2[0] == 1);
    assert(array2[1] == 2);
}

void run_dynamic_array_tests()
{
    test_destructor();
    test_constructors();
    test_assignment();
    test_accessors();
    test_modifiers();
    test_swap();

    std::cout << "All DynamicArray tests passed successfully!" << std::endl;
}