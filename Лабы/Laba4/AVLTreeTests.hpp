#ifndef AVLTREE_TESTS_HPP
#define AVLTREE_TESTS_HPP

#include "AVLTree.h"
#include <iostream>
#include <string>
#include <vector>

class AVLTreeTests {
public:
    static void runAllTests();

private:
    // Test declarations
    static void testInsert();
    static void testRemove();
    static void testContains();
    static void testBalancing();
    static void testTraversals();
    static void testExceptions();
    static void testEmptyTree();

    // Helper functions
    template <typename T>
    static void assertTrue(const T& condition, const std::string& message);

    template <typename T>
    static void assertFalse(const T& condition, const std::string& message);

    template <typename T, typename U>
    static void assertEqual(const T& actual, const U& expected, const std::string& message);

    template <typename Func>
    static void assertThrows(Func func, const std::string& message);
};

#endif // AVLTREE_TESTS_HPP