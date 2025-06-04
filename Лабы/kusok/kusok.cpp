#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <map>
#include <functional>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <windows.h>

#pragma execution_character_set( "utf-8" )

using namespace std;

using MathFunc = function<double(double, const vector<double>&)>;
struct FunctionSegment {
    string name;
    MathFunc func;
    vector<double> coefficients;
    pair<double, double> interval;
    string operation = ""; // "+", "*", "/", "&" (композиция)
    vector<FunctionSegment> operands;
};

map<string, MathFunc> predefinedFuncs = {
    {"poly", [](double x, const vector<double>& coefs) {
        double result = 0;
        for (size_t i = 0; i < coefs.size(); ++i) {
            result += coefs[i] * pow(x, coefs.size() - 1 - i);
        }
        return result;
    }},
    {"inv", [](double x, const vector<double>& coefs) {
        return coefs[0] / (x + coefs[1]);
    }},
    {"exp", [](double x, const vector<double>& coefs) { return coefs[0] * exp(coefs[1] * x); }},
    {"log", [](double x, const vector<double>& coefs) { return coefs[0] * log(coefs[1] * x); }},
    {"sin", [](double x, const vector<double>& coefs) { return coefs[0] * sin(coefs[1] * x); }},
    {"cos", [](double x, const vector<double>& coefs) { return coefs[0] * cos(coefs[1] * x); }},
    {"sqrt", [](double x, const vector<double>& coefs) { return coefs[0] * sqrt(coefs[1] * x); }}
};

vector<FunctionSegment> userFunctions;

vector<double> parseCoefficients(const string& coefStr) {
    vector<double> coefs;
    stringstream ss(coefStr);
    string token;
    while (getline(ss, token, ';')) {
        coefs.push_back(stod(token));
    }
    return coefs;
}

pair<double, double> parseInterval(const string& intervalStr) {
    string cleanStr = intervalStr.substr(1, intervalStr.size() - 2);
    size_t delimPos = cleanStr.find(';');
    double a = stod(cleanStr.substr(0, delimPos));
    double b = stod(cleanStr.substr(delimPos + 1));
    return make_pair(a, b);
}

double evaluateFunction(const FunctionSegment& func, double x) {
    if (x < func.interval.first || x > func.interval.second) {
        return NAN;
    }

    if (func.operation.empty()) {
        // Проверка особых случаев для базовых функций
        if (func.name == "log" && func.coefficients[1] * x <= 0) return NAN;
        if (func.name == "sqrt" && func.coefficients[1] * x < 0) return NAN;
        if (func.name == "inv" && x + func.coefficients[1] == 0) return NAN;

        return func.func(x, func.coefficients);
    }
    else {
        vector<double> values;
        for (const auto& op : func.operands) {
            double val = evaluateFunction(op, x);
            if (!isnan(val)) values.push_back(val);
        }

        if (values.empty()) return NAN;

        if (func.operation == "+") {
            double sum = 0;
            for (double v : values) sum += v;
            return sum;
        }
        else if (func.operation == "*") {
            double prod = 1;
            for (double v : values) prod *= v;
            return isfinite(prod) ? prod : NAN;
        }
        else if (func.operation == "/") {
            if (values.size() != 2 || values[1] == 0) return NAN;
            double res = values[0] / values[1];
            return isfinite(res) ? res : NAN;
        }
        else if (func.operation == "&") {
            if (values.size() != 2) return NAN;
            double gx = values[1];
            // Проверяем, что gx попадает в интервал первой функции
            if (gx < func.operands[0].interval.first ||
                gx > func.operands[0].interval.second) {
                return NAN;
            }
            return evaluateFunction(func.operands[0], gx);
        }
        return NAN;
    }
}

void printFunctionList() {
    if (userFunctions.empty()) {
        cout << "Нет добавленных функций.\n";
        return;
    }

    cout << "Текущие функции:\n";
    for (size_t i = 0; i < userFunctions.size(); ++i) {
        cout << i << ": " << userFunctions[i].name << " ";
        for (double coef : userFunctions[i].coefficients) {
            cout << coef << ";";
        }
        cout << " [" << userFunctions[i].interval.first << ";"
            << userFunctions[i].interval.second << "]";

        if (!userFunctions[i].operation.empty()) {
            cout << " " << userFunctions[i].operation << " "
                << userFunctions[i].operands[1].name;
        }
        cout << "\n";
    }
}

