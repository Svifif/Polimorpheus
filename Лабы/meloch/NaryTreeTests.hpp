#pragma once
#include "NaryTree.hpp"
#include <cassert>
#include <iostream>
#include <string>

class NaryTreeTests
{
public:
    static void runAllTests()
    {
        testConstructor();
        testInsert();
        testRemove();
        testEdgeCases();
        testExceptions();
        testPrintOperations();
        std::cout << "All NaryTree tests passed successfully!\n";
    }

private:
    static inline void testConstructor()
    {
        // Test basic construction
        NaryTree<int> tree(3);
        assert(tree.isEmpty());
        assert(tree.getArity() == 3);

        // Test invalid arity
        bool exceptionThrown = false;
        try 
        {
            NaryTree<int> invalidTree(0);
        }
        catch (const std::invalid_argument&) 
        {
            exceptionThrown = true;
        }
        assert(exceptionThrown);
    }

    static inline void testInsert()
    {
        NaryTree<std::string> tree(2);

        // Test root insertion
        assert(tree.insert("root", "root"));
        assert(!tree.isEmpty());
        assert(tree.contains("root"));

        // Test child insertion
        assert(tree.insert("root", "child1"));
        assert(tree.insert("root", "child2"));
        assert(tree.contains("child1"));
        assert(tree.contains("child2"));

        // Test arity limit
        bool exceptionThrown = false;
        try
        {
            tree.insert("root", "child3");
        }
        catch (const std::out_of_range&)
        {
            exceptionThrown = true;
        }
        assert(exceptionThrown);

        // Test invalid parent (теперь должно возвращать false, а не бросать исключение)
        assert(!tree.insert("invalid_parent", "test"));
    }

    static inline void testRemove()
    {
        NaryTree<int> tree(3);

        // Build test tree
        tree.insert(10, 10);
        tree.insert(10, 20);
        tree.insert(10, 30);
        tree.insert(20, 40);
        tree.insert(20, 50);

        // Test leaf removal
        assert(tree.remove(50));
        assert(!tree.contains(50));

        // Test node with children removal
        assert(tree.remove(20));
        assert(!tree.contains(20));
        assert(!tree.contains(40));

        // Test root removal
        assert(tree.remove(10));
        assert(tree.isEmpty());
    }

    static inline void testEdgeCases()
    {
        // Test large arity
        NaryTree<int> bigTree(1000);
        bigTree.insert(1, 1);

        for (int i = 0; i < 1000; ++i) 
        {
            bigTree.insert(1, i + 2);
        }
        assert(bigTree.getArity() == 1000);

        // Test arity overflow
        bool exceptionThrown = false;
        try 
        {
            bigTree.insert(1, 1002);
        }
        catch (const std::out_of_range&) 
        {
            exceptionThrown = true;
        }
        assert(exceptionThrown);
    }

    static inline void testExceptions()
    {
        NaryTree<double> tree(2);

        // Test empty tree removal
        bool exceptionThrown = false;
        try
        {
            tree.remove(3.14);
        }
        catch (const std::logic_error&) 
        {
            exceptionThrown = true;
        }
        assert(exceptionThrown);

        // Test root value mismatch
        exceptionThrown = false;
        try
        {
            tree.insert(1.0, 2.0);
        }
        catch (const std::invalid_argument&)
        {
            exceptionThrown = true;
        }
        assert(exceptionThrown);
    }

    static inline void testPrintOperations()
    {
        NaryTree<char> tree(3);
        tree.insert('A', 'A');
        tree.insert('A', 'B');
        tree.insert('A', 'C');
        tree.insert('B', 'D');

        std::cout << "=== Tree Structure Test ===" << std::endl;
        printTree(tree);
        std::cout << "=== End of Test ===" << std::endl;
    }

    template<typename T>
    static void printTree(const NaryTree<T>& tree)
    {
        tree.printLevelOrder();
    }
};