#include <stdlib.h>
#include <iostream>

// Пример того, как надо выстраивать иерархию классов во 2 лабе
// 1. реализация классов DynamicArray / LinkedList
// 2. Создание абстрактного класса (интерфейса) Sequence
// 3. Реализация классов ArraySequence / LinkedListSequence, пример находится ниже

template<typename T>
class DynamicArray {
public:
    DynamicArray()
        : data(nullptr)
        , size(0) {}

    // Обычный конструктор копирования
    DynamicArray(const DynamicArray<T>& other) {
        data = new T[other.size];
        size = other.size;
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    // Конструктор копирования по мув ссылке
    DynamicArray(DynamicArray<T>&& other) {
        data = other.data;
        size = other.size;

        other.data = nullptr;
        other.size = 0;
    }

    // Обычный оператор присвоения
    DynamicArray& operator=(const DynamicArray<T>& other) {
        if (this == &other) {
            return *this;
        }
        if (data) {
            delete []data;
        }

        data = new T[other.size];
        size = other.size;
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }

        return *this;
    }

    // Оператор присвоения по мув ссылке
    DynamicArray& operator=(DynamicArray<T>&& other) {
        if (this == &other) {
            return *this;
        }
        if (data) {
            delete []data;
        }

        data = other.data;
        size = other.size;

        other.data = nullptr;
        other.size = 0;

        return *this;
    }

    ~DynamicArray() {
        delete[] data;
    }

    void Resize(size_t new_size) {
        T* new_data = new T[new_size];
        if (!data) {
            data = new_data;
            size = new_size;
            return;
        }

        for (int i = 0; i < size; i++) {
            new_data[i] = data[i];
        }

        delete[] data;
        data = new_data;
        size = new_size;
    }
    size_t GetSize() {
        return size;
    }
    void Set(const T& object, size_t index) {
        if (index >= size) {
            // Ошибка
            return;
        }
        data[index] = object;
    }
    
    void Set(T&& object, size_t index) {
        if (index >= size) {
            // Ошибка
            return;
        }
        data[index] = std::move(object);
    }

    T Get(size_t index) {
        if (index >= size) {
            // Ошибка
            // return T{};
        }
        return data[index];
    }

    // Пример реализации итератора
    class Iterator {
    public:
        Iterator(T* data, size_t index, size_t size)
            : data(data)
            , index(index)
            , size(size) {}

        // Получить текущее значение
        T& operator*() {
            return data[index];
        }

        // Перейти к следующему значению
        Iterator& operator++() {
            index++;
            return *this;
        }

        // Сравнение разных итераторов
        bool operator!=(const Iterator& other) {
            return index != other.index;
        }

    private:
        T* data;
        size_t index;
        size_t size;
    };

    // Итератор на первое значение (data[0])
    Iterator begin() {
        return Iterator(data, 0, size);
    }

    // Итератор на последнее значение + 1 (data[size])
    Iterator end() {
        return Iterator(data, size, size);
    }

private:
    T* data;
    size_t size;
};

template<typename T>
class LinkedList {
public:
    //... Реализация методов
private:
    class Node {
        T data;
        Node* next;
    };

    Node* head;
};

template<typename T>
class Sequence {
public:
    virtual void Append(T data) = 0;
    virtual void Prepend(T data) = 0;

    // Останльные методы
};

template<typename T>
class ArraySequence: public Sequence<T> {
public:
    void Append(T data) override {
        array.Resize(array.GetSize() + 1);
        array.Set(array.GetSize(), data);
    }

    void Prepend(T data) override {
        //...
    }
private:
    DynamicArray<T> array;
};

// Итераторы на обычных массивах
int main2() {
    int array[20];

    for (int i = 0; i < 20; i++) {
        array[i] = i*10;
    }

    // Можно изменять значение массива, если принимать по ссылке (int&)
    for (int& i : array) {
        std::cout << i << " ";
        i += 10;
    }
    // Та же самая запись
    // for (int index = 0; index < size(array); index++) {
    //      int& i = array[index];
    //      std::cout << i << " ";
    //      i += 10;
    // }

    std::cout << std::endl;

    // Нельзя изменять значение массива (значение копируется в i)
    for (auto i : array) {
        std::cout << i << " ";
    }

    return 0;
}

// Пример итераторов
int main3() {
    DynamicArray<double> array;
    array.Resize(5);

    for (int i = 0; i < 5; i++) {
        array.Set(i*10, i);
    }

    // Ручная работа с итераторами, можно увидеть все перегруженные операторы (operator*, operator !=, operator ++)
    // auto - на этапе компиляции тип определяется автоматически, в данном случае вместо auto будет подставляться DynamicArray::Iterator.
    // auto намного проще написать вместо того длинного типа, но злоупотреблять тоже не надо, в данном случае понятно, что это итератор, тк
    // это результат метода begin(), который обычно используется для итераторов
    // for (auto iter = array.begin(); iter != array.end(); ++iter) {
    //     std::cout << *iter << " ";
    // }

    // Та же самая запись, но более красивая
    // Вместо auto можно написать double, тк значение *iter имеет тип double
    for (auto &i : array) {
        std::cout << i << " ";
    }

    return 0;
}

// Мув семантика (дополнительно почитать где-нибудь в инете, например, тут https://habr.com/ru/articles/226229/)
int main() {
    DynamicArray< DynamicArray<int> > array;

    array.Resize(5);

    DynamicArray<int> buff;
    for (int i = 0; i < 5; i++) {
        buff.Resize(5);
        for (int j = 0; j < 5; j++) {
            buff.Set(i*10 + j, j);
        }
        // Вызов мув оператора копирования
        // DynamicArray& operator =(DynamicArray&&)
        array.Set(std::move(buff), i);
        // Вместо того, чтобы копировать значения буфера buff, выполняется подмена указателей, 
        // что гараздо дешевле (О(1) против О(n) в случае копирования)
        // После operator= буфер останется пустым и его можно использовать снова
    }

    // Вывод значений через итераторы
    for (auto &i : array) {
        for (auto &j : i) {
            std::cout << j << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
