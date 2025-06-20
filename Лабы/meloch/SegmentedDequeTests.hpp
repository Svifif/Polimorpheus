#pragma once
#include "SegmentedDeque.h"
#include <cassert>
#include <iostream>
#include <string>

class SegmentedDequeTests
{
public:
    static void runAllTests();

private:
    static void testConstructor();
    static void testSegmentOperations();
    static void testElementOperations();
    static void testEdgeCases();
    static void testExceptions();

    template<typename T>
    static void printDeque(const SegmentedDeque<T>& deque);
};