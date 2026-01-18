#ifndef NEURAL_LEARNING_HPP
#define NEURAL_LEARNING_HPP

#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <string>
#include <set>
#include <numeric>
#include <iomanip>
#include <chrono>
#include "generate_maze.hpp"
#include "dijkstra_search.hpp"

// Параметры архитектуры нейронной сети
const int INPUT_SIZE = 10;      // Размер входного вектора (позиция, голод, стены, направление)
const int HIDDEN_SIZE = 16;     // Количество нейронов в скрытых слоях
const int OUTPUT_SIZE = 4;      // 4 направления движения: вниз, вправо, вверх, влево
const double LEARNING_RATE = 0.02; // Скорость обучения
const int TRAINING_EPOCHS = 300;  // Количество эпох обучения
const int MAZES_TO_GENERATE = 100; // Количество лабиринтов для генерации данных

/**
 * Структура для тренировочного примера с весовым коэффициентом.
 * Вес определяет важность примера при обучении.
 */
struct WeightedTrainingExample
{
    std::vector<double> inputs; // Входной вектор состояния
    int target_direction;       // Целевое направление движения
    double weight;              // Вес примера (чем дальше по пути, тем выше вес)
};

/**
 * Нейронная сеть с взвешенным обучением.
 * Особенность: придает большее значение примерам из поздних этапов пути.
 * Архитектура: 3 слоя (входной -> скрытый -> скрытый -> выходной)
 */
class WeightedNeuralNetwork
{
private:
    // Матрицы весов и векторы смещений
    std::vector<std::vector<double>> w1; // Веса между входным и первым скрытым слоем
    std::vector<std::vector<double>> w2; // Веса между скрытыми слоями
    std::vector<std::vector<double>> w3; // Веса между последним скрытым и выходным слоем
    std::vector<double> b1, b2, b3;      // Смещения для каждого слоя

    // Функция активации ReLU
    double relu(double x)
    {
        return std::max(0.0, x);
    }

public:
    WeightedNeuralNetwork()
    {
        initialize_weights();
    }

    /**
     * Инициализация весов сети методом He initialization.
     * Сложность: O(I×H + H×H + H×O), где I - входной размер, H - скрытый размер, O - выходной размер
     */
    void initialize_weights()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> dis(0.0, 0.1);

        // Стандартные отклонения для инициализации He
        double std1 = sqrt(2.0 / INPUT_SIZE);
        double std2 = sqrt(2.0 / HIDDEN_SIZE);
        double std3 = sqrt(2.0 / HIDDEN_SIZE);

        // Инициализация матриц весов
        w1.resize(INPUT_SIZE, std::vector<double>(HIDDEN_SIZE));
        w2.resize(HIDDEN_SIZE, std::vector<double>(HIDDEN_SIZE));
        w3.resize(HIDDEN_SIZE, std::vector<double>(OUTPUT_SIZE));

        // Инициализация векторов смещений
        b1.resize(HIDDEN_SIZE);
        b2.resize(HIDDEN_SIZE);
        b3.resize(OUTPUT_SIZE);

        // Заполнение весов первого слоя
        for (int i = 0; i < INPUT_SIZE; i++)
        {
            for (int j = 0; j < HIDDEN_SIZE; j++)
            {
                w1[i][j] = dis(gen) * std1;
            }
        }

        // Заполнение весов второго слоя
        for (int i = 0; i < HIDDEN_SIZE; i++)
        {
            for (int j = 0; j < HIDDEN_SIZE; j++)
            {
                w2[i][j] = dis(gen) * std2;
            }
            b1[i] = 0.0;
        }

        // Заполнение весов третьего слоя
        for (int i = 0; i < HIDDEN_SIZE; i++)
        {
            for (int j = 0; j < OUTPUT_SIZE; j++)
            {
                w3[i][j] = dis(gen) * std3;
            }
            b2[i] = 0.0;
        }

