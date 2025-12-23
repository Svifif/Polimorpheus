#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <iostream>
#include <vector>
#include <functional>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <memory>


// ИНТЕРФЕЙС СЛОВАРЯ
/*
 * Интерфейс IDictionary - абстрактный базовый класс для всех словарей
 * Определяет основные операции, которые должен поддерживать словарь
 */
template <typename TKey, typename TValue>
class IDictionary
{
public:
    // Виртуальный деструктор для корректного удаления производных классов
    virtual ~IDictionary() = default;

    // Получить количество элементов в словаре
    virtual int GetCount() const = 0;

    // Получить емкость (размер внутреннего хранилища)
    virtual int GetCapacity() const = 0;

    // Получить значение по ключу (с проверкой существования)
    virtual TValue Get(const TKey& key) const = 0;

    // Проверить наличие ключа в словаре
    virtual bool ContainsKey(const TKey& key) const = 0;

    // Добавить пару ключ-значение (или обновить значение, если ключ существует)
    virtual void Add(const TKey& key, const TValue& value) = 0;

    // Удалить элемент по ключу
    virtual void Remove(const TKey& key) = 0;

    // Очистить словарь (удалить все элементы)
    virtual void Clear() = 0;

    // Вывести содержимое словаря на экран
    virtual void Print() const = 0;
};

// ХЕШ-ТАБЛИЦА С МЕТОДОМ ЦЕПОЧЕК 
/*
 * HashTable - реализация хеш-таблицы с разрешением коллизий методом цепочек
 * Наследует интерфейс IDictionary для совместимости
 */
template <typename TKey, typename TValue>
class HashTable : public IDictionary<TKey, TValue>
{
private:
    // Структура узла цепочки (связный список)
    struct Node
    {
        TKey key;            // Ключ элемента
        TValue value;        // Значение элемента
        Node* next;          // Указатель на следующий узел в цепочке

        // Конструктор узла
        Node(const TKey& k, const TValue& v) : key(k), value(v), next(nullptr) {}
    };

    // Вектор указателей на корзины (buckets) - основной массив хеш-таблицы
    std::vector<Node*> buckets;

    int count;                      // Текущее количество элементов
    int capacity;                   // Текущая емкость (размер массива buckets)

    // Параметры для динамического изменения размера
    double loadFactorThreshold;     // Порог для увеличения размера (по умолчанию 0.75)
    double shrinkThreshold;         // Порог для уменьшения размера (по умолчанию 0.25)
    double expansionFactor;         // Коэффициент увеличения/уменьшения (по умолчанию 2.0)

    // Хеш-функция для преобразования ключа в индекс
    std::function<size_t(const TKey&)> hashFunction;

    // ПРИВАТНЫЕ МЕТОДЫ 

    /*
     * Вычисляет индекс корзины для заданного ключа
     * Использует хеш-функцию и операцию модуля
     */
    size_t GetBucketIndex(const TKey& key) const
    {
        return hashFunction(key) % capacity;
    }

    /*
     * Вычисляет текущий коэффициент загрузки таблицы
     * Коэффициент загрузки = количество элементов / емкость
     */
    double GetLoadFactor() const
    {
        return static_cast<double>(count) / capacity;
    }

    /*
     * Рехеширование - перераспределение элементов при изменении размера таблицы
     * Создает новый массив корзин, пересчитывает индексы всех элементов
     * и перемещает их в новые позиции
     */
    void Rehash(int newCapacity)
    {
        if (newCapacity <= 0) return;

        // Создаем новый массив корзин нужного размера
        std::vector<Node*> newBuckets(newCapacity, nullptr);

        // Проходим по всем элементам старой таблицы
        for (int i = 0; i < capacity; ++i)
        {
            Node* node = buckets[i];
            while (node)
            {
                // Сохраняем указатель на следующий элемент
                Node* next = node->next;

                // Вычисляем новый индекс для элемента
                size_t newIndex = hashFunction(node->key) % newCapacity;

                // Вставляем элемент в начало цепочки новой корзины
                node->next = newBuckets[newIndex];
                newBuckets[newIndex] = node;

                // Переходим к следующему элементу
                node = next;
            }
        }

        // Заменяем старый массив корзин новым
        buckets = std::move(newBuckets);
        capacity = newCapacity;
    }

public:
    // КОНСТРУКТОРЫ 

