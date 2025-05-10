#include "MatrixTests.hpp"

void MatrixTests::runAllTests()
{
    testConstructor();
    testDimensions();
    testIndexOperator();
    testAddition();
    testScalarMultiplication();
    testNorm();
    testComplexNorm();
    std::cout << "All Matrix tests passed successfully!\n";
}

void MatrixTests::testConstructor()
{
    Matrix<int> mat(2, 3);
    assert(mat.getRows() == 2);
    assert(mat.getCols() == 3);

    for (size_t i = 0; i < mat.getRows(); ++i) 
    {
        for (size_t j = 0; j < mat.getCols(); ++j) 
        {
            assert(mat[i][j] == 0);
        }
    }
}

void MatrixTests::testDimensions()
{
    Matrix<double> mat(4, 5);
    assert(mat.getRows() == 4);
    assert(mat.getCols() == 5);
}

void MatrixTests::testIndexOperator()
{
    Matrix<int> mat(2, 2);
    mat[0][0] = 1;
    mat[0][1] = 2;
    mat[1][0] = 3;
    mat[1][1] = 4;

    assert(mat[0][0] == 1);
    assert(mat[0][1] == 2);
    assert(mat[1][0] == 3);
    assert(mat[1][1] == 4);

    const Matrix<int>& constMat = mat;
    assert(constMat[0][0] == 1);
}

void MatrixTests::testAddition() 
{
    Matrix<int> mat1(2, 2);
    mat1[0][0] = 1; mat1[0][1] = 2;
    mat1[1][0] = 3; mat1[1][1] = 4;

    Matrix<int> mat2(2, 2);
    mat2[0][0] = 5; mat2[0][1] = 6;
    mat2[1][0] = 7; mat2[1][1] = 8;

    Matrix<int> result = mat1 + mat2;

    assert(result[0][0] == 6);
    assert(result[0][1] == 8);
    assert(result[1][0] == 10);
    assert(result[1][1] == 12);

    try
    {
        Matrix<int> mat3(3, 3);
        mat1 + mat3;
        assert(false); // Shouldn't reach here
    }
    catch (const std::invalid_argument&)
    {
        // Expected
    }
}

void MatrixTests::testScalarMultiplication() 
{
    Matrix<double> mat(2, 2);
    mat[0][0] = 1.5; mat[0][1] = 2.5;
    mat[1][0] = 3.5; mat[1][1] = 4.5;

    Matrix<double> result = mat * 2.0;

    assert(std::abs(result[0][0] - 3.0) < 1e-10);
    assert(std::abs(result[0][1] - 5.0) < 1e-10);
    assert(std::abs(result[1][0] - 7.0) < 1e-10);
    assert(std::abs(result[1][1] - 9.0) < 1e-10);
}

void MatrixTests::testNorm() 
{
    Matrix<double> mat(2, 2);
    mat[0][0] = 3.0; mat[0][1] = 0.0;
    mat[1][0] = 0.0; mat[1][1] = 4.0;

    double n = mat.norm();
    assert(std::abs(n - 5.0) < 1e-10);
}

void MatrixTests::testComplexNorm() 
{
    using namespace std::complex_literals;
    Matrix<std::complex<double>> mat(2, 2);
    mat[0][0] = 3.0 + 4.0i; mat[0][1] = 0.0 + 0.0i;
    mat[1][0] = 0.0 + 0.0i; mat[1][1] = 6.0 + 8.0i;

    double n = mat.norm();
    double expected = std::sqrt(125.0); // ?(5? + 10?) = ?125
    assert(std::abs(n - expected) < 1e-10);
}

template<typename T>
void MatrixTests::printMatrix(const Matrix<T>& mat) 
{
    for (size_t i = 0; i < mat.getRows(); ++i) 
    {
        for (size_t j = 0; j < mat.getCols(); ++j) 
        {
            std::cout << mat[i][j] << " ";
        }
        std::cout << "\n";
    }
}