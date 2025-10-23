#pragma once
#include "LazySequenceTests.hpp"
#include "CardinalTests.hpp"
#include "SmartPointerTests.hpp"
#include <iostream>  

using namespace std;

int main()
{
    CardinalTests::test_all();
    LazySequenceTests::test_all();
    SmartPointerTests::test_all();

    cout << " ALL TESTS PASSED! " << endl;
    return 0;
}