    /*
     * Конструктор по умолчанию
     * Создает хеш-таблицу с начальной емкостью 16
     * Использует стандартную хеш-функцию std::hash
     */
    HashTable(int initialCapacity = 16)
        : count(0), capacity(initialCapacity),
        loadFactorThreshold(0.75),     // Таблица увеличивается при заполнении на 75%
        shrinkThreshold(0.25),         // Таблица уменьшается при заполнении менее 25%
        expansionFactor(2.0)           // Размер увеличивается/уменьшается в 2 раза
    {
        if (capacity <= 0) capacity = 16;  // Минимальная емкость 16

        // Инициализируем все корзины как пустые
        buckets.resize(capacity, nullptr);

        // Используем стандартную хеш-функцию C++
        hashFunction = [](const TKey& key)
            {
                return std::hash<TKey>{}(key);
            };
    }

    /*
     * Конструктор с пользовательской хеш-функцией
     * Позволяет использовать специальные хеш-функции для определенных типов
     */
    HashTable(std::function<size_t(const TKey&)> customHashFunc, int initialCapacity = 16)
        : count(0), capacity(initialCapacity),
        loadFactorThreshold(0.75),
        shrinkThreshold(0.25),
        expansionFactor(2.0),
        hashFunction(customHashFunc)   // Используем переданную хеш-функцию
    {
        if (capacity <= 0) capacity = 16;
        buckets.resize(capacity, nullptr);
    }

    /*
     * Конструктор копирования
     * Создает глубокую копию другой хеш-таблицы
     */
    HashTable(const HashTable& other)
        : count(other.count),
        capacity(other.capacity),
        loadFactorThreshold(other.loadFactorThreshold),
        shrinkThreshold(other.shrinkThreshold),
        expansionFactor(other.expansionFactor),
        hashFunction(other.hashFunction)
    {
        // Выделяем память под новый массив корзин
        buckets.resize(capacity, nullptr);

        // Копируем все элементы из другой таблицы
        for (int i = 0; i < capacity; ++i)
        {
            Node* otherNode = other.buckets[i];
            Node* lastNode = nullptr;  // Для связи узлов в новой цепочке

            while (otherNode)
            {
                // Создаем новый узел с такими же данными
                Node* newNode = new Node(otherNode->key, otherNode->value);

                if (!lastNode)
                {
                    // Первый узел в цепочке
                    buckets[i] = newNode;
                }
                else
                {
                    // Присоединяем узел к концу цепочки
                    lastNode->next = newNode;
                }

                // Переходим к следующему узлу для копирования
                lastNode = newNode;
                otherNode = otherNode->next;
            }
        }
    }

    /*
     * Конструктор перемещения
     * "Перехватывает" ресурсы другой таблицы без копирования
     */
    HashTable(HashTable&& other) noexcept
        : buckets(std::move(other.buckets)),  // Перемещаем вектор корзин
        count(other.count),
        capacity(other.capacity),
        loadFactorThreshold(other.loadFactorThreshold),
        shrinkThreshold(other.shrinkThreshold),
        expansionFactor(other.expansionFactor),
        hashFunction(std::move(other.hashFunction))  // Перемещаем хеш-функцию
    {
        // Обнуляем указатели в исходной таблице
        other.buckets.clear();
        other.count = 0;
        other.capacity = 0;
    }

    /*
     * Оператор присваивания копированием
     */
    HashTable& operator=(const HashTable& other)
    {
        if (this != &other)  // Защита от самоприсваивания
        {
            // Очищаем текущие данные
            Clear();

            // Копируем параметры
            capacity = other.capacity;
            count = other.count;
            loadFactorThreshold = other.loadFactorThreshold;
            shrinkThreshold = other.shrinkThreshold;
            expansionFactor = other.expansionFactor;
            hashFunction = other.hashFunction;

            // Выделяем память под копию
            buckets.resize(capacity, nullptr);

            // Копируем элементы (аналогично конструктору копирования)
            for (int i = 0; i < capacity; ++i)
            {
                Node* otherNode = other.buckets[i];
                Node* lastNode = nullptr;

                while (otherNode)
                {
                    Node* newNode = new Node(otherNode->key, otherNode->value);

                    if (!lastNode)
                    {
                        buckets[i] = newNode;
                    }
                    else
                    {
                        lastNode->next = newNode;
                    }

                    lastNode = newNode;
                    otherNode = otherNode->next;
                }
            }
        }
        return *this;
    }

