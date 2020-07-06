#ifndef AnalizatorH
#define AnalizatorH

#include "Scanner.h"
#include "Tree.h"

class TAnallizator {
private:
    TScanner *Scanner;
    Tree * SemanticTree ;

public:
    TAnallizator(TScanner * s, Tree * r);       // Конструктор
    ~TAnallizator(){}                           // Деструктор

    void Programm();                            // Программа
    void Function();                            // Функция
    void VariableDescription();                 // Описания переменных
    void Block();                               // Блок
    void Operator();                            // Оператор
    void Return();                              // Возврат
    void While();                               // Оператор While
    void Assign();                              // Присваивание
    void CallFunction(TDataValue*);             // Вызов функции

    // операции
    void A1(TDataType*, TDataValue*);
    void A2(TDataType*, TDataValue*);
    void A3(TDataType*, TDataValue*);
    void A4(TDataType*, TDataValue*);
    void A5(TDataType*, TDataValue*);
    void A6(TDataType*, TDataValue*);
    void A7(TDataType*, TDataValue*);

    void logAssing(char *,TDataType,TDataValue);
    void logChange(char *,const char*,TDataType,TDataValue);
 };

#endif
