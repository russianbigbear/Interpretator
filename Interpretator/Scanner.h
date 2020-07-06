#ifndef ScannerH
#define ScannerH

#include "Dictionary.h"
#include <clocale>
#include <iostream>
using namespace std;

class TScanner{
private:
    TypeCode InputCode;                                 // Исходный код
    int Cur;                                            // Указатель на текущюю позицию
    int Str;                                            // Текущая строка
    int Pos;                                            // Текущая позиция

public:
    void ReadFile(string);                              // Чтение файла с кодом

    void SetCurrent (int ,int, int);                    // Задать указатель на текущюю позицию
    void GetCurrent (int*, int*, int*);                 // Получить указатель на текущюю позицию

    TScanner(const char *);                             // Конструктор
    ~TScanner() {}                                      // Деструктор

    int Scanner (TypeLexem);                            // Работа сканера

    void PrintError(const char*, bool);                 // Вывод ошибки с параметрами
    void PrintError();                                  // Вывод ошибки без параметров
};

#endif
