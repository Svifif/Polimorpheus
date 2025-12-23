#ifndef TASKS_HPP
#define TASKS_HPP

#include "hashtable.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <algorithm>

// ============================================================================
// БЕЗОПАСНЫЕ ФУНКЦИИ ДЛЯ РАБОТЫ С СИМВОЛАМИ
// ============================================================================

// Безопасная проверка пунктуации
inline bool is_punctuation_safe(char c) {
    unsigned char uc = static_cast<unsigned char>(c);
    return std::ispunct(uc);
}

// Безопасное приведение к нижнему регистру
inline char to_lower_safe(char c) {
    unsigned char uc = static_cast<unsigned char>(c);
    return static_cast<char>(std::tolower(uc));
}

// Безопасное приведение к верхнему регистру
inline char to_upper_safe(char c) {
    unsigned char uc = static_cast<unsigned char>(c);
    return static_cast<char>(std::toupper(uc));
}

// ============================================================================
// ПОСТРОЕНИЕ ГИСТОГРАММЫ (ЗАДАЧА И-1)
// ============================================================================

class HistogramBuilder
{
private:
    HashTable<std::string, int> histogram;

public:
    void BuildAgeHistogram(const std::vector<int>& ages, int binSize = 10)
    {
        histogram.Clear();

        for (int age : ages)
        {
            int binStart = (age / binSize) * binSize;
            std::string range = std::to_string(binStart) + "-" +
                std::to_string(binStart + binSize - 1);

            if (histogram.ContainsKey(range))
            {
                histogram[range] = histogram[range] + 1;
            }
            else
            {
                histogram.Add(range, 1);
            }
        }
    }

    void PrintHistogram() const
    {
        std::cout << "\n=== ГИСТОГРАММА ===\n";
        for (const auto& pair : histogram)
        {
            std::cout << "  " << pair.first << ": "
                << pair.second << " элементов\n";
        }
    }
};

// ============================================================================
// АЛФАВИТНЫЙ УКАЗАТЕЛЬ (ЗАДАЧА И-4)
// ============================================================================

class AlphabeticalIndex
{
private:
    HashTable<std::string, std::vector<int>> index;
    int pageSize;

    int CalculatePageNumber(int charPosition) const
    {
        return (charPosition / pageSize) + 1;
    }

    // Безопасная обработка слова
    std::string ProcessWord(const std::string& word) const
    {
        std::string result;

        for (char c : word)
        {
            // Пропускаем пунктуацию
            if (!is_punctuation_safe(c))
            {
                // Приводим к нижнему регистру
                result += to_lower_safe(c);
            }
        }

        return result;
    }

public:
    AlphabeticalIndex(int charPageSize = 1000) : pageSize(charPageSize) {}

    void BuildIndex(const std::string& text)
    {
        index.Clear();
        std::istringstream iss(text);
        std::string word;
        int charCount = 0;

        while (iss >> word)
        {
            // Обрабатываем слово (убираем пунктуацию, приводим к нижнему регистру)
            std::string cleaned_word = ProcessWord(word);

            if (cleaned_word.empty()) continue;

            int pageNum = CalculatePageNumber(charCount);

            try {
                if (index.ContainsKey(cleaned_word))
                {
                    auto& pages = index[cleaned_word];
                    if (pages.empty() || pages.back() != pageNum)
                    {
                        pages.push_back(pageNum);
                    }
                }
                else
                {
                    index.Add(cleaned_word, { pageNum });
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Ошибка при обработке слова '" << cleaned_word << "': " << e.what() << "\n";
            }

            charCount += static_cast<int>(word.length()) + 1; // +1 для пробела
        }

        std::cout << "Построено " << index.GetCount() << " записей в указателе.\n";
    }

    void PrintIndex() const
    {
        std::cout << "\n=== АЛФАВИТНЫЙ УКАЗАТЕЛЬ ===\n";
        std::cout << "Размер страницы: " << pageSize << " символов\n\n";

        try {
            auto keys = index.GetKeys();
            std::sort(keys.begin(), keys.end());

            for (const auto& word : keys)
            {
                std::cout << word << ": страницы ";
                try {
                    auto pages = index.Get(word);
                    for (size_t i = 0; i < pages.size(); ++i)
                    {
                        std::cout << pages[i];
                        if (i < pages.size() - 1) std::cout << ", ";
                    }
                    std::cout << "\n";
                }
                catch (const std::exception& e) {
                    std::cout << "ошибка получения страниц: " << e.what() << "\n";
                }
            }
        }
        catch (const std::exception& e) {
            std::cout << "Ошибка при печати указателя: " << e.what() << "\n";
        }
    }
};

// ============================================================================
// ИНДЕКСИРОВАНИЕ ДАННЫХ (ЗАДАЧА И-5)
// ============================================================================

class Person
{
private:
    std::string firstName;
    std::string lastName;
    int birthYear;

public:
    Person(const std::string& fn, const std::string& ln, int year)
        : firstName(fn), lastName(ln), birthYear(year) {
    }

    std::string GetFullName() const { return firstName + " " + lastName; }
    std::string GetFirstName() const { return firstName; }
    std::string GetLastName() const { return lastName; }
    int GetBirthYear() const { return birthYear; }

    void Print() const
    {
        std::cout << GetFullName() << " (родился в " << birthYear << ")";
    }
};

class PersonIndex
{
private:
    HashTable<std::string, Person*> byName;
    HashTable<int, std::vector<Person*>> byYear;

public:
    void AddPerson(Person* person)
    {
        try {
            byName.Add(person->GetFullName(), person);

            if (byYear.ContainsKey(person->GetBirthYear()))
            {
                byYear[person->GetBirthYear()].push_back(person);
            }
            else
            {
                byYear.Add(person->GetBirthYear(), { person });
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка при добавлении персоны: " << e.what() << "\n";
        }
    }

    Person* FindByName(const std::string& name)
    {
        try {
            if (byName.ContainsKey(name))
            {
                return byName.Get(name);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка при поиске по имени: " << e.what() << "\n";
        }
        return nullptr;
    }

    std::vector<Person*> FindByBirthYear(int year)
    {
        try {
            if (byYear.ContainsKey(year))
            {
                return byYear.Get(year);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка при поиске по году рождения: " << e.what() << "\n";
        }
        return {};
    }

    void PrintAll()
    {
        std::cout << "\n=== ЛЮДИ В ИНДЕКСЕ ===\n";
        try {
            for (const auto& pair : byName)
            {
                std::cout << "  ";
                pair.second->Print();
                std::cout << "\n";
            }
        }
        catch (const std::exception& e) 
        {
            std::cout << "Ошибка при печати индекса: " << e.what() << "\n";
        }
    }
};

#endif // TASKS_HPP