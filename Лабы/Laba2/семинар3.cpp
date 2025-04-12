#include "TestClass.h"

// Для static полей класса необходимо в любом cpp файле выделить память, например, так
int TestDeleteClass::counter = 0;

TestDeleteClass::TestDeleteClass() {
    incrementCounter();
}

TestDeleteClass::TestDeleteClass(const TestDeleteClass& other) {
    incrementCounter();
}

TestDeleteClass& TestDeleteClass::operator=(const TestDeleteClass& other) {
    return *this;
}


TestDeleteClass::~TestDeleteClass() {
    decrementCounter();
}


int TestDeleteClass::getCounter() {
    return counter;
}

void TestDeleteClass::resetCounter() {
    counter = 0;
}

void TestDeleteClass::incrementCounter() {
    counter += 1;
}

void TestDeleteClass::decrementCounter() {
    counter -= 1;
}
