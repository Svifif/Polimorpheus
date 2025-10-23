#pragma once
#include "SmartPointer.hpp"
#include <iostream>
#include <cassert>
#include <stdexcept>

// Вспомогательный класс для отслеживания созданий/удалений
class TrackedObject 
{
public:
    static int constructorCount;
    static int destructorCount;
    int value;

    TrackedObject(int v = 0) : value(v) { constructorCount++; }
    ~TrackedObject() { destructorCount++; }

    static void resetCounters() 
    {
        constructorCount = 0;
        destructorCount = 0;
    }
};

int TrackedObject::constructorCount = 0;
int TrackedObject::destructorCount = 0;

class SmartPointerTests
{
public:
    static void test_constructors() 
    {
        std::cout << "Testing SmartPointer constructors...\n";

        TrackedObject::resetCounters();

        // Test default constructor
        SmartPointer<TrackedObject> ptr1;
        assert(!ptr1); // Should be null
        assert(ptr1.use_count() == 0);
        assert(TrackedObject::constructorCount == 0);

        // Test single object constructor
        SmartPointer<TrackedObject> ptr2(new TrackedObject(42));
        assert(ptr2);
        assert(ptr2.use_count() == 1);
        assert(ptr2->value == 42);
        assert(TrackedObject::constructorCount == 1);

        // Test array constructor
        SmartPointer<int> arr(new int[5], SmartPointer<int>::ARRAY);
        assert(arr);
        assert(arr.is_array());
        assert(arr.use_count() == 1);

        std::cout << "Constructor tests passed!\n";
    }

    static void test_single_object() 
    {
        std::cout << "Testing single object functionality...\n";

        TrackedObject::resetCounters();

        {
            SmartPointer<TrackedObject> ptr1(new TrackedObject(100));
            assert(ptr1->value == 100);
            assert(ptr1.use_count() == 1);
            assert(TrackedObject::constructorCount == 1);

            // Modify through pointer
            ptr1->value = 200;
            assert(ptr1->value == 200);
        } // ptr1 goes out of scope

        assert(TrackedObject::destructorCount == 1); // Object should be destroyed

        std::cout << "Single object tests passed!\n";
    }

    static void test_array() 
    {
        std::cout << "Testing array functionality...\n";

        {
            SmartPointer<int> arr = SmartPointer<int>::make_array(3);
            assert(arr);
            assert(arr.is_array());
            assert(arr.use_count() == 1);

            // Test array access
            arr[0] = 10;
            arr[1] = 20;
            arr[2] = 30;

            assert(arr[0] == 10);
            assert(arr[1] == 20);
            assert(arr[2] == 30);

            // Test array copy
            SmartPointer<int> arr2 = arr;
            assert(arr.use_count() == 2);
            assert(arr2.use_count() == 2);

            arr2[0] = 100;
            assert(arr[0] == 100); // Should share the same array
        } // Array should be properly deleted with delete[]

        std::cout << "Array tests passed!\n";
    }

    static void test_copy_semantics() 
    {
        std::cout << "Testing copy semantics...\n";

        TrackedObject::resetCounters();

        SmartPointer<TrackedObject> ptr1(new TrackedObject(50));
        assert(ptr1.use_count() == 1);
        assert(TrackedObject::constructorCount == 1);

        // Test copy constructor
        SmartPointer<TrackedObject> ptr2 = ptr1;
        assert(ptr1.use_count() == 2);
        assert(ptr2.use_count() == 2);
        assert(ptr1->value == 50);
        assert(ptr2->value == 50);

        // Test assignment operator
        SmartPointer<TrackedObject> ptr3;
        ptr3 = ptr1;
        assert(ptr1.use_count() == 3);
        assert(ptr3.use_count() == 3);

        // Test self-assignment
        ptr1 = ptr1;
        assert(ptr1.use_count() == 3); // Should remain unchanged

        // Destroy copies - object should persist
        {
            SmartPointer<TrackedObject> ptr4 = ptr1;
            assert(ptr1.use_count() == 4);
        } // ptr4 destroyed

        assert(ptr1.use_count() == 3); // Back to 3
        assert(TrackedObject::destructorCount == 0); // Object still alive

        std::cout << "Copy semantics tests passed!\n";
    }

