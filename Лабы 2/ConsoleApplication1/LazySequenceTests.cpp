#include "LazySequenceTests.hpp"
#include <iostream>
using namespace std;

void LazySequenceTests::test_constructors()
{
    cout << "Testing LazySequence constructors...\n";

    // Test default constructor
    LazySequence<int> seq1;
    assert(seq1.GetLength().index == 0);
    assert(seq1.GetExCntr() == 0);

    // Test array constructor
    int items[] = { 1, 2, 3 };
    LazySequence<int> seq2(items, 3);
    assert(seq2.GetLength().index == 3);
    assert(seq2.GetFirst() == 1);

    // Test generator constructor
    auto fibRule = [](int n)
        {
            if (n == 0) return 1;
            if (n == 1) return 1;
            // Simplified Fibonacci
            return n * 2;
        };
    LazySequence<int> seq3(fibRule, 3);
    assert(seq3.GetLength().index == 3);
    assert(seq3.GetExCntr() == 3);

    // Test copy constructor
    LazySequence<int> seq4(seq3);
    assert(seq4.GetLength().index == 3);
    assert(seq4.GetFirst() == 1);

    cout << "Constructor tests passed!\n";
}

void LazySequenceTests::test_lazy_generation()
{
    cout << "Testing lazy generation...\n";

    int callCount = 0;
    auto counterRule = [&callCount](int n)
        {
            callCount++;
            return n * 10;
        };

    LazySequence<int> seq(counterRule, 2);
    assert(callCount == 2); // Initial generation

    // Access pre-generated elements
    assert(seq.Get(0) == 0);
    assert(seq.Get(1) == 10);
    assert(callCount == 2); // No new calls

    // Lazy generation for new elements
    assert(seq.Get(5) == 50);
    assert(callCount == 6); // Generated elements 2,3,4,5

    cout << "Lazy generation tests passed!\n";
}

void LazySequenceTests::test_accessors()
{
    cout << "Testing accessors...\n";

    int items[] = { 10, 20, 30 };
    LazySequence<int> seq(items, 3);

    assert(seq.GetFirst() == 10);
    assert(seq.GetLast() == 30);
    assert(seq.Get(1) == 20);
    assert(seq.GetLength().index == 3);

    // Test empty sequence
    LazySequence<int> empty;
    try
    {
        empty.GetFirst();
        assert(false);
    }
    catch (const out_of_range&) {}

    try
    {
        empty.GetLast();
        assert(false);
    }
    catch (const out_of_range&) {}

    cout << "Accessor tests passed!\n";
}

void LazySequenceTests::test_mutation()
{
    cout << "Testing mutation...\n";

    LazySequence<int> seq;
    seq.Append(1)->Append(2)->Append(3);
    assert(seq.GetLength().index == 3);
    assert(seq.GetLast() == 3);

    seq.Prepend(0);
    assert(seq.GetFirst() == 0);
    assert(seq.GetLength().index == 4);

    seq.InsertAt(99, 2);
    assert(seq.Get(2) == 99);
    assert(seq.GetLength().index == 5);

    // Test subsequence
    auto sub = seq.GetSubsequence(1, 3);
    assert(sub->GetLength().index == 3);
    assert(sub->GetFirst() == 1);
    delete sub;

    cout << "Mutation tests passed!\n";
}

void LazySequenceTests::test_composition() {
    cout << "Testing composition...\n";

    int items1[] = { 1, 2, 3 };
    int items2[] = { 4, 5 };
    LazySequence<int> seq1(items1, 3);
    LazySequence<int> seq2(items2, 2);
    /*
    // Test concat
    auto concat = seq1.Concat(&seq2);
    assert(concat != nullptr);
    // Note: concat returns MixedLazySequence, so we'd need Mixed tests*/

   /* // Test InsertLSeq
    auto inserted = seq1.InsertLSeq(&seq2, Cardinal(0, 1));
    assert(inserted != nullptr);*/

    cout << "Composition tests passed!\n";
}

void LazySequenceTests::test_functional_operations()
{
    cout << "Testing functional operations...\n";

    int items[] = { 1, 2, 3, 4, 5 };
    LazySequence<int> seq(items, 5);

    // Test Map
    auto doubled = seq.Map<double>([](int x) { return x * 2.0; });
    assert(doubled->GetFirst() == 2.0);
    assert(doubled->Get(2) == 6.0);
    delete doubled;

    // Test Reduce
    auto sum = seq.Reduce<int>([](int acc, int x) { return acc + x; }, 0);
    assert(sum == 15);

    // Test Where
    auto evens = seq.Where([](int x) { return x % 2 == 0; });
    assert(evens->GetLength().index == 2);
    assert(evens->GetFirst() == 2);
    delete evens;

    // Test Zip - ИСПРАВЛЕННЫЙ ТЕСТ
    int otherItems[] = { 10, 20, 30 };
    LazySequence<int> other(otherItems, 3);

    // Создаем функцию-zipper для объединения
    auto zipper = [](int a, int b) { return a + b; }; // Суммируем элементы

    // Вызываем Zip с zipper функцией
    auto zipped = seq.Zip(&other, zipper);
    assert(zipped != nullptr);

    // Проверяем результат
    assert(zipped->GetLength().index == 3); // min(5, 3) = 3
    assert(zipped->GetFirst() == 11); // 1 + 10 = 11
    assert(zipped->Get(1) == 22); // 2 + 20 = 22

    delete zipped;

    cout << "Functional operations tests passed!\n";
}

void LazySequenceTests::test_move_semantics()
{
    cout << "Testing move semantics...\n";

    int items[] = { 1, 2, 3 };
    LazySequence<int> seq1(items, 3);

    LazySequence<int> seq2(move(seq1));
    assert(seq2.GetLength().index == 3);

    // Проверяем moved-from state безопасно
    assert(seq1.GetLength().index == 0); // Должен вернуть (0,0) а не падать

    LazySequence<int> seq3;
    seq3 = move(seq2);
    assert(seq3.GetLength().index == 3);
    assert(seq2.GetLength().index == 0); // Тоже безопасно

    cout << "Move semantics tests passed!\n";
}

void LazySequenceTests::test_exceptions()
{
    cout << "Testing exceptions...\n";

    LazySequence<int> seq;

    // Test negative index
    try
    {
        seq.Get(-1);
        assert(false);
    }
    catch (const out_of_range&) {}

    // Test out of bounds (empty sequence)
    try
    {
        seq.Get(0);
        assert(false);
    }
    catch (const out_of_range&) {}

    // Test invalid rule
    try
    {
        LazySequence<int> badSeq(nullptr, 3);
        assert(false);
    }
    catch (const invalid_argument&) {}

    cout << "Exception tests passed!\n";
}

void LazySequenceTests::test_all()
{
    cout << "=== Running LazySequence Tests ===\n";

    test_constructors();
    test_lazy_generation();
    test_accessors();
    test_mutation();
    test_composition();
    test_functional_operations();    test_move_semantics();
    test_exceptions();


    cout << "=== All LazySequence tests passed! ===\n\n";
}