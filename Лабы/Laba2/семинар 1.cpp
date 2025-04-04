#include <iostream>

struct SomeStruct {
    int a;
    double b;
    int c;
};

class Complex {
public:
    Complex(double real, double imag) {
        this->real = real;
        this->imaginary = imag;
    }

    Complex(double real) {
        this->real = real;
        this->imaginary = 0;
    }

    ~Complex() {
        std::cout << "I am died: " << real << ", " << imaginary << std::endl;
    }

private:
    double real;
    double imaginary;
};

class Array {
public:
    Array(int size): size(size) {
        this->array = new int[size];
    }

    ~Array() {
        std::cout << "Delete array with size " << size << std::endl;
        delete[] array;
    }

    int get(int index) {
        return array[index];
    }

    void set(int index, int value) {
        array[index] = value;
    }

private:
    int *array;
    int size;
};

template<typename T>
class Container {
public:
    virtual void append(T element) = 0;
    virtual void prepend(T element) = 0;
    virtual T get(int index) = 0;
    virtual int getSize() = 0;

    std::string toString() {
        std::string res;
        res += "[";
        for (int i = 0; i < getSize(); ++i) {
            res += std::to_string(get(i));
            if (i < getSize() - 1) {
                res += ", ";
            }
        }
        res += "]";
        return res;
    }
};

// public: доступно всем, и из вне класса
// protected: доступно только в наследниках класса
// private: доступно только внутри методов класса

template<typename T>
class LinkedList: public Container<T> {
public:
    LinkedList() = default;

    LinkedList(const LinkedList& other) {
        // O(n^2), можно за O(n)
        this->size = other.size;
        for (int i = 0; i < other.size; ++i) {
            this->append(other.get(i));
        }
    }

    // Добавление в конец
    void append(T value) override {
        if (head == nullptr) {
            head = new Node;
            head->value = value;
            head->next = nullptr;
            size = 1;
            return;
        }

        Node* last = head;
        for (int i = 1; i < size; i++) {
            last = last->next;
        }
        last->next = new Node;
        last->next->value = value;
        last->next->next = nullptr;
        size++;
    }

    // Добавление в начало
    void prepend(T value) override {
        if (head == nullptr) {
            head = new Node;
            head->value = value;
            head->next = nullptr;
            size = 1;
            return;
        }

        Node *newNode = new Node;
        newNode->value = value;
        newNode->next = head;
        head = newNode;
        size++;
    }

    T get(int index) override {
        if (index < 0 || index >= size) {
            // TODO: обработать ошибку
            return 0;
        }

        Node *current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->value;
    }

    int getSize() override {
        return size;
    }

    ~LinkedList() {
        Node* next = nullptr;
        Node* now = this->head;

        while (now != nullptr) {
            next = now->next;
            delete now;
            now = next;
        }
    }

private:
    struct Node {
        T value;
        Node *next;
    };

    Node *head = nullptr;
    int size = 0;
};

template<typename T>
void printContainer(Container<T> &container) {
    std::cout << container.toString() << std::endl;
}

int main() {
    // Complex c1(1.0);
    // Complex c2(1.0, 5.642);

    // Создание динамического указателя
    // new int(value)
    // new int[size]

    // Освобождение памяти
    // delete ptr;
    // delete[] ptr;

    // Пример класса массива
    // Array arr(3);

    // arr.set(0, 5);
    // arr.set(1, 8);
    // arr.set(2, 11);

    // for (int i = 0; i < 3; i++) {
    //     std::cout << arr.get(i) << " ";
    // }
    // std::cout << std::endl;

    LinkedList<int> l1;
    LinkedList<double> l2;
    l2.append(123.654);
    l2.append(-5);
    l2.append(712736123);

    printContainer(l2);

    return 0;
}