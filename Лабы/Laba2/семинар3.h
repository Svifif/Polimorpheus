#pragma once

// Класс не шаблонный, можно разнести исплементацию и декларацию методов в разные файлы
class TestDeleteClass {
public:
    TestDeleteClass();
    TestDeleteClass(const TestDeleteClass& other);
    TestDeleteClass& operator=(const TestDeleteClass& other);

    ~TestDeleteClass();

    static int getCounter();
    static void resetCounter();
    static void incrementCounter();
    static void decrementCounter();

private:
    static int counter;
};
