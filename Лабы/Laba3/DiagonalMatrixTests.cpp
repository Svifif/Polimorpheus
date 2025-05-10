#include "DiagonalMatrixTests.hpp"

void DiagonalMatrixTests::runAllTests()
{
    testConstructor();
    testSize();
    testAccessOperator();
    testVectorMultiplication();
    testNorm();
    testComplexNorm();
    testExceptions();
    std::cout << "All DiagonalMatrix tests passed successfully!\n";
}

void DiagonalMatrixTests::testConstructor()
{
    DiagonalMatrix<int> mat(3);
    assert(mat.size() == 3);

    for (size_t i = 0; i < mat.size(); ++i)
    {
        assert(mat(i) == 0);
    }
}

void DiagonalMatrixTests::testSize()
{
    DiagonalMatrix<double> mat(5);
    assert(mat.size() == 5);
}

void DiagonalMatrixTests::testAccessOperator()
{
    DiagonalMatrix<int> mat(2);
    mat(0) = 10;
    mat(1) = 20;

    assert(mat(0) == 10);
    assert(mat(1) == 20);

    const DiagonalMatrix<int>& constMat = mat;
    assert(constMat(0) == 10);
}

void DiagonalMatrixTests::testVectorMultiplication() 
{
    DiagonalMatrix<double> mat(3);
    mat(0) = 1.5;
    mat(1) = 2.5;
    mat(2) = 3.5;

    Vector<double> vec = { 2.0, 3.0, 4.0 };
    Vector<double> result = mat * vec;

    assert(result.size() == 3);
    assert(std::abs(result[0] - 3.0) < 1e-10);  // 1.5 * 2.0
    assert(std::abs(result[1] - 7.5) < 1e-10);  // 2.5 * 3.0
    assert(std::abs(result[2] - 14.0) < 1e-10); // 3.5 * 4.0
}

void DiagonalMatrixTests::testNorm()
{
    DiagonalMatrix<double> mat(2);
    mat(0) = 3.0;
    mat(1) = 4.0;

    double n = mat.norm();
    assert(std::abs(n - 5.0) < 1e-10);  // sqrt(3? + 4?) = 5
}

void DiagonalMatrixTests::testComplexNorm()
{
    using namespace std::complex_literals;
    DiagonalMatrix<std::complex<double>> mat(2);
    mat(0) = 3.0 + 4.0i;
    mat(1) = 6.0 + 8.0i;

    double n = mat.norm();
    assert(std::abs(n - sqrt(125)) < 1e-10);
}

void DiagonalMatrixTests::testExceptions()
{
    DiagonalMatrix<int> mat(2);
    Vector<int> vec(3);

    try
    {
        mat* vec; // Разные размеры
        assert(false); // Не должно сюда попасть
    }
    catch (const std::invalid_argument&) {}

    try 
    {
        mat(5) = 10; // Выход за границы
        assert(false);
    }
    catch (const std::out_of_range&) {}
}

template<typename T>
void DiagonalMatrixTests::printDiagonal(const DiagonalMatrix<T>& mat)
{
    for (size_t i = 0; i < mat.size(); ++i) 
    {
        std::cout << mat(i) << " ";
    }
    std::cout << "\n";
}