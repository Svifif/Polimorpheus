#pragma once
#include "Vector.h"
#include <complex>
#include <cassert>
#include <iostream>

class VectorTests 
{
public:
    static void runAllTests();

private:
    static void testDefaultConstructor();
    static void testSizeConstructor();
    static void testArrayConstructor();
    static void testInitializerListConstructor();
    static void testSizeMethod();
    static void testIndexOperator();
    static void testAddition();
    static void testScalarMultiplication();
    static void testDotProduct();
    static void testNorm();
    static void testComplexNorm();

    template<typename T>
    static void printVector(const Vector<T>& vec);
};