    /*
     * Оператор присваивания перемещением
     */
    HashTable& operator=(HashTable&& other) noexcept
    {
        if (this != &other)  // Защита от самоприсваивания
        {
            // Очищаем текущие данные
            Clear();

            // Перемещаем ресурсы из другой таблицы
            buckets = std::move(other.buckets);
            count = other.count;
            capacity = other.capacity;
            loadFactorThreshold = other.loadFactorThreshold;
            shrinkThreshold = other.shrinkThreshold;
            expansionFactor = other.expansionFactor;
            hashFunction = std::move(other.hashFunction);

            // Обнуляем исходную таблицу
            other.buckets.clear();
            other.count = 0;
            other.capacity = 0;
        }
        return *this;
    }

    /*
     * Деструктор - освобождает всю выделенную память
     */
    ~HashTable()
    {
        Clear();
    }

    // ==================== IDICTIONARY ИНТЕРФЕЙС ====================

    /*
     * Возвращает текущее количество элементов в таблице
     */
    int GetCount() const override
    {
        return count;
    }

    /*
     * Возвращает текущую емкость таблицы (размер массива корзин)
     */
    int GetCapacity() const override
    {
        return capacity;
    }

    /*
     * Получает значение по ключу
     * Ищет ключ в соответствующей цепочке
     * Если ключ не найден, выбрасывает исключение
     */
    TValue Get(const TKey& key) const override
    {
        // Вычисляем индекс корзины для ключа
        size_t index = GetBucketIndex(key);
        Node* node = buckets[index];

        // Линейный поиск по цепочке
        while (node)
        {
            if (node->key == key)  // Ключ найден
            {
                return node->value;
            }
            node = node->next;
        }

        // Ключ не найден
        throw std::out_of_range("Ключ не найден в хеш-таблице");
    }

    /*
     * Проверяет наличие ключа в таблице
     * Возвращает true если ключ найден, false - в противном случае
     */
    bool ContainsKey(const TKey& key) const override
    {
        size_t index = GetBucketIndex(key);
        Node* node = buckets[index];

        while (node)
        {
            if (node->key == key)
            {
                return true;
            }
            node = node->next;
        }

        return false;
    }

    /*
     * Добавляет пару ключ-значение в таблицу
     * Если ключ уже существует, обновляет его значение
     * Автоматически выполняет рехеширование при необходимости
     */
    void Add(const TKey& key, const TValue& value) override
    {
        // Проверяем коэффициент загрузки
        if (GetLoadFactor() >= loadFactorThreshold)
        {
            // Увеличиваем размер таблицы в expansionFactor раз
            Rehash(static_cast<int>(capacity * expansionFactor));
        }

        size_t index = GetBucketIndex(key);
        Node* node = buckets[index];

        // Проверяем, не существует ли уже такой ключ
        while (node)
        {
            if (node->key == key)
            {
                // Ключ найден - обновляем значение
                node->value = value;
                return;
            }
            node = node->next;
        }

        // Ключ не найден - создаем новый узел
        Node* newNode = new Node(key, value);

        // Вставляем новый узел в начало цепочки
        newNode->next = buckets[index];
        buckets[index] = newNode;
        count++;  // Увеличиваем счетчик элементов
    }

    /*
     * Удаляет элемент по ключу
     * Ищет ключ в цепочке и удаляет соответствующий узел
     * Автоматически уменьшает размер таблицы при необходимости
     */
    void Remove(const TKey& key) override
    {
        size_t index = GetBucketIndex(key);
        Node* node = buckets[index];
        Node* prev = nullptr;  // Для удаления из середины цепочки

        while (node)
        {
            if (node->key == key)
            {
                // Нашли узел для удаления
                if (prev)
                {
                    // Удаляем из середины/конца цепочки
                    prev->next = node->next;
                }
                else
                {
                    // Удаляем первый узел цепочки
                    buckets[index] = node->next;
                }

                // Освобождаем память
                delete node;
                count--;  // Уменьшаем счетчик элементов

                // Проверяем, нужно ли уменьшить размер таблицы
                if (GetLoadFactor() <= shrinkThreshold && capacity > 16)
                {
                    // Уменьшаем размер таблицы в expansionFactor раз
                    Rehash(std::max(16, static_cast<int>(capacity / expansionFactor)));
                }

                return;
            }
            prev = node;
            node = node->next;
        }

        // Ключ не найден
        throw std::out_of_range("Ключ не найден для удаления");
    }

