#include "TLinkedList.hpp"


void test_destructor1()
{
    {
        LinkedList<TrackedObject> list;
        list.push_back(TrackedObject{});
        list.push_back(TrackedObject{});
        assert(TrackedObject::counter == 2);
    }
    assert(TrackedObject::counter == 0);
}

void test_constructors1()
{
    // Default constructor
    {
        LinkedList<int> list;
        assert(list.empty());
        assert(list.get_size() == 0);
    }

    // Copy constructor
    {
        LinkedList<int> list1;
        list1.push_back(1);
        list1.push_back(2);

        LinkedList<int> list2(list1);
        assert(list2.get_size() == 2);
        assert(list2[0] == 1);
        assert(list2[1] == 2);
    }

    // Move constructor
    {
        LinkedList<int> list1;
        list1.push_back(1);
        list1.push_back(2);

        LinkedList<int> list2(std::move(list1));
        assert(list2.get_size() == 2);
        assert(list2[0] == 1);
        assert(list2[1] == 2);
        assert(list1.empty());
    }
}

void test_assignment1()
{
    // Copy assignment
    {
        LinkedList<int> list1;
        list1.push_back(1);
        list1.push_back(2);

        LinkedList<int> list2;
        list2 = list1;
        assert(list2.get_size() == 2);
        assert(list2[0] == 1);
        assert(list2[1] == 2);
    }

    // Move assignment
    {
        LinkedList<int> list1;
        list1.push_back(1);
        list1.push_back(2);

        LinkedList<int> list2;
        list2 = std::move(list1);
        assert(list2.get_size() == 2);
        assert(list2[0] == 1);
        assert(list2[1] == 2);
        assert(list1.empty());
    }
}

void test_accessors1()
{
    LinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    // operator[]
    assert(list[0] == 1);
    assert(list[1] == 2);
    assert(list[2] == 3);

    // get_data
    assert(list.get_data(0) == 1);
    assert(list.get_data(1) == 2);
    assert(list.get_data(2) == 3);

    // front/back
    assert(list.front() == 1);
    assert(list.back() == 3);

    // const versions
    const LinkedList<int>& clist = list;
    assert(clist[0] == 1);
    assert(clist.front() == 1);
    assert(clist.back() == 3);

    // out of bounds checks
    try
    {
        list[3];
        assert(false);
    }
    catch (const std::out_of_range&)
    {
        assert(true);
    }
}

void test_modifiers1()
{
    // push_front/push_back
    {
        LinkedList<int> list;
        list.push_front(2);
        list.push_front(1);
        list.push_back(3);

        assert(list.get_size() == 3);
        assert(list[0] == 1);
        assert(list[1] == 2);
        assert(list[2] == 3);
    }

    // pop_front/pop_back
    {
        LinkedList<int> list;
        list.push_back(1);
        list.push_back(2);
        list.push_back(3);

        list.pop_front();
        assert(list.get_size() == 2);
        assert(list[0] == 2);

        list.pop_back();
        assert(list.get_size() == 1);
        assert(list[0] == 2);
    }

    // insert/pop at position
    {
        LinkedList<int> list;
        list.push_back(1);
        list.push_back(3);

        list.insert(1, 2);
        assert(list.get_size() == 3);
        assert(list[0] == 1);
        assert(list[1] == 2);
        assert(list[2] == 3);

        list.pop(1);
        assert(list.get_size() == 2);
        assert(list[0] == 1);
        assert(list[1] == 3);
    }

    // clear
    {
        LinkedList<int> list;
        list.push_back(1);
        list.push_back(2);
        list.clear();
        assert(list.empty());
        assert(list.get_size() == 0);
    }
}

void test_concat()
{
    LinkedList<int> list1;
    list1.push_back(1);
    list1.push_back(2);

    LinkedList<int> list2;
    list2.push_back(3);
    list2.push_back(4);

    // Copy concat
    list1 += list2;
    assert(list1.get_size() == 4);
    assert(list1[0] == 1);
    assert(list1[3] == 4);

    // Move concat
    LinkedList<int> list3;
    list3.push_back(5);
    list1 += std::move(list3);
    assert(list1.get_size() == 5);
    assert(list1[4] == 5);
    assert(list3.empty());
}

void test_sublist()
{
    LinkedList<int> list;
    for (int i = 0; i < 5; ++i)
    {
        list.push_back(i);
    }

    auto sublist = list.GetSubList(1, 3);
    assert(sublist.get_size() == 3);
    assert(sublist[0] == 1);
    assert(sublist[2] == 3);
}

void test_iterators()
{
    LinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    // Non-const iterator
    int sum = 0;
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        sum += *it;
    }
    assert(sum == 6);

    // Const iterator
    const LinkedList<int>& clist = list;
    sum = 0;
    for (auto it = clist.begin(); it != clist.end(); ++it)
    {
        sum += *it;
    }
    assert(sum == 6);

    // Range-based for loop
    sum = 0;
    for (int val : list)
    {
        sum += val;
    }
    assert(sum == 6);
}

void run_linked_list_tests(void)
{
    test_destructor1();
    test_constructors1();
    test_assignment1();
    test_accessors1();
    test_modifiers1();
    test_concat();
    test_sublist();
    test_iterators();

    std::cout << "All LinkedList tests passed successfully!" << std::endl;
}