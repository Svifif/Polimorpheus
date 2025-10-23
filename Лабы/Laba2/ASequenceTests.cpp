#include "ASequenceTests.hpp"
using namespace std;

void SequenceTests::test_constructors()
{
    // Test default constructor
    MArraySequence<int> seq1;
    assert(seq1.GetLength() == 0);

    // Test array constructor
    int items[] = { 1, 2, 3 };
    MArraySequence<int> seq2(items, 3);
    assert(seq2.GetLength() == 3);
    assert(seq2[0] == 1);

    // Test invalid arguments
    try 
    {
        MArraySequence<int> seq3(nullptr, 3);
        assert(false);
    }
    catch (const std::invalid_argument&) {}

    // Test move constructor
    MArraySequence<int> seq4(items, 3);
    MArraySequence<int> seq5(std::move(seq4));
    assert(seq5.GetLength() == 3);
    assert(seq4.GetLength() == 0);
}

void SequenceTests::test_accessors() 
{
    MArraySequence<std::string> seq;
    seq.Append("first");
    seq.Append("second");
    seq.Append("third");

    assert(seq.GetLength() == 3);
    assert(seq.GetFirst() == "first");
    assert(seq.GetLast() == "third");
    assert(seq[1] == "second");
    assert(seq.Get(1) == "second");

    // Test empty sequence access
    MArraySequence<int> empty;
    try 
    {
        empty.GetFirst();
        assert(false);
    }
    catch (const std::out_of_range&) {}
}

void SequenceTests::test_mutation() 
{
    MArraySequence<int> seq;
    seq.Append(1);
    seq.Prepend(0);
    seq.Append(2);
    assert(seq.GetLength() == 3);

    // Insert test
    seq.InsertAt(10, 1);
    assert(seq[1] == 10);
    assert(seq.GetLength() == 4);

    // Subsequence test
    auto sub = seq.GetSubsequence(1, 2);
    assert(sub->GetLength() == 2);
    assert(sub->Get(0) == 10);
    delete sub;

    // Exception test
    try 
    {
        seq.InsertAt(99, 999);
        assert(false);
    }
    catch (const std::out_of_range&) {}
}

void SequenceTests::test_concat() 
{
    int items1[] = { 1, 2 };
    int items2[] = { 3, 4 };
    MArraySequence<int> seq1(items1, 2);
    MArraySequence<int> seq2(items2, 2);

    auto concat = seq1.Concat(seq2);
    assert(concat->GetLength() == 4);
    assert(concat->Get(3) == 4);

    // Self-concat test
    try
    {
        seq1.Concat(seq1);
        assert(false);
    }
    catch (const std::invalid_argument&) {}
}

void SequenceTests::test_exceptions()
{
    MArraySequence<int> seq;
    bool caught = false;

    try
    {
        seq.GetFirst();
    }
    catch (const std::out_of_range&)
    {
        caught = true;
    }
    assert(caught);

    caught = false;
    try 
    {
        seq[0];
    }
    catch (const std::out_of_range&)
    {
        caught = true;
    }
    assert(caught);
}

void SequenceTests::test_move_semantics()
{
    TrackedObject::counter = 0;
    {
        MArraySequence<TrackedObject> seq1;
        seq1.Append(TrackedObject());
        assert(TrackedObject::counter == 1);

        MArraySequence<TrackedObject> seq2(std::move(seq1));
        assert(TrackedObject::counter == 1);
        assert(seq1.GetLength() == 0);
    }
    assert(TrackedObject::counter == 0);
}

void SequenceTests::test_where_operation() 
{
    int items[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    MArraySequence<int> seq(items, 10);

    auto filtered = seq.Where([](int x) { return x % 2 == 0; });
    assert(filtered->GetLength() == 5);
    assert(filtered->Get(1) == 2);
    delete filtered;
}

void SequenceTests::test_with_pairs() 
{
    std::pair<int, std::string> items[] ={ {1, "one"}, {2, "two"}, {3, "three"} };
    MArraySequence<std::pair<int, std::string>> seq(items, 3);

    assert(seq.GetLength() == 3);
    assert(seq[0].first == 1);
    assert(seq[0].second == "one");

    seq.Append(std::make_pair(4, "four"));
    assert(seq.GetLength() == 4);
}

void SequenceTests::test_map_operation() 
{
    int items[] = { 1, 2, 3 };
    MArraySequence<int> seq(items, 3);

    auto mapped = seq.Map<double, MArraySequence<double>>([](int x) { return x * 1.5; });
    assert(mapped.GetLength() == 3);
    assert(mapped.Get(1) == 3.0);
}

void SequenceTests::test_zip_operation() 
{
    int int_items[] = { 1, 2, 3 };
    std::string str_items[] = { "one", "two", "three" };

    MArraySequence<int> int_seq(int_items, 3);
    MArraySequence<std::string> str_seq(str_items, 3);

    auto zipper = [](int num, std::string str)
        {
        return std::to_string(num) + ": " + str;
        };

    auto zipped = int_seq.Zip<std::string, std::string, MArraySequence<std::string>>(&str_seq, zipper);
    assert(zipped.GetLength() == 3);
    assert(zipped.Get(0) == "1: one");
}

void SequenceTests::test_unzip_operation() 
{
    std::pair<int, string> items[] = { {1, "one"}, {2, "two"} };
    MArraySequence<pair<int, string>> zipped(items, 2);

    auto unzipped = Sequence<int>::Unzip<int, string, MArraySequence<int>, MArraySequence<string>>(&zipped);

    assert(unzipped.first.GetLength() == 2);
    assert(unzipped.second.GetLength() == 2);
    assert(unzipped.first.Get(0) == 1);
    assert(unzipped.second.Get(0) == "one");
}

void SequenceTests::test_destructor()
{
    TrackedObject::counter = 0;
    {
        MArraySequence<TrackedObject> seq;
        seq.Append(TrackedObject());
        seq.Append(TrackedObject());
        assert(TrackedObject::counter == 2);
    }
    assert(TrackedObject::counter == 0);
}

void SequenceTests::test_all() 
{
    test_constructors();
    test_accessors();
    test_mutation();
    test_concat();
    test_exceptions();
    test_move_semantics();
    test_where_operation();
    test_map_operation();
    test_with_pairs();
    test_zip_operation();
    test_unzip_operation();
    test_destructor();

    std::cout << "All Sequence tests passed!\n";
}