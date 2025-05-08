#include <iostream>
#include "DequeTests.hpp"
#include "PriorityQueueTests.hpp"
#include "QueueTests.hpp"
#include "StackTests.hpp"
int main()
{
	runAllQueueTests();
	runAllPriorityQueueTests();
	runAllStackTests();
	runAllDequeTests();
}