        // Инициализация смещений выходного слоя
        for (int i = 0; i < OUTPUT_SIZE; i++)
        {
            b3[i] = 0.0;
        }
    }

    /**
     * Создание входного вектора для нейронной сети.
     * Включает нормализованные значения позиции, голода, стен и направления к выходу.
     * Сложность: O(1) - фиксированное количество операций
     */
    std::vector<double> get_input_vector(int x, int y, int hunger) const
    {
        std::vector<double> inputs(INPUT_SIZE);

        // 1-2: Нормализованная позиция (от 0 до 1)
        inputs[0] = (x + 0.5) / WIDTH;
        inputs[1] = (y + 0.5) / HEIGHT;

        // 3: Нормализованный уровень голода
        inputs[2] = hunger / 100.0;

        // 4-7: Наличие стен в каждом направлении (1 = есть стена, 0 = нет)
        inputs[3] = (y == HEIGHT - 1 || maze[y][x].south) ? 1.0 : 0.0;
        inputs[4] = (x == WIDTH - 1 || maze[y][x].east) ? 1.0 : 0.0;
        inputs[5] = (y == 0 || maze[y][x].north) ? 1.0 : 0.0;
        inputs[6] = (x == 0 || maze[y][x].west) ? 1.0 : 0.0;

        // 8: Наличие еды в текущей клетке
        inputs[7] = maze[y][x].has_food ? 1.0 : 0.0;

        // 9-10: Направление к выходу (нормализованные смещения)
        inputs[8] = (WIDTH - 1 - x) / (double)WIDTH;
        inputs[9] = (HEIGHT - 1 - y) / (double)HEIGHT;

        return inputs;
    }

    /**
     * Предсказание направления движения с использованием softmax и температуры.
     * Температура контролирует случайность выбора (выше = более случайно).
     * Сложность: O(I×H + H×H + H×O) - прямое распространение
     */
    int predict(int x, int y, int hunger, double temperature = 1.0)
    {
        auto inputs = get_input_vector(x, y, hunger);

        // Прямое распространение через первый скрытый слой
        std::vector<double> h1(HIDDEN_SIZE, 0.0);
        for (int i = 0; i < HIDDEN_SIZE; i++)
        {
            for (int j = 0; j < INPUT_SIZE; j++)
            {
                h1[i] += inputs[j] * w1[j][i];
            }
            h1[i] += b1[i];
            h1[i] = relu(h1[i]);
        }

        // Прямое распространение через второй скрытый слой
        std::vector<double> h2(HIDDEN_SIZE, 0.0);
        for (int i = 0; i < HIDDEN_SIZE; i++)
        {
            for (int j = 0; j < HIDDEN_SIZE; j++)
            {
                h2[i] += h1[j] * w2[j][i];
            }
            h2[i] += b2[i];
            h2[i] = relu(h2[i]);
        }

        // Прямое распространение через выходной слой
        std::vector<double> outputs(OUTPUT_SIZE, 0.0);
        for (int i = 0; i < OUTPUT_SIZE; i++)
        {
            for (int j = 0; j < HIDDEN_SIZE; j++)
            {
                outputs[i] += h2[j] * w3[j][i];
            }
            outputs[i] += b3[i];
        }

        // Применение softmax с температурой для получения вероятностей
        std::vector<double> probs(OUTPUT_SIZE);
        double max_output = *std::max_element(outputs.begin(), outputs.end());
        double sum = 0.0;

        for (int i = 0; i < OUTPUT_SIZE; i++)
        {
            probs[i] = exp((outputs[i] - max_output) / temperature);
            sum += probs[i];
        }

        // Нормализация вероятностей
        for (int i = 0; i < OUTPUT_SIZE; i++)
        {
            probs[i] /= sum;
        }

        // Выбор направления на основе полученных вероятностей
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        double r = dis(gen);

        double cumulative = 0.0;
        for (int i = 0; i < OUTPUT_SIZE; i++)
        {
            cumulative += probs[i];
            if (r <= cumulative)
            {
                return i;
            }
        }

        return 0;
    }

    /**
     * Взвешенное обучение нейронной сети.
     * Примеры с большим весом оказывают большее влияние на обновление весов.
     * Сложность: O(E×N×(I×H + H×H + H×O)), где E - эпохи, N - количество примеров
     */
    void weighted_train(const std::vector<WeightedTrainingExample>& examples, int epochs = 50)
    {
        if (examples.empty())
        {
            return;
        }

        std::cout << "Weighted training on " << examples.size() << " examples..." << std::endl;

        // Находим максимальный вес для нормализации
        double max_weight = 0.0;
        for (const auto& example : examples)
        {
            if (example.weight > max_weight)
            {
                max_weight = example.weight;
            }
        }

        // Основной цикл обучения по эпохам
        for (int epoch = 0; epoch < epochs; epoch++)
        {
            double total_error = 0.0;
            int correct = 0;

            // Перемешиваем примеры для улучшения обучения
            std::vector<int> indices(examples.size());
            for (size_t i = 0; i < indices.size(); i++)
            {
                indices[i] = i;
            }
            std::random_shuffle(indices.begin(), indices.end());

            // Обработка каждого примера в эпохе
            for (size_t idx = 0; idx < indices.size(); idx++)
            {
                int example_idx = indices[idx];
                const auto& example = examples[example_idx];

                // Нормализация веса в диапазоне 0.5-2.0
                double normalized_weight = 0.5 + 1.5 * (example.weight / max_weight);

                // Прямое распространение
                auto inputs = example.inputs;
                int target = example.target_direction;

                std::vector<double> h1(HIDDEN_SIZE, 0.0);
                for (int i = 0; i < HIDDEN_SIZE; i++)
                {
                    for (int j = 0; j < INPUT_SIZE; j++)
                    {
                        h1[i] += inputs[j] * w1[j][i];
                    }
                    h1[i] += b1[i];
                    h1[i] = relu(h1[i]);
                }

                std::vector<double> h2(HIDDEN_SIZE, 0.0);
                for (int i = 0; i < HIDDEN_SIZE; i++)
                {
                    for (int j = 0; j < HIDDEN_SIZE; j++)
                    {
                        h2[i] += h1[j] * w2[j][i];
                    }
                    h2[i] += b2[i];
                    h2[i] = relu(h2[i]);
                }

                std::vector<double> outputs(OUTPUT_SIZE, 0.0);
                for (int i = 0; i < OUTPUT_SIZE; i++)
                {
                    for (int j = 0; j < HIDDEN_SIZE; j++)
                    {
                        outputs[i] += h2[j] * w3[j][i];
                    }
                    outputs[i] += b3[i];
                }

                // Softmax для получения вероятностей
                std::vector<double> probs(OUTPUT_SIZE);
                double max_output = *std::max_element(outputs.begin(), outputs.end());
                double sum = 0.0;

                for (int i = 0; i < OUTPUT_SIZE; i++)
                {
                    probs[i] = exp(outputs[i] - max_output);
                    sum += probs[i];
                }

                for (int i = 0; i < OUTPUT_SIZE; i++)
                {
                    probs[i] /= sum;
                }

                // Проверка правильности предсказания
                int prediction = std::max_element(probs.begin(), probs.end()) - probs.begin();
                if (prediction == target)
                {
                    correct++;
                }

                // Вычисление ошибки для каждого выходного нейрона
                for (int i = 0; i < OUTPUT_SIZE; i++)
                {
                    double error = probs[i] - (i == target ? 1.0 : 0.0);
                    total_error += error * error;

                    // Взвешенная скорость обучения
                    double weighted_lr = LEARNING_RATE * normalized_weight;

                    // Обновление весов третьего слоя
                    for (int j = 0; j < HIDDEN_SIZE; j++)
                    {
                        w3[j][i] -= weighted_lr * error * h2[j];
                    }
                    b3[i] -= weighted_lr * error;

                    // Обратное распространение для скрытых слоев
                    for (int j = 0; j < HIDDEN_SIZE; j++)
                    {
                        double hidden_error = error * w3[j][i];
                        if (h2[j] > 0) // Производная ReLU
                        {
                            // Обновление второго слоя
                            for (int k = 0; k < HIDDEN_SIZE; k++)
                            {
                                w2[k][j] -= weighted_lr * hidden_error * h1[k];
                            }
                            b2[j] -= weighted_lr * hidden_error;

                            // Обновление первого слоя
                            for (int k = 0; k < INPUT_SIZE; k++)
                            {
                                w1[k][j] -= weighted_lr * hidden_error * inputs[k];
                            }
                            b1[j] -= weighted_lr * hidden_error;
                        }
                    }
                }
            }

            // Логирование прогресса обучения
            if (epoch % 10 == 0 || epoch == epochs - 1)
            {
                double accuracy = 100.0 * correct / examples.size();
                std::cout << "Epoch " << epoch << ": Accuracy = "
                    << std::fixed << std::setprecision(2) << accuracy
                    << "%, Error = " << total_error << std::endl;
            }
        }
    }

    /**
     * Сохранение весов нейронной сети в файл.
     * Сложность: O(I×H + H×H + H×O) - запись всех весов
     */
    bool save_to_file(const std::string& filename)
    {
        std::ofstream file(filename, std::ios::binary);
        if (!file)
        {
            return false;
        }

        // Сохранение размеров сети
        int sizes[3] = { INPUT_SIZE, HIDDEN_SIZE, OUTPUT_SIZE };
        file.write((char*)sizes, sizeof(sizes));

        // Сохранение матриц весов
        for (const auto& row : w1)
        {
            file.write((char*)row.data(), row.size() * sizeof(double));
        }
        for (const auto& row : w2)
        {
            file.write((char*)row.data(), row.size() * sizeof(double));
        }
        for (const auto& row : w3)
        {
            file.write((char*)row.data(), row.size() * sizeof(double));
        }

        // Сохранение векторов смещений
        file.write((char*)b1.data(), b1.size() * sizeof(double));
        file.write((char*)b2.data(), b2.size() * sizeof(double));
        file.write((char*)b3.data(), b3.size() * sizeof(double));

        return true;
    }

    /**
     * Загрузка весов нейронной сети из файла.
     * Сложность: O(I×H + H×H + H×O) - чтение всех весов
     */
    bool load_from_file(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file)
        {
            return false;
        }

        // Проверка совместимости размеров сети
        int sizes[3];
        file.read((char*)sizes, sizeof(sizes));

        if (sizes[0] != INPUT_SIZE || sizes[1] != HIDDEN_SIZE || sizes[2] != OUTPUT_SIZE)
        {
            std::cout << "Network size mismatch!" << std::endl;
            return false;
        }

        // Выделение памяти под веса
        w1.resize(INPUT_SIZE, std::vector<double>(HIDDEN_SIZE));
        w2.resize(HIDDEN_SIZE, std::vector<double>(HIDDEN_SIZE));
        w3.resize(HIDDEN_SIZE, std::vector<double>(OUTPUT_SIZE));
        b1.resize(HIDDEN_SIZE);
        b2.resize(HIDDEN_SIZE);
        b3.resize(OUTPUT_SIZE);

        // Загрузка матриц весов
        for (auto& row : w1)
        {
            file.read((char*)row.data(), row.size() * sizeof(double));
        }
        for (auto& row : w2)
        {
            file.read((char*)row.data(), row.size() * sizeof(double));
        }
        for (auto& row : w3)
        {
            file.read((char*)row.data(), row.size() * sizeof(double));
        }

        // Загрузка векторов смещений
        file.read((char*)b1.data(), b1.size() * sizeof(double));
        file.read((char*)b2.data(), b2.size() * sizeof(double));
        file.read((char*)b3.data(), b3.size() * sizeof(double));

        return true;
    }
};

