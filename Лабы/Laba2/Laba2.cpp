
#include <iostream>
#include  "DynamicArrayTests.hpp"
#include "TLinkedList.hpp"
#include "LLSequenceTests.hpp"
#include "ASequenceTests.hpp"
int main()
{
    run_dynamic_array_tests();
    run_linked_list_tests();
    run_llsequence_tests();
    SequenceTests::test_all();
    return 0;
}

