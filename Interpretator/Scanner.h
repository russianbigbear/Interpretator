#ifndef ScannerH
#define ScannerH

#include "Dictionary.h"
#include <clocale>
#include <iostream>
using namespace std;

class TScanner{
private:
    TypeCode InputCode;                                 // �������� ���
    int Cur;                                            // ��������� �� ������� �������
    int Str;                                            // ������� ������
    int Pos;                                            // ������� �������

public:
    void ReadFile(string);                              // ������ ����� � �����

    void SetCurrent (int ,int, int);                    // ������ ��������� �� ������� �������
    void GetCurrent (int*, int*, int*);                 // �������� ��������� �� ������� �������

    TScanner(const char *);                             // �����������
    ~TScanner() {}                                      // ����������

    int Scanner (TypeLexem);                            // ������ �������

    void PrintError(const char*, bool);                 // ����� ������ � �����������
    void PrintError();                                  // ����� ������ ��� ����������
};

#endif