/**
 * Добавление критически важных примеров с высоким весом.
 * Включает позиции рядом с выходом и позиции с едой.
 * Сложность: O(K), где K - количество добавляемых примеров
 */
void add_critical_examples(std::vector<WeightedTrainingExample>& data, WeightedNeuralNetwork& nn)
{
    std::cout << "   Adding CRITICAL examples with HIGH weights..." << std::endl;

    // Примеры для клеток рядом с выходом (самый высокий вес)
    for (int x = WIDTH - 3; x < WIDTH; x++)
    {
        for (int y = HEIGHT - 3; y < HEIGHT; y++)
        {
            if (x < 0 || y < 0)
            {
                continue;
            }

            // Определение оптимального направления к выходу
            int dx = WIDTH - 1 - x;
            int dy = HEIGHT - 1 - y;

            int best_dir = 0;
            if (abs(dx) > abs(dy))
            {
                best_dir = (dx > 0) ? 1 : 3; // Вправо или влево
            }
            else
            {
                best_dir = (dy > 0) ? 0 : 2; // Вниз или вверх
            }

            // Добавление примеров с разным уровнем голода
            for (int hunger : {10, 20, 30})
            {
                auto inputs = nn.get_input_vector(x, y, hunger);

                WeightedTrainingExample example;
                example.inputs = inputs;
                example.target_direction = best_dir;
                example.weight = 10.0; // Максимальный вес для критических позиций

                data.push_back(example);
            }
        }
    }

    // Примеры для позиций с едой (высокий вес)
    for (const auto& food_pos : food_locations)
    {
        int x = food_pos.first;
        int y = food_pos.second;

        // Определение направления от еды к выходу
        int dx = WIDTH - 1 - x;
        int dy = HEIGHT - 1 - y;

        int best_dir = 0;
        if (abs(dx) > abs(dy))
        {
            best_dir = (dx > 0) ? 1 : 3;
        }
        else
        {
            best_dir = (dy > 0) ? 0 : 2;
        }

        // Добавление примеров с разным уровнем голода
        for (int hunger : {5, 15, 25})
        {
            auto inputs = nn.get_input_vector(x, y, hunger);

            WeightedTrainingExample example;
            example.inputs = inputs;
            example.target_direction = best_dir;
            example.weight = 5.0; // Высокий вес для позиций с едой

            data.push_back(example);
        }
    }
}