void addPredefinedFunction() {
    cout << "Доступные функции:\n";
    cout << "  poly c0;c1;...;cn - многочлен c0*x^n + c1*x^(n-1) + ... + cn\n";
    cout << "  inv c0;c1 - обратная функция c0/(x + c1)\n";
    for (const auto& func : predefinedFuncs) {
        if (func.first != "poly" && func.first != "inv") {
            cout << "  " << func.first << " c0;c1 - " << func.first << "(c1*x)*c0\n";
        }
    }

    string input;
    cout << "Введите функцию в формате:\n"
        << "  [f1] [coefs1] [interval1] [операция] [f2] [coefs2] [interval2]\n"
        << "  Примеры:\n"
        << "    poly 1;0;0 [0;2] + exp 1;1 [0;2]\n"
        << "    sin 1;1 [0;3.14] / poly 1;1 [0.1;3.14]\n"
        << "    poly 1;0 [0;2] & exp 1;1 [0;2]\n";
    cin.ignore();
    getline(cin, input);

    stringstream ss(input);
    vector<string> tokens;
    string token;
    while (ss >> token) tokens.push_back(token);

    if (tokens.size() < 3 || tokens.size() > 7) {
        cerr << "Неверный формат ввода!\n";
        return;
    }

    FunctionSegment newFunc;

    if (tokens.size() == 3) {
        if (predefinedFuncs.find(tokens[0]) == predefinedFuncs.end()) {
            cerr << "Функция " << tokens[0] << " не найдена!\n";
            return;
        }

        newFunc = {
            tokens[0],
            predefinedFuncs[tokens[0]],
            parseCoefficients(tokens[1]),
            parseInterval(tokens[2])
        };
    }
    else {
        if (tokens.size() != 7 ||
            (tokens[3] != "+" && tokens[3] != "*" && tokens[3] != "/" && tokens[3] != "&")) {
            cerr << "Неверный формат составной функции!\n";
            return;
        }

        FunctionSegment f1 = {
            tokens[0],
            predefinedFuncs[tokens[0]],
            parseCoefficients(tokens[1]),
            parseInterval(tokens[2])
        };

        FunctionSegment f2 = {
            tokens[4],
            predefinedFuncs[tokens[4]],
            parseCoefficients(tokens[5]),
            parseInterval(tokens[6])
        };

        newFunc.operation = tokens[3];
        newFunc.operands = { f1, f2 };
        newFunc.interval = {
            max(f1.interval.first, f2.interval.first),
            min(f1.interval.second, f2.interval.second)
        };
    }

    userFunctions.push_back(newFunc);
    cout << "Функция добавлена!\n";
    printFunctionList();
}