    /*
     * Очищает таблицу - удаляет все элементы
     * Освобождает память всех узлов
     */
    void Clear() override
    {
        for (int i = 0; i < capacity; ++i)
        {
            Node* node = buckets[i];
            while (node)
            {
                Node* next = node->next;  // Сохраняем указатель на следующий
                delete node;              // Удаляем текущий узел
                node = next;              // Переходим к следующему
            }
            buckets[i] = nullptr;  // Обнуляем корзину
        }
        count = 0;  // Сбрасываем счетчик
    }

    /*
     * Выводит содержимое таблицы на экран
     * Показывает структуру цепочек
     */
    void Print() const override
    {
        std::cout << "Хеш-таблица (элементов: " << count
            << ", ёмкость: " << capacity << ")\n";

        for (int i = 0; i < capacity; ++i)
        {
            if (buckets[i])  // Показываем только непустые корзины
            {
                std::cout << "  Корзина[" << i << "]: ";
                Node* node = buckets[i];

                // Проходим по всей цепочке
                while (node)
                {
                    std::cout << "[" << node->key << " -> ";
                    // Для произвольных типов значений просто показываем наличие
                    std::cout << "(значение)";
                    std::cout << "]";
                    if (node->next) std::cout << " -> ";
                    node = node->next;
                }
                std::cout << "\n";
            }
        }
    }

  // ДОПОЛНИТЕЛЬНЫЕ МЕТОДЫ 

    /*
     * Безопасное получение значения по ключу
     * Возвращает true и устанавливает value, если ключ найден
     * Возвращает false, если ключ не найден
     */
    bool TryGetValue(const TKey& key, TValue& value) const
    {
        size_t index = GetBucketIndex(key);
        Node* node = buckets[index];

        while (node)
        {
            if (node->key == key)
            {
                value = node->value;
                return true;
            }
            node = node->next;
        }
        return false;
    }

    /*
     * Возвращает вектор всех ключей таблицы
     * Полезно для итерации по всем элементам
     */
    std::vector<TKey> GetKeys() const
    {
        std::vector<TKey> keys;
        keys.reserve(count);  // Резервируем память для эффективности

        // Проходим по всем корзинам
        for (int i = 0; i < capacity; ++i)
        {
            Node* node = buckets[i];
            while (node)
            {
                keys.push_back(node->key);
                node = node->next;
            }
        }
        return keys;
    }

    /*
     * Возвращает вектор всех значений таблицы
     * Порядок значений не определен
     */
    std::vector<TValue> GetValues() const
    {
        std::vector<TValue> values;
        values.reserve(count);

        for (int i = 0; i < capacity; ++i)
        {
            Node* node = buckets[i];
            while (node)
            {
                values.push_back(node->value);
                node = node->next;
            }
        }
        return values;
    }

    /*
     * Оператор индексации для доступа к элементам
     * Если ключ не существует, создает новый элемент со значением по умолчанию
     * Возвращает ссылку на значение для возможности модификации
     */
    TValue& operator[](const TKey& key)
    {
        size_t index = GetBucketIndex(key);
        Node* node = buckets[index];

        // Ищем существующий ключ
        while (node)
        {
            if (node->key == key)
            {
                return node->value;
            }
            node = node->next;
        }

        // Ключ не найден - проверяем, нужно ли рехеширование
        if (GetLoadFactor() >= loadFactorThreshold)
        {
            Rehash(static_cast<int>(capacity * expansionFactor));
            index = GetBucketIndex(key);  // Пересчитываем индекс после рехеширования
        }

        // Создаем новый узел со значением по умолчанию
        Node* newNode = new Node(key, TValue());
        newNode->next = buckets[index];
        buckets[index] = newNode;
        count++;

        // Возвращаем ссылку на значение нового узла
        return newNode->value;
    }

    /*
     * Константная версия оператора индексации
     * Только для чтения, не создает новых элементов
     */
    const TValue& operator[](const TKey& key) const
    {
        return Get(key);
    }