/**
 * Определение направления движения между двумя точками пути.
 * Сложность: O(1)
 */
int get_direction_from_path(const std::pair<int, int>& current,
    const std::pair<int, int>& next)
{
    if (next.first > current.first)
    {
        return 1;  // Вправо
    }
    if (next.first < current.first)
    {
        return 3;  // Влево
    }
    if (next.second > current.second)
    {
        return 0; // Вниз
    }
    if (next.second < current.second)
    {
        return 2; // Вверх
    }
    return 0;
}

/**
 * Генерация взвешенных тренировочных данных из множества лабиринтов.
 * Особенность: вес примера увеличивается с каждым шагом пути.
 * Сложность: O(M×P), где M - количество лабиринтов, P - длина пути
 */
std::vector<WeightedTrainingExample> generate_weighted_training_data(int num_mazes)
{
    std::vector<WeightedTrainingExample> training_data;

    std::cout << "\n Generating WEIGHTED training data..." << std::endl;
    std::cout << "   Examples from LATER steps get MORE weight!" << std::endl;

    WeightedNeuralNetwork temp_nn;
    int successful_mazes = 0;

    // Генерация данных из каждого лабиринта
    for (int maze_idx = 0; maze_idx < num_mazes; maze_idx++)
    {
        generate_maze();

        path.clear();
        if (!find_shortest_path())
        {
            continue;
        }

        successful_mazes++;

        // Параметры взвешивания: вес увеличивается с каждым шагом
        double base_weight = 1.0;
        double weight_increment = 0.5;

        // Обработка каждого шага в найденном пути
        for (size_t step = 0; step < path.size() - 1; step++)
        {
            int x = path[step].first;
            int y = path[step].second;

            // Моделирование уменьшения голода со временем
            int hunger = INITIAL_HUNGER - (step * HUNGER_COST_PER_STEP);
            hunger = std::max(5, hunger); // Минимальный уровень голода

            auto inputs = temp_nn.get_input_vector(x, y, hunger);
            int target_dir = get_direction_from_path(path[step], path[step + 1]);

            // Ключевая особенность: вес увеличивается с каждым шагом пути
            double weight = base_weight + (step * weight_increment);

            WeightedTrainingExample example;
            example.inputs = inputs;
            example.target_direction = target_dir;
            example.weight = weight;

            training_data.push_back(example);

            // Для шагов во второй половине пути добавляем дополнительный пример с высоким голодом
            if (step > path.size() / 2)
            {
                int high_hunger = INITIAL_HUNGER * 3 / 4;
                auto inputs2 = temp_nn.get_input_vector(x, y, high_hunger);

                WeightedTrainingExample example2;
                example2.inputs = inputs2;
                example2.target_direction = target_dir;
                example2.weight = weight * 1.5; // Еще больший вес

                training_data.push_back(example2);
            }
        }

        // Периодический вывод прогресса
        if (maze_idx % 20 == 0)
        {
            std::cout << "  Maze " << maze_idx << ": " << training_data.size()
                << " examples (weight range: 1.0 to "
                << (base_weight + ((path.size() - 1) * weight_increment)) << ")" << std::endl;
        }
    }

    // Добавление критических примеров (выход и еда)
    add_critical_examples(training_data, temp_nn);

    // Статистика сгенерированных данных
    std::cout << "\n Generated " << training_data.size() << " WEIGHTED examples from "
        << successful_mazes << "/" << num_mazes << " mazes" << std::endl;

    // Вычисление статистики весов
    double min_weight = 1000, max_weight = 0, avg_weight = 0;
    for (const auto& example : training_data)
    {
        if (example.weight < min_weight)
        {
            min_weight = example.weight;
        }
        if (example.weight > max_weight)
        {
            max_weight = example.weight;
        }
        avg_weight += example.weight;
    }
    avg_weight /= training_data.size();

    std::cout << "   Weight statistics: Min=" << std::fixed << std::setprecision(2) << min_weight
        << ", Max=" << max_weight << ", Avg=" << avg_weight << std::endl;

    return training_data;
}

