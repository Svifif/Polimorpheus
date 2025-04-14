#pragma once
#include "LLSequenceTests.hpp"
#include "TrackedObject.hpp"
#include <iostream>
#include <stdexcept>
#include <cassert>
#include <initializer_list>
#include <string>
#include <utility>

using namespace std;

void test_llseq_constructors()
{
    // ���� ������������ �� ���������
    MLLSequence<int> seq1;
    assert(seq1.GetLength() == 0);

    // ���� ������������ � ���������� ����� ������
    int items[] = { 1, 2, 3 };
    MLLSequence<int> seq2(items, 3);
    assert(seq2.GetLength() == 3);
    assert(seq2[0] == 1);

    // ���� � ������������ ����������� ���������
    try
    {
        MLLSequence<int> seq3(nullptr, 3);
        assert(false);
    }
    catch (const std::invalid_argument&) {}

    // ���� move-������������
    MLLSequence<int> seq4(items, 3);
    MLLSequence<int> seq5(std::move(seq4));
    assert(seq5.GetLength() == 3);
    assert(seq4.GetLength() == 0);
}

void test_llseq_accessors()
{
    MLLSequence<std::string> seq;

    // ���������� ���������
    seq.Append("first");
    seq.Append("second");
    seq.Append("third");

    // �������� ������� �������
    assert(seq.GetLength() == 3);
    assert(seq.GetFirst() == "first");
    assert(seq.GetLast() == "third");
    assert(seq[1] == "second");
    assert(seq.Get(1) == "second");
}

void test_llseq_mutation()
{
    MLLSequence<int> seq;

    // ���������� ���������
    seq.Append(1);
    seq.Prepend(0);
    seq.Append(2);
    assert(seq.GetLength() == 3);

    // �������
    seq.InsertAt(10, 1);
    assert(seq[1] == 10);
    assert(seq.GetLength() == 4);

    // ���������������������
    auto sub = seq.GetSubsequence(1, 2);
    assert(sub->GetLength() == 2);
    assert(sub->Get(0) == 10);
    delete sub;

    // �������� ����������
    bool caught = false;
    try
    {
        seq.InsertAt(99, 999);
    }
    catch (const std::out_of_range&)
    {
        caught = true;
    }
    assert(caught);
}

void test_llseq_concat()
{
    // �������� �������������������
    int items1[] = { 1, 2 };
    int items2[] = { 3, 4 };
    MLLSequence<int> seq1(items1, 2);
    MLLSequence<int> seq2(items2, 2);

    // ������������
    auto concat = seq1.Concat(seq2);
    assert(concat->GetLength() == 4);
    assert(concat->Get(3) == 4);

    // ������������ � ������ �������������������
    MLLSequence<int> emptySeq;
    auto concat2 = seq1.Concat(emptySeq);
    assert(concat2->GetLength() == 4);
}

void test_llseq_exceptions()
{
    MLLSequence<int> seq;
    bool caught = false;

    // �������� ���������� ��� ������ ������������������
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
        seq.GetLast();
    }
    catch (const std::out_of_range&)
    {
        caught = true;
    }
    assert(caught);
}

void test_llseq_move_semantics()
{
    // ���� � ���������������� �����
    MLLSequence<TrackedObject> seq1;
    seq1.Append(TrackedObject());

    assert(TrackedObject::counter == 1);
    MLLSequence<TrackedObject> seq2(std::move(seq1));
    assert(TrackedObject::counter == 1);
    assert(seq1.GetLength() == 0);
}

void test_llseq_where_operation()
{
    // �������� � ���������� ������������������
    int items[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    MLLSequence<int> seq(items, 10);

    // ����������
    auto filtered = seq.Where([](int x) { return x % 2 == 0; });
    assert(filtered->GetLength() == 5);
    assert(filtered->Get(1) == 2);
    delete filtered;
}

void test_llseq_with_pairs()
{
    // ������� ������ ��� ��� �������������
    std::pair<int, std::string> items[] = {
        {1, "one"},
        {2, "two"},
        {3, "three"}
    };

    // ���������� ����������� � ��������
    MLLSequence<pair<int, string>> seq(items, 3);

    assert(seq.GetLength() == 3);
    assert(seq[0].first == 1);
    assert(seq[0].second == "one");

    // ��������� ���������� ����� ���������
    seq.Append(pair<int, string>(4, "four"));
    assert(seq.GetLength() == 4);
}

void test_llseq_map_operation()
{
    int items[] = { 1, 2, 3 };
    MLLSequence<int> seq(items, 3);

    auto mapped = seq.Map<double, MLLSequence<double>>([](int x) { return x * 1.5; });
    assert(mapped.GetLength() == 3);
    assert(mapped.Get(1) == 3.0);
}

// ��������� ������� zipper ��������� (��� �������)
std::string zipper_func(int num, const std::string& str)
{
    return std::to_string(num) + ": " + str;
}

// ��������� ������� � ������ ������������� �����
std::string zipper_func(int num, std::string str) // �������� �� �������� �� ��������
{
    return std::to_string(num) + ": " + str;
}

void test_llseq_zip_operation()
{
    // ���������� ������
    int int_items[] = { 1, 2, 3 };
    std::string str_items[] = { "one", "two", "three" };

    MLLSequence<int> int_seq(int_items, 3);
    MLLSequence<std::string> str_seq(str_items, 3);

    // ��������� �� ������� � ���������� ����������
    std::string(*zipper)(int, std::string) = &zipper_func; // ��� const&

    // ����� ������ � ����� �����������
    auto zipped = int_seq.Sequence<int>::Zip<std::string, std::string, MLLSequence<string>>(
        static_cast<Sequence<std::string>*>(&str_seq),
        zipper
    );

    // ��������
    assert(zipped.GetLength() == 3);
    assert(zipped.Get(0) == "1: one");
}

void test_llseq_unzip_operation()
{
    // ������� ������������������ ���
    std::pair<int, std::string> items[] = {
        {1, "one"},
        {2, "two"}
    };
    MLLSequence<std::pair<int, std::string>> zipped(items, 2);

    // ��������� Unzip
    auto unzipped = Sequence<int>::Unzip<int, string, MLLSequence<int>, MLLSequence<string>>(&zipped);

    // �������� ���� �������������������
    MLLSequence<int> nums = unzipped.first;
    MLLSequence<string> strs = unzipped.second;

    // ��������� ����������
    assert(nums.GetLength() == 2);
    assert(strs.GetLength() == 2);
    assert(nums.Get(0) == 1);
    assert(strs.Get(0) == "one");

}

void run_llsequence_tests()
{
    test_llseq_constructors();
    test_llseq_accessors();
    test_llseq_mutation();
    test_llseq_concat();
    test_llseq_exceptions();
    test_llseq_move_semantics();
    test_llseq_where_operation();
    test_llseq_map_operation();
    test_llseq_with_pairs();
    test_llseq_unzip_operation();
    test_llseq_zip_operation();

    std::cout << "All LLSequence tests passed!\n";
}