void renderPlot(GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (userFunctions.empty()) {
        glOrtho(-1, 1, -1, 1, -1, 1);
        return;
    }

    // Вычисляем диапазоны только по конечным значениям
    double xMin = userFunctions[0].interval.first;
    double xMax = userFunctions[0].interval.second;
    double yMin = INFINITY;
    double yMax = -INFINITY;

    const int SAMPLE_POINTS = min(1000, width * 2); // Ограничиваем количество точек
    for (const auto& func : userFunctions) {
        xMin = min(xMin, func.interval.first);
        xMax = max(xMax, func.interval.second);

        const double step = (func.interval.second - func.interval.first) / SAMPLE_POINTS;
        int pointsInRow = 0;

        for (double x = func.interval.first; x <= func.interval.second; x += step) {
            double y = evaluateFunction(func, x);
            if (isfinite(y)) {
                yMin = min(yMin, y);
                yMax = max(yMax, y);
                pointsInRow++;
            }
            else {
                pointsInRow = 0;
            }

            // Прерываем если слишком много точек подряд дают NaN/Inf
            if (pointsInRow == 0 && x > func.interval.first + step * 10) {
                break;
            }
        }
    }

    // Защита от особых случаев
    const double MAX_RANGE = 1e6;
    if (!isfinite(yMin) || yMin < -MAX_RANGE) yMin = -MAX_RANGE;
    if (!isfinite(yMax) || yMax > MAX_RANGE) yMax = MAX_RANGE;
    if (xMin == xMax) { xMin -= 1; xMax += 1; }
    if (yMin == yMax) { yMin -= 1; yMax += 1; }

    // Добавляем отступы
    double xPadding = (xMax - xMin) * 0.1;
    double yPadding = (yMax - yMin) * 0.1;
    glOrtho(xMin - xPadding, xMax + xPadding, yMin - yPadding, yMax + yPadding, -1, 1);

    // Оси
    glBegin(GL_LINES);
    glColor3f(0.7f, 0.7f, 0.7f);
    glVertex2f(xMin - xPadding, 0); glVertex2f(xMax + xPadding, 0);
    glVertex2f(0, yMin - yPadding); glVertex2f(0, yMax + yPadding);
    glEnd();

    // Графики с адаптивным рендерингом
    vector<vector<float>> colors = {
        {1,0,0}, {0,1,0}, {0,0,1}, {1,1,0}, {1,0,1}, {0,1,1}
    };

    for (size_t i = 0; i < userFunctions.size(); ++i) {
        const auto& func = userFunctions[i];
        glColor3fv(colors[i % colors.size()].data());

        const int SEGMENTS = min(2000, width * 2); // Ограничение количества сегментов
        const double step = (func.interval.second - func.interval.first) / SEGMENTS;

        bool drawing = false;
        glBegin(GL_LINES);

        for (double x = func.interval.first; x <= func.interval.second; x += step) {
            double y = evaluateFunction(func, x);

            if (isfinite(y)) {
                if (!drawing) {
                    glEnd();
                    glBegin(GL_LINE_STRIP);
                    drawing = true;
                }
                glVertex2f(x, y);
            }
            else {
                if (drawing) {
                    glEnd();
                    drawing = false;
                }
            }
        }

        if (drawing) {
            glEnd();
        }
    }
}
void removeFunction() {
    if (userFunctions.empty()) {
        cout << "Нет функций для удаления!\n";
        return;
    }

    printFunctionList();

    cout << "Введите номер функции для удаления (0-" << userFunctions.size() - 1 << ") или -1 для отмены: ";
    int index;
    cin >> index;

    if (index == -1) {
        cout << "Отмена удаления.\n";
        return;
    }

    if (index < 0 || index >= (int)userFunctions.size()) {
        cerr << "Неверный индекс!\n";
        return;
    }

    userFunctions.erase(userFunctions.begin() + index);
    cout << "Функция удалена!\n";
    printFunctionList();
}

void clearAllFunctions() {
    userFunctions.clear();
    cout << "Все функции удалены.\n";
}

int main() {
    SetConsoleOutputCP(65001);
    if (!glfwInit()) {
        cerr << "Ошибка инициализации GLFW!\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "График функций", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        renderPlot(window);
        glfwSwapBuffers(window);

        cout << "\n1. Добавить функцию\n"
            << "2. Вычислить значение\n"
            << "3. Удалить функцию\n"
            << "4. Удалить все функции\n"
            << "5. Показать список функций\n"
            << "6. Выход\n"
            << "Выберите: ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            addPredefinedFunction();
        }
        else if (choice == 2) {
            double x;
            cout << "Введите x: ";
            cin >> x;
            for (size_t i = 0; i < userFunctions.size(); ++i) {
                double result = evaluateFunction(userFunctions[i], x);
                if (!isnan(result)) {
                    cout << "f" << i << "(" << x << ") = " << result << "\n";
                }
                else {
                    cout << "f" << i << "(" << x << ") не определена\n";
                }
            }
        }
        else if (choice == 3) {
            removeFunction();
        }
        else if (choice == 4) {
            clearAllFunctions();
        }
        else if (choice == 5) {
            printFunctionList();
        }
        else if (choice == 6) {
            break;
        }
    }

    glfwTerminate();
    return 0;
}