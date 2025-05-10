#pragma once
#include "SquareMatrix.h"
#include <cassert>
#include <iostream>
#include <complex>

class SquareMatrixTests
{
public:
    static void runAllTests();

private:
    static void testConstructor();
    static void testSwapRows();
    static void testAddRows();
    static void testMultiplyRow();
    static void testExceptions();

    template<typename T>
    static void printMatrix(const SquareMatrix<T>& mat);
};