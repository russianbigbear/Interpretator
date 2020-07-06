#include "Dictionary.h"
#include "Anallizator.h"
#include <windows.h>

int main()
{
    setlocale(LC_ALL, "Russian");

    // ���� � �������� �����
    TScanner *_scanner = new TScanner("input.txt");

    // ������������� �������
    Tree *root = new Tree;
    TAnallizator* _anallizator = new TAnallizator(_scanner, root);
    _anallizator->Programm();

    // ��������� ���������� ���� � ����������� �������
    int _type;
    TypeLexem _lexem;
    _type = _scanner->Scanner(_lexem);

    // ����������
    if (_type == TEnd)
        printf("�������������� ������ �� ����������\n");
    else
        _scanner->PrintError("������ ����� � ����� ���������", false);

    return 0;
}
