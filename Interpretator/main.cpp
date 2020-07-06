#include "Dictionary.h"
#include "Anallizator.h"
#include <windows.h>

int main()
{
    setlocale(LC_ALL, "Russian");

    // файл с исхожным кодом
    TScanner *_scanner = new TScanner("input.txt");

    // семантическое деррево
    Tree *root = new Tree;
    TAnallizator* _anallizator = new TAnallizator(_scanner, root);
    _anallizator->Programm();

    // локальные переменные типа и изображения лексемы
    int _type;
    TypeLexem _lexem;
    _type = _scanner->Scanner(_lexem);

    // завершение
    if (_type == TEnd)
        printf("Синтаксических ошибок не обнаружено\n");
    else
        _scanner->PrintError("Лишний текст в конце программы", false);

    return 0;
}
