#include "tests.hpp"
#include <windows.h>  // Добавьте этот заголовочный файл

// Функция для настройки кодировки консоли
void SetupConsole() {
    // Устанавливаем русскую кодовую страницу
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    // Для корректного отображения кириллицы
    system("chcp 1251 > nul");
}

int main()
{
    // Настраиваем консоль
    SetupConsole();

    try
    {
        RunAll();
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\nОШИБКА: " << e.what() << "\n";
        return 1;
    }
}