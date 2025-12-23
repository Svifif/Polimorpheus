#ifndef TESTS_HPP
#define TESTS_HPP

#include "hashtable.hpp"
#include "task.hpp"
#include <cassert>
#include <chrono>
#include <random>
#include <set>
#include <iostream>

inline void TestBasicOperations()
{
    std::cout << "Тест 1: Базовые операции... ";

    HashTable<std::string, int> table;

    table.Add("один", 1);
    table.Add("два", 2);
    table.Add("три", 3);

    assert(table.GetCount() == 3);
    assert(table.ContainsKey("один"));
    assert(table.ContainsKey("два"));
    assert(!table.ContainsKey("четыре"));

    assert(table.Get("один") == 1);
    assert(table.Get("два") == 2);

    table.Add("один", 100);
    assert(table.Get("один") == 100);

    table.Remove("два");
    assert(table.GetCount() == 2);
    assert(!table.ContainsKey("два"));

    table.Clear();
    assert(table.GetCount() == 0);

    std::cout << "ПРОЙДЕН\n";
}

inline void TestOperatorBracket()
{
    std::cout << "Тест 2: Оператор []... ";

    HashTable<std::string, int> table;

    table["a"] = 1;
    table["b"] = 2;

    assert(table["a"] == 1);
    assert(table["b"] == 2);

    table["a"] = 100;
    assert(table["a"] == 100);

    int& value = table["c"];
    value = 3;
    assert(table["c"] == 3);

    std::cout << "ПРОЙДЕН\n";
}

inline void TestRehashing()
{
    std::cout << "Тест 3: Рехеширование... ";

    HashTable<int, int> table(4);

    for (int i = 0; i < 100; ++i)
    {
        table.Add(i, i * 10);
    }

    assert(table.GetCount() == 100);
    assert(table.GetCapacity() > 4);

    for (int i = 0; i < 100; ++i)
    {
        assert(table.ContainsKey(i));
        assert(table.Get(i) == i * 10);
    }

    std::cout << "ПРОЙДЕН\n";
}

inline void TestIterator()
{
    std::cout << "Тест 4: Итератор... ";

    HashTable<int, std::string> table;

    table.Add(1, "один");
    table.Add(2, "два");
    table.Add(3, "три");

    int count = 0;
    std::set<int> foundKeys;
    for (const auto& pair : table)
    {
        count++;
        foundKeys.insert(pair.first);
        assert(table.Get(pair.first) == pair.second);
    }

    assert(count == 3);
    assert(foundKeys.count(1) > 0);
    assert(foundKeys.count(2) > 0);
    assert(foundKeys.count(3) > 0);

    // Проверка пустой таблицы
    HashTable<int, std::string> emptyTable;
    count = 0;
    for (const auto& pair : emptyTable)
    {
        count++;
    }
    assert(count == 0);

    std::cout << "ПРОЙДЕН\n";
}

inline void TestEdgeCases()
{
    std::cout << "Тест 5: Граничные случаи... ";

    HashTable<int, int> emptyTable;
    assert(emptyTable.GetCount() == 0);
    assert(!emptyTable.ContainsKey(1));

    HashTable<int, std::string> singleTable;
    singleTable.Add(42, "ответ");
    assert(singleTable.GetCount() == 1);
    assert(singleTable.Get(42) == "ответ");

    singleTable.Remove(42);
    assert(singleTable.GetCount() == 0);
    assert(!singleTable.ContainsKey(42));

    HashTable<std::string, int> dupTable;
    dupTable.Add("ключ", 1);
    dupTable.Add("ключ", 2);
    assert(dupTable.GetCount() == 1);
    assert(dupTable.Get("ключ") == 2);

    std::cout << "ПРОЙДЕН\n";
}

inline void RunAllTests()
{
    std::cout << "=== ТЕСТИРОВАНИЕ ХЕШ-ТАБЛИЦЫ ===\n\n";

    try
    {
        TestBasicOperations();
        TestOperatorBracket();
        TestRehashing();
        TestIterator();
        TestEdgeCases();

        std::cout << "\n=== ВСЕ ТЕСТЫ ПРОЙДЕНЫ УСПЕШНО ===\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "ТЕСТ ПРОВАЛЕН: " << e.what() << "\n";
        throw;
    }
}

