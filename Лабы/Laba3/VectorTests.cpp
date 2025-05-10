#include "VectorTests.hpp"

void VectorTests::runAllTests()
{
    testDefaultConstructor();
    testSizeConstructor();
    testArrayConstructor();
    testInitializerListConstructor();
    testSizeMethod();
    testIndexOperator();
    testAddition();
    testScalarMultiplication();
    testDotProduct();
    testNorm();
    testComplexNorm();
    std::cout << "All Vector tests passed successfully!\n";
}

void VectorTests::testDefaultConstructor() 
{
    Vector<int> vec;
    assert(vec.size() == 0);
}

void VectorTests::testSizeConstructor()
{
    Vector<double> vec(5);
    assert(vec.size() == 5);
    for (size_t i = 0; i < vec.size(); ++i)
    {
        assert(vec[i] == 0.0);
    }
}

void VectorTests::testArrayConstructor()
{
    int arr[] = { 1, 2, 3, 4, 5 };
    Vector<int> vec(arr, 5);
    assert(vec.size() == 5);
    for (size_t i = 0; i < vec.size(); ++i) 
    {
        assert(vec[i] == arr[i]);
    }
}

void VectorTests::testInitializerListConstructor() 
{
    Vector<int> vec = { 1, 2, 3, 4, 5 };
    assert(vec.size() == 5);
    for (size_t i = 0; i < vec.size(); ++i) 
    {
        assert(vec[i] == static_cast<int>(i + 1));
    }
}

void VectorTests::testSizeMethod() 
{
    Vector<float> vec(10);
    assert(vec.size() == 10);
}

void VectorTests::testIndexOperator()
{
    Vector<int> vec = { 10, 20, 30 };
    assert(vec[0] == 10);
    assert(vec[1] == 20);
    assert(vec[2] == 30);

    vec[1] = 50;
    assert(vec[1] == 50);

    const Vector<int>& constVec = vec;
    assert(constVec[0] == 10);
}

void VectorTests::testAddition()
{
    Vector<int> vec1 = { 1, 2, 3 };
    Vector<int> vec2 = { 4, 5, 6 };
    Vector<int> result = vec1 + vec2;

    assert(result.size() == 3);
    assert(result[0] == 5);
    assert(result[1] == 7);
    assert(result[2] == 9);

    try 
    {
        Vector<int> vec3(5);
        vec1 + vec3;
        assert(false); 
    }
    catch (const std::invalid_argument&) 
    {
    }
}

void VectorTests::testScalarMultiplication() 
{
    Vector<double> vec = { 1.5, 2.5, 3.5 };
    Vector<double> result = vec * 2.0;

    assert(result.size() == 3);
    assert(result[0] == 3.0);
    assert(result[1] == 5.0);
    assert(result[2] == 7.0);
}

void VectorTests::testDotProduct() {
    Vector<int> vec1 = { 1, 2, 3 };
    Vector<int> vec2 = { 4, 5, 6 };
    int dot = vec1.dot(vec2);

    assert(dot == 32);

    try 
    {
        Vector<int> vec3(5);
        vec1.dot(vec3);
        assert(false); // Shouldn't reach here
    }
    catch (const std::invalid_argument&)
    {
        // Expected
    }
}

void VectorTests::testNorm()
{
    Vector<double> vec = { 3.0, 4.0 };
    double n = vec.norm();
    assert(std::abs(n - 5.0) < 1e-10);
}

void VectorTests::testComplexNorm() 
{
    using namespace std::complex_literals;
    Vector<std::complex<double>> vec = { 3.0 + 4.0i, 6.0 + 8.0i };
    double n = vec.norm();

    // Правильное ожидаемое значение
    double expected = std::sqrt(125.0); // √(5² + 10²) = √125 ≈ 11.1803
    assert(std::abs(n - expected) < 1e-10);
}

template<typename T>
void VectorTests::printVector(const Vector<T>& vec) 
{
    std::cout << "[ ";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        std::cout << vec[i] << " ";
    }
    std::cout << "]\n";
}