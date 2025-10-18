#pragma once
#include "CardinalTests.hpp"
using namespace std;

void CardinalTests::test_constructors() 
{
    cout << "Testing Cardinal constructors...\n";

    Cardinal c1;
    assert(c1.order == 0 && c1.index == 0);

    Cardinal c2(1, 5);
    assert(c2.order == 1 && c2.index == 5);

    Cardinal c3(c2);
    assert(c3.order == 1 && c3.index == 5);

    Cardinal c4(move(c3));
    assert(c4.order == 1 && c4.index == 5);

    cout << "Cardinal constructor tests passed!\n";
}

void CardinalTests::test_comparison_operators() 
{
    cout << "Testing Cardinal comparison operators...\n";

    Cardinal c1(0, 3);
    Cardinal c2(0, 5);
    Cardinal c3(1, 0);
    Cardinal c4(1, 2);

    // Same order comparisons
    assert(c1 < c2);
    assert(c2 > c1);
    assert(c1 == Cardinal(0, 3));
    assert(c1 != c2);

    // Different order comparisons
    assert(c2 < c3);  // (0,5) < (1,0)
    assert(c3 > c2);  // (1,0) > (0,5)
    assert(c3 < c4);  // (1,0) < (1,2)

    cout << "Cardinal comparison tests passed!\n";
}

void CardinalTests::test_arithmetic_operators() 
{
    cout << "Testing Cardinal arithmetic operators...\n";

    Cardinal c1(0, 3);
    Cardinal c2(0, 5);
    Cardinal c3(1, 2);

    Cardinal sum1 = c1 + c2;
    assert(sum1.order == 0 && sum1.index == 8);

    Cardinal c4(0, 2);
    c4 = c1;
    assert(c4.order == 0 && c4.index == 3);

    cout << "Cardinal arithmetic tests passed!\n";
}

void CardinalTests::test_all() 
{
    cout << "=== Running Cardinal Tests ===\n";

    test_constructors();
    test_comparison_operators();
    test_arithmetic_operators();

    cout << "=== All Cardinal tests passed! ===\n\n";
}