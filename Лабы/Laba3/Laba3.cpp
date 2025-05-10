#include <iostream>
#include "DequeTests.hpp"
#include "PriorityQueueTests.hpp"
#include "QueueTests.hpp"
#include "StackTests.hpp"
#include "MatrixTests.hpp"
#include "DiagonalMatrixTests.hpp"
#include "SquareMatrixTests.hpp"
#include "VectorTests.hpp"
int main()
{
	runAllQueueTests();
	runAllPriorityQueueTests();
	runAllStackTests();
	runAllDequeTests();
	DiagonalMatrixTests::runAllTests();
	MatrixTests::runAllTests();
	SquareMatrixTests::runAllTests();
	VectorTests::runAllTests();
}