inline void PerformanceTest()
{
    std::cout << "\n=== ТЕСТ ПРОИЗВОДИТЕЛЬНОСТИ ===\n";

    std::vector<std::string> keys;
    std::vector<int> values;

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(1000, 9999);

    const int DATA_SIZE = 10000;
    for (int i = 0; i < DATA_SIZE; ++i)
    {
        keys.push_back("key_" + std::to_string(dist(rng)));
        values.push_back(i);
    }

    auto start = std::chrono::high_resolution_clock::now();

    HashTable<std::string, int> hashTable;
    for (int i = 0; i < DATA_SIZE; ++i)
    {
        hashTable.Add(keys[i], values[i]);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto insertTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    start = std::chrono::high_resolution_clock::now();

    int found = 0;
    for (int i = 0; i < 1000; ++i)
    {
        int idx = dist(rng) % DATA_SIZE;
        if (hashTable.ContainsKey(keys[idx]))
        {
            found++;
        }
    }

    end = std::chrono::high_resolution_clock::now();
    auto searchTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Результаты для " << DATA_SIZE << " элементов:\n";
    std::cout << "  Время вставки: " << insertTime.count() << " микросекунд\n";
    std::cout << "  Время поиска (1000 ключей): " << searchTime.count() << " микросекунд\n";
    std::cout << "  Найдено ключей: " << found << "/1000\n";

    hashTable.PrintStats();
}

inline void DemonstrateTasks()
{
    std::cout << "\n=== ДЕМОНСТРАЦИЯ РЕШЕНИЯ ЗАДАЧ ===\n";

    std::cout << "\n1. Построение гистограммы (задача И-1):\n";
    HistogramBuilder histBuilder;
    std::vector<int> ages = { 22, 25, 19, 30, 22, 25, 19, 30, 35, 40,
                             22, 19, 25, 30, 35 };
    histBuilder.BuildAgeHistogram(ages, 10);
    histBuilder.PrintHistogram();

    std::cout << "\n2. Алфавитный указатель (задача И-4):\n";

    // Используем простой английский текст для избежания проблем с кодировкой
    std::string sampleText =
        "C plus plus is a programming language. "
        "C plus plus supports object oriented programming. "
        "C plus plus is widely used for system programming. "
        "Programming in C plus plus is interesting and useful.";

    std::cout << "Текст для индексации:\n";
    std::cout << "\"" << sampleText << "\"\n\n";

    AlphabeticalIndex indexBuilder(30);
    indexBuilder.BuildIndex(sampleText);
    indexBuilder.PrintIndex();

    std::cout << "\n3. Индексирование данных (задача И-5):\n";
    PersonIndex personIndex;

    // Используем английские имена для избежания проблем с кодировкой
    Person p1("Ivan", "Ivanov", 1990);
    Person p2("Petr", "Petrov", 1985);
    Person p3("Anna", "Sidorova", 1990);
    Person p4("Maria", "Ivanova", 1995);

    personIndex.AddPerson(&p1);
    personIndex.AddPerson(&p2);
    personIndex.AddPerson(&p3);
    personIndex.AddPerson(&p4);

    personIndex.PrintAll();

    std::cout << "\nПоиск по имени 'Ivan Ivanov': ";
    Person* foundPerson = personIndex.FindByName("Ivan Ivanov");
    if (foundPerson)
    {
        foundPerson->Print();
    }
    else
    {
        std::cout << "не найден";
    }
    std::cout << "\n";

    std::cout << "Люди 1990 года рождения:\n";
    auto people1990 = personIndex.FindByBirthYear(1990);
    for (auto person : people1990)
    {
        std::cout << "  ";
        person->Print();
        std::cout << "\n";
    }
}

inline void RunAll()
{
    std::cout << "=================================================\n";
    std::cout << "ЛАБОРАТОРНАЯ РАБОТА №2: ХЕШ-ТАБЛИЦА\n";
    std::cout << "=================================================\n\n";

    std::cout << "ЧАСТЬ 1: ТЕСТИРОВАНИЕ РЕАЛИЗАЦИИ\n";
    std::cout << "---------------------------------\n";
    RunAllTests();

    std::cout << "\nЧАСТЬ 2: АНАЛИЗ ПРОИЗВОДИТЕЛЬНОСТИ\n";
    std::cout << "-----------------------------------\n";
    PerformanceTest();

    std::cout << "\nЧАСТЬ 3: РЕШЕНИЕ ЗАДАЧ \n";
    std::cout << "----------------------------\n";
    DemonstrateTasks();

    std::cout << "\n=================================================\n";
    std::cout << "ПРОГРАММА УСПЕШНО ЗАВЕРШЕНА!\n";
    std::cout << "=================================================\n";
}

#endif // TESTS_HPP