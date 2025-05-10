#pragma once
#include "DiagonalMatrix.h"
#include <cassert>
#include <iostream>
#include <complex>

class DiagonalMatrixTests 
{
public:
    static void runAllTests();

private:
    static void testConstructor();
    static void testSize();
    static void testAccessOperator();
    static void testVectorMultiplication();
    static void testNorm();
    static void testComplexNorm();
    static void testExceptions();

    template<typename T>
    static void printDiagonal(const DiagonalMatrix<T>& mat);
};