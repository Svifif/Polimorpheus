#pragma once
#pragma once
#include "LazySequence.hpp"
#include <iostream>
#include <stdexcept>
#include <cassert>
#include <string>

class LazySequenceTests
{
public:
    static void test_constructors();
    static void test_lazy_generation();
    static void test_accessors();
    static void test_mutation();
    static void test_composition();
    static void test_functional_operations();
    static void test_move_semantics();
    static void test_exceptions();
    static void test_all();
};