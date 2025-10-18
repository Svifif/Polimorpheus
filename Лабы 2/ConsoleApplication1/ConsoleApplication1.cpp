#include "LazySequenceTests.hpp" 
#include "CardinalTests.hpp"
#include <iostream>  
using namespace std;
int main() 
{
    CardinalTests::test_all();
    LazySequenceTests::test_all();

    cout << "🎉 ALL TESTS PASSED! 🎉" << endl;
    return 0;
}