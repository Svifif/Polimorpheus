#pragma once
#include "Sequence.hpp"
#include "ASequence.hpp"
#include "TrackedObject.hpp"
#include <iostream>
#include <stdexcept>
#include <cassert>
#include <string>
//#include <utility>

class SequenceTests 
{
public:
    static void test_constructors();
    static void test_accessors();
    static void test_mutation();
    static void test_concat();
    static void test_exceptions();
    static void test_move_semantics();
    static void test_where_operation();
    static void test_map_operation();
    static void test_with_pairs();
    static void test_zip_operation();
    static void test_unzip_operation();
    static void test_destructor();
    static void test_all();
};
