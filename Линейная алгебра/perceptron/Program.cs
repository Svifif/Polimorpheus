using System;
using System.Diagnostics;

class Perceptron
{
    private double[] weights;
    private double bias;
    private readonly double learningRate;
    private readonly int epochs;

    public Perceptron(int inputSize, double learningRate = 0.1, int epochs = 100)
    {
        var random = new Random();
        this.weights = new double[inputSize];
        for (int i = 0; i < inputSize; i++)
        {
            weights[i] = (random.NextDouble() * 2 - 1) * 0.01; // Малая инициализация
        }
        this.bias = 0;
        this.learningRate = learningRate;
        this.epochs = epochs;
    }

    private double Sigmoid(double z)
    {
        return 1.0 / (1.0 + Math.Exp(-z));
    }

    public double[] Predict(double[][] inputs)
    {
        double[] outputs = new double[inputs.Length];
        for (int i = 0; i < inputs.Length; i++)
        {
            double z = bias;
            for (int j = 0; j < weights.Length; j++)
            {
                z += inputs[i][j] * weights[j];
            }
            outputs[i] = Sigmoid(z);
        }
        return outputs;
    }

    public void Train(double[][] trainInputs, int[] trainLabels, double[][] testInputs = null, int[] testLabels = null)
    {
        Stopwatch sw = Stopwatch.StartNew();

        for (int epoch = 0; epoch < epochs; epoch++)
        {
            double[] predictions = Predict(trainInputs);

            // Вычисление градиентов
            double[] dw = new double[weights.Length];
            double db = 0.0;

            for (int i = 0; i < trainInputs.Length; i++)
            {
                double error = predictions[i] - trainLabels[i];
                for (int j = 0; j < weights.Length; j++)
                {
                    dw[j] += error * trainInputs[i][j];
                }
                db += error;
            }

            // Обновление параметров
            for (int j = 0; j < weights.Length; j++)
            {
                weights[j] -= learningRate * dw[j] / trainInputs.Length;
            }
            bias -= learningRate * db / trainInputs.Length;

            // Логирование
            if (epoch % 10 == 0 || epoch == epochs - 1)
            {
                double trainAcc = EvaluateAccuracy(trainInputs, trainLabels);
                Console.Write($"Epoch {epoch,4}: Train Acc = {trainAcc:P2}");

                if (testInputs != null)
                {
                    double testAcc = EvaluateAccuracy(testInputs, testLabels);
                    Console.Write($", Test Acc = {testAcc:P2}");
                }

                Console.WriteLine($", Time = {sw.Elapsed.TotalSeconds:F2}s");
                sw.Restart();
            }
        }
    }

    public double EvaluateAccuracy(double[][] inputs, int[] labels)
    {
        double[] predictions = Predict(inputs);
        int correct = 0;
        for (int i = 0; i < predictions.Length; i++)
        {
            if ((predictions[i] > 0.5 ? 1 : 0) == labels[i])
                correct++;
        }
        return (double)correct / labels.Length;
    }
}

class Program
{
    static void Main()
    {
        // Параметры
        int numSamples = 1000000;
        int numFeatures = 30;
        double trainTestSplit = 0.8;

        // Генерация данных
        Console.WriteLine("Generating data...");
        var (allInputs, allLabels) = GenerateData(numSamples, numFeatures);

        // Разделение на train/test
        int trainSize = (int)(numSamples * trainTestSplit);
        var trainInputs = new double[trainSize][];
        var trainLabels = new int[trainSize];
        var testInputs = new double[numSamples - trainSize][];
        var testLabels = new int[numSamples - trainSize];

        Array.Copy(allInputs, 0, trainInputs, 0, trainSize);
        Array.Copy(allLabels, 0, trainLabels, 0, trainSize);
        Array.Copy(allInputs, trainSize, testInputs, 0, numSamples - trainSize);
        Array.Copy(allLabels, trainSize, testLabels, 0, numSamples - trainSize);

        // Создание и обучение модели
        Console.WriteLine("Training model...");
        var perceptron = new Perceptron(numFeatures, learningRate: 0.1, epochs: 50);
        perceptron.Train(trainInputs, trainLabels, testInputs, testLabels);

        // Финальная оценка
        double finalTrainAcc = perceptron.EvaluateAccuracy(trainInputs, trainLabels);
        double finalTestAcc = perceptron.EvaluateAccuracy(testInputs, testLabels);

        Console.WriteLine("\nFinal Results:");
        Console.WriteLine($"Training Accuracy: {finalTrainAcc:P2}");
        Console.WriteLine($"Test Accuracy:     {finalTestAcc:P2}");
    }

    static (double[][] inputs, int[] labels) GenerateData(int numSamples, int numFeatures)
    {
        var random = new Random(); // Фиксированный seed для воспроизводимости
        var inputs = new double[numSamples][];
        var labels = new int[numSamples];

        // Истинные веса (для генерации данных)
        var trueWeights = new double[numFeatures];
        for (int i = 0; i < numFeatures; i++)
        {
            trueWeights[i] = random.NextDouble() * 2 - 1;
        }
        double trueBias = random.NextDouble() * 2 - 1;

        // Генерация данных
        for (int i = 0; i < numSamples; i++)
        {
            inputs[i] = new double[numFeatures];

            // Генерация признаков
            for (int j = 0; j < numFeatures; j++)
            {
                inputs[i][j] = random.NextDouble() * 4 - 2; // Диапазон [-2, 2]
            }

            // Вычисление "истинной" вероятности
            double z = trueBias;
            for (int j = 0; j < numFeatures; j++)
            {
                z += trueWeights[j] * inputs[i][j];
            }
            double probability = 1.0 / (1.0 + Math.Exp(-z));

            // Добавление шума
            double noise = random.NextDouble() * 0.1 - 0.05; // ±10% шума
            probability = Math.Max(0, Math.Min(1, probability + noise));

            // Определение метки класса
            labels[i] = random.NextDouble() < probability ? 1 : 0;
        }

        return (inputs, labels);
    }
}