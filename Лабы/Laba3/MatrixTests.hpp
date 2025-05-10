#pragma once
#include "Matrix.h"
#include <complex>
#include <cassert>
#include <iostream>

class MatrixTests
{
public:
    static void runAllTests();

private:
    static void testConstructor();
    static void testDimensions();
    static void testIndexOperator();
    static void testAddition();
    static void testScalarMultiplication();
    static void testNorm();
    static void testComplexNorm();

    template<typename T>
    static void printMatrix(const Matrix<T>& mat);
};