    static void test_move_semantics() 
    {
        std::cout << "Testing move semantics...\n";

        TrackedObject::resetCounters();

        SmartPointer<TrackedObject> ptr1(new TrackedObject(75));
        assert(ptr1.use_count() == 1);

        // Test move constructor
        SmartPointer<TrackedObject> ptr2 = std::move(ptr1);
        assert(!ptr1); // Should be empty after move
        assert(ptr2.use_count() == 1);
        assert(ptr2->value == 75);
        assert(ptr1.use_count() == 0);

        // Test move assignment
        SmartPointer<TrackedObject> ptr3;
        ptr3 = std::move(ptr2);
        assert(!ptr2);
        assert(ptr3.use_count() == 1);
        assert(ptr3->value == 75);

        std::cout << "Move semantics tests passed!\n";
    }

    static void test_access_operators() 
    {
        std::cout << "Testing access operators...\n";

        // Test dereference operator
        SmartPointer<int> ptr(new int(42));
        assert(*ptr == 42);
        *ptr = 100;
        assert(*ptr == 100);

        // Test arrow operator with struct
        struct TestStruct 
        {
            int a, b;
            TestStruct(int x, int y) : a(x), b(y) {}
        };

        SmartPointer<TestStruct> sptr(new TestStruct(1, 2));
        assert(sptr->a == 1);
        assert(sptr->b == 2);
        sptr->a = 10;
        assert(sptr->a == 10);

        // Test array subscript
        SmartPointer<int> arr = SmartPointer<int>::make_array(2);
        arr[0] = 5;
        arr[1] = 6;
        assert(arr[0] == 5);
        assert(arr[1] == 6);

        std::cout << "Access operator tests passed!\n";
    }

    static void test_exceptions() 
    {
        std::cout << "Testing exceptions...\n";

        SmartPointer<int> nullPtr;
        SmartPointer<int> validPtr(new int(42));
        SmartPointer<int> singlePtr(new int(10));
        SmartPointer<int> arrayPtr = SmartPointer<int>::make_array(3);

        // Test null pointer dereference
        try 
        {
            *nullPtr;
            assert(false); // Should not reach here
        }
        catch (const std::runtime_error&) {}

        try 
        {
            nullPtr.operator->();
            assert(false);
        }
        catch (const std::runtime_error&) {}

        // Test array access on non-array
        try
        {
            singlePtr[0];
            assert(false);
        }
        catch (const std::runtime_error&) {}

        // Test array bounds (компилятор не проверяет границы, но наша логика проверяет тип)
        try 
        {
            arrayPtr[10]; // Выход за границы - undefined behavior, но наш оператор [] доступен
            // Не можем проверить границы в тесте, но хотя бы не падаем на доступе к null
        }
        catch (...) {}

        std::cout << "Exception tests passed!\n";
    }

    static void test_factory_methods() 
    {
        std::cout << "Testing factory methods...\n";

        TrackedObject::resetCounters();

        // Test make_single without arguments
        auto ptr1 = SmartPointer<TrackedObject>::make_single();
        assert(ptr1);
        assert(ptr1.use_count() == 1);
        assert(TrackedObject::constructorCount == 1);

        // Test make_single with arguments
        auto ptr2 = SmartPointer<TrackedObject>::make_single(999);
        assert(ptr2);
        assert(ptr2->value == 999);
        assert(ptr2.use_count() == 1);

        // Test make_array
        auto arr = SmartPointer<double>::make_array(4);
        assert(arr);
        assert(arr.is_array());
        assert(arr.use_count() == 1);

        for (int i = 0; i < 4; ++i) 
        {
            arr[i] = i * 1.5;
            assert(arr[i] == i * 1.5);
        }

        std::cout << "Factory method tests passed!\n";
    }

    static void test_all() 
    {
        std::cout << "=== Running SmartPointer Tests ===\n";

        test_constructors();
        test_single_object();
        test_array();
        test_copy_semantics();
        test_move_semantics();
        test_access_operators();
        test_exceptions();
        test_factory_methods();

        std::cout << "=== All SmartPointer tests passed! ===\n\n";
    }
};