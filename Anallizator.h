#ifndef AnalizatorH
#define AnalizatorH

#include "Scanner.h"
#include "Tree.h"

class TAnallizator {
private:
    TScanner *Scanner;
    Tree * SemanticTree ;

public:
    TAnallizator(TScanner * s, Tree * r);       // �����������
    ~TAnallizator(){}                           // ����������

    void Programm();                            // ���������
    void Function();                            // �������
    void VariableDescription();                 // �������� ����������
    void Block();                               // ����
    void Operator();                            // ��������
    void Return();                              // �������
    void While();                               // �������� While
    void Assign();                              // ������������
    void CallFunction(TDataValue*);             // ����� �������

    // ��������
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
