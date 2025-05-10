#include "SquareMatrixTests.hpp"

void SquareMatrixTests::runAllTests() 
{
    testConstructor();
    testSwapRows();
    testAddRows();
    testMultiplyRow();
    testExceptions();
    std::cout << "All SquareMatrix tests passed successfully!\n";
}

void SquareMatrixTests::testConstructor()
{
    SquareMatrix<int> mat(3);
    assert(mat.getRows() == 3);
    assert(mat.getCols() == 3);

    for (size_t i = 0; i < mat.getRows(); ++i) 
    {
        for (size_t j = 0; j < mat.getCols(); ++j) 
        {
            assert(mat[i][j] == 0);
        }
    }
}

void SquareMatrixTests::testSwapRows() 
{
    SquareMatrix<int> mat(2);
    mat[0][0] = 1; mat[0][1] = 2;
    mat[1][0] = 3; mat[1][1] = 4;

    mat.swapRows(0, 1);

    assert(mat[0][0] == 3);
    assert(mat[0][1] == 4);
    assert(mat[1][0] == 1);
    assert(mat[1][1] == 2);
}

void SquareMatrixTests::testAddRows()
{
    SquareMatrix<double> mat(3);
    mat[0][0] = 1.0; mat[0][1] = 2.0; mat[0][2] = 3.0;
    mat[1][0] = 4.0; mat[1][1] = 5.0; mat[1][2] = 6.0;
    mat[2][0] = 7.0; mat[2][1] = 8.0; mat[2][2] = 9.0;

    // Добавляем строку 0 к строке 1 с коэффициентом 2
    mat.addRows(0, 1, 2.0);

    assert(mat[1][0] == 6.0);  // 4 + 1*2
    assert(mat[1][1] == 9.0);  // 5 + 2*2
    assert(mat[1][2] == 12.0); // 6 + 3*2

    // Проверяем, что другие строки не изменились
    assert(mat[0][0] == 1.0);
    assert(mat[2][2] == 9.0);
}

void SquareMatrixTests::testMultiplyRow() 
{
    SquareMatrix<std::complex<double>> mat(2);
    using namespace std::complex_literals;
    mat[0][0] = 1.0 + 2.0i; mat[0][1] = 3.0 + 4.0i;
    mat[1][0] = 5.0 + 6.0i; mat[1][1] = 7.0 + 8.0i;

    // Умножаем строку 0 на комплексное число
    mat.multiplyRow(0, 2.0 + 1.0i);

    // Проверяем результат умножения
    assert(std::abs(mat[0][0].real() - 0.0) < 1e-10);  // (1+2i)*(2+i) = 0 + 5i
    assert(std::abs(mat[0][0].imag() - 5.0) < 1e-10);
    assert(std::abs(mat[0][1].real() - 2.0) < 1e-10);  // (3+4i)*(2+i) = 2 + 11i
    assert(std::abs(mat[0][1].imag() - 11.0) < 1e-10);

    // Проверяем, что другая строка не изменилась
    assert(mat[1][0] == (5.0 + 6.0i));
}

void SquareMatrixTests::testExceptions()
{
    SquareMatrix<int> mat(2);

    try 
    {
        mat.swapRows(0, 2); // Выход за границы
        assert(false); // Не должно сюда попасть
    }
    catch (const std::out_of_range&) {}

    try 
    {
        mat.addRows(0, 3); // Выход за границы
        assert(false);
    }
    catch (const std::out_of_range&) {}

    try 
    {
        mat.multiplyRow(5, 2); // Выход за границы
        assert(false);
    }
    catch (const std::out_of_range&) {}
}

template<typename T>
void SquareMatrixTests::printMatrix(const SquareMatrix<T>& mat) 
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