    /*
     * Выводит статистику таблицы:
     * - Количество элементов
     * - Емкость
     * - Коэффициент загрузки
     * - Количество пустых корзин
     * - Среднюю и максимальную длину цепочек
     */
    void PrintStats() const
    {
        std::cout << "=== Статистика хеш-таблицы ===\n";
        std::cout << "Количество элементов: " << count << "\n";
        std::cout << "Ёмкость: " << capacity << "\n";
        std::cout << "Коэффициент загрузки: " << GetLoadFactor() << "\n";

        int emptyBuckets = 0;       // Счетчик пустых корзин
        int maxChainLength = 0;     // Максимальная длина цепочки
        int totalChainLength = 0;   // Суммарная длина всех цепочек
        int nonEmptyBuckets = 0;    // Количество непустых корзин

        // Анализируем распределение элементов
        for (int i = 0; i < capacity; ++i)
        {
            if (!buckets[i])
            {
                emptyBuckets++;
            }
            else
            {
                nonEmptyBuckets++;
                int chainLength = 0;
                Node* node = buckets[i];

                // Измеряем длину цепочки
                while (node)
                {
                    chainLength++;
                    node = node->next;
                }

                totalChainLength += chainLength;
                maxChainLength = std::max(maxChainLength, chainLength);
            }
        }

        std::cout << "Пустых корзин: " << emptyBuckets << "/" << capacity << "\n";
        std::cout << "Средняя длина цепочки: "
            << (nonEmptyBuckets > 0 ? (double)totalChainLength / nonEmptyBuckets : 0) << "\n";
        std::cout << "Максимальная длина цепочки: " << maxChainLength << "\n";
    }

  //ИТЕРАТОР 

    /*
     * Класс итератора для обхода элементов хеш-таблицы
     * Реализует forward iterator (однонаправленный итератор)
     */
    class Iterator
    {
    private:
        const HashTable* table;     // Указатель на таблицу, по которой итерируем
        int bucketIndex;            // Текущий индекс корзины
        Node* currentNode;          // Текущий узел в цепочке

        /*
         * Находит следующий непустой элемент
         * Пропускает пустые корзины
         */
        void FindNext()
        {
            while (bucketIndex < table->capacity)
            {
                if (currentNode)
                {
                    return;  // Уже нашли следующий элемент
                }
                bucketIndex++;
                if (bucketIndex < table->capacity)
                {
                    currentNode = table->buckets[bucketIndex];
                }
            }
        }

    public:
        /*
         * Конструктор итератора
         * Начинает с заданной позиции
         */
        Iterator(const HashTable* tbl, int bucketIdx, Node* node)
            : table(tbl), bucketIndex(bucketIdx), currentNode(node)
        {
            // Если начальная позиция пуста, ищем следующий элемент
            if (bucketIndex < table->capacity && !currentNode)
            {
                FindNext();
            }
        }

        /*
         * Проверка неравенства итераторов
         * Используется в циклах for
         */
        bool operator!=(const Iterator& other) const
        {
            return !(*this == other);
        }

        /*
         * Проверка равенства итераторов
         * Итераторы равны, если указывают на один и тот же элемент
         */
        bool operator==(const Iterator& other) const
        {
            return bucketIndex == other.bucketIndex && currentNode == other.currentNode;
        }

        /*
         * Префиксный инкремент итератора
         * Переход к следующему элементу
         */
        Iterator& operator++()
        {
            if (currentNode)
            {
                // Переходим к следующему элементу в текущей цепочке
                currentNode = currentNode->next;
            }

            // Если цепочка закончилась, ищем следующую непустую корзину
            if (!currentNode)
            {
                bucketIndex++;
                if (bucketIndex < table->capacity)
                {
                    currentNode = table->buckets[bucketIndex];
                }
                // Пропускаем пустые корзины
                while (bucketIndex < table->capacity && !currentNode)
                {
                    bucketIndex++;
                    if (bucketIndex < table->capacity)
                    {
                        currentNode = table->buckets[bucketIndex];
                    }
                }
            }
            return *this;
        }

        /*
         * Оператор разыменования
         * Возвращает пару ключ-значение текущего элемента
         */
        std::pair<TKey, TValue> operator*() const
        {
            if (!currentNode)
            {
                throw std::out_of_range("Итератор за пределами коллекции");
            }
            return { currentNode->key, currentNode->value };
        }
    };

    /*
     * Возвращает итератор на первый элемент таблицы
     * Ищет первую непустую корзину
     */
    Iterator begin() const
    {
        for (int i = 0; i < capacity; ++i)
        {
            if (buckets[i])
            {
                return Iterator(this, i, buckets[i]);
            }
        }
        return end();  // Таблица пуста
    }

    /*
     * Возвращает итератор за последний элемент (end iterator)
     * Используется как маркер конца коллекции
     */
    Iterator end() const
    {
        return Iterator(this, capacity, nullptr);
    }
};

#endif // HASHTABLE_HPP