/**
 * Основная функция для поиска пути с использованием взвешенной нейронной сети.
 * Может использовать предварительно обученную модель или обучить новую.
 * Сложность: O(T + S×P), где T - сложность обучения, S - количество шагов, P - сложность предсказания
 */
std::vector<std::pair<int, int>> find_path_weighted_neural(bool use_pretrained = false)
{
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "     WEIGHTED NEURAL PATHFINDING" << std::endl;
    std::cout << "    LATER STEPS = MORE IMPORTANCE!" << std::endl;
    std::cout << std::string(70, '=') << std::endl;

    WeightedNeuralNetwork nn;
    const std::string model_file = "weighted_nn_weights.bin";

    // Загрузка предварительно обученной модели или обучение новой
    if (use_pretrained && nn.load_from_file(model_file))
    {
        std::cout << " Loaded pre-trained WEIGHTED network!" << std::endl;
    }
    else
    {
        std::cout << " Training NEW WEIGHTED network..." << std::endl;
        std::cout << "   Examples from later path steps get MORE weight!" << std::endl;

        auto start = std::chrono::steady_clock::now();
        auto training_data = generate_weighted_training_data(MAZES_TO_GENERATE);
        auto end = std::chrono::steady_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        std::cout << "📊 Data generation: " << duration.count() << " seconds" << std::endl;

        if (training_data.empty())
        {
            std::cout << "❌ No training data!" << std::endl;
            return std::vector<std::pair<int, int>>();
        }

        // Взвешенное обучение сети
        nn.weighted_train(training_data, 60);

        // Сохранение обученной модели
        if (nn.save_to_file(model_file))
        {
            std::cout << " Saved WEIGHTED model to " << model_file << std::endl;
        }
    }

    // Поиск пути с использованием обученной нейронной сети
    std::cout << "\n Finding path with WEIGHTED neural network..." << std::endl;

    std::vector<std::pair<int, int>> neural_path;
    std::set<std::pair<int, int>> visited;

    int x = 0, y = 0;
    int hunger = INITIAL_HUNGER;
    const int MAX_STEPS = 250;
    int steps = 0;
    int progress_counter = 0;

    neural_path.push_back(std::make_pair(x, y));
    visited.insert(std::make_pair(x, y));

    // Основной цикл движения агента
    while (steps < MAX_STEPS && hunger > 0)
    {
        // Проверка достижения выхода
        if (x == WIDTH - 1 && y == HEIGHT - 1)
        {
            std::cout << " Reached exit at step " << steps << "!" << std::endl;
            break;
        }

        // Постепенное уменьшение температуры (становимся более уверенными)
        double temperature = std::max(0.3, 1.0 - (steps / 200.0));
        int direction = nn.predict(x, y, hunger, temperature);

        // Попытка движения в предсказанном направлении
        int nx = x, ny = y;
        const int dx[4] = { 0, 1, 0, -1 };
        const int dy[4] = { 1, 0, -1, 0 };

        bool moved = false;
        for (int attempt = 0; attempt < 4 && !moved; attempt++)
        {
            int dir = (direction + attempt) % 4;
            nx = x + dx[dir];
            ny = y + dy[dir];

            if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT)
            {
                continue;
            }

            // Проверка наличия стены
            bool wall = false;
            if (dir == 0 && maze[y][x].south)
            {
                wall = true;
            }
            else if (dir == 1 && maze[y][x].east)
            {
                wall = true;
            }
            else if (dir == 2 && maze[y][x].north)
            {
                wall = true;
            }
            else if (dir == 3 && maze[y][x].west)
            {
                wall = true;
            }

            if (!wall)
            {
                std::pair<int, int> new_pos = std::make_pair(nx, ny);

                // Разрешаем некоторые повторные посещения для исследования
                if (visited.find(new_pos) == visited.end() ||
                    (visited.size() > 20 && progress_counter < 3))
                {
                    x = nx;
                    y = ny;
                    neural_path.push_back(std::make_pair(x, y));

                    if (visited.find(new_pos) == visited.end())
                    {
                        visited.insert(new_pos);
                        progress_counter = 0;
                    }
                    else
                    {
                        progress_counter++;
                    }

                    // Сбор еды
                    if (maze[y][x].has_food)
                    {
                        hunger = std::min(100, hunger + FOOD_RESTORE);
                        std::cout << " Found food at (" << x << "," << y << ")! Hunger: " << hunger << std::endl;
                    }

                    moved = true;
                }
            }
        }

        hunger -= HUNGER_COST_PER_STEP;
        steps++;

        // Периодический вывод прогресса
        if (steps % 30 == 0)
        {
            int distance = abs(WIDTH - 1 - x) + abs(HEIGHT - 1 - y);
            std::cout << "   Step " << steps << ": Pos=(" << x << "," << y
                << "), Hunger=" << hunger << ", Dist=" << distance << std::endl;
        }
    }

    // Вывод результатов работы нейронной сети
    std::cout << "\n" << std::string(50, '-') << std::endl;
    std::cout << " WEIGHTED NEURAL NETWORK RESULTS" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    bool reached_exit = (x == WIDTH - 1 && y == HEIGHT - 1);
    int distance = abs(WIDTH - 1 - x) + abs(HEIGHT - 1 - y);
    double efficiency = 100.0 * visited.size() / neural_path.size();

    std::cout << " Exit reached: " << (reached_exit ? "YES " : "NO ❌") << std::endl;
    std::cout << " Final position: (" << x << ", " << y << ")" << std::endl;
    std::cout << " Distance to exit: " << distance << " cells" << std::endl;
    std::cout << " Path length: " << neural_path.size() << " steps" << std::endl;
    std::cout << " Unique cells: " << visited.size() << std::endl;
    std::cout << " Efficiency: " << std::fixed << std::setprecision(1) << efficiency << "%" << std::endl;
    std::cout << " Final hunger: " << hunger << "/" << INITIAL_HUNGER << std::endl;

    // Оценка успешности
    if (reached_exit)
    {
        std::cout << "\n SUCCESS! Weighted neural network ROCKS! " << std::endl;
        std::cout << "   Later steps = More importance strategy WORKED!" << std::endl;
    }
    else if (distance <= 3)
    {
        std::cout << "\n Almost there! Only " << distance << " cells away!" << std::endl;
    }
    else if (distance <= 10)
    {
        std::cout << "\n Good progress! " << distance << " cells to go." << std::endl;
    }

    std::cout << "\n Press SPACE to animate the WEIGHTED neural agent!" << std::endl;
    std::cout << std::string(70, '=') << std::endl;

    return neural_path;
}

#endif