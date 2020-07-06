#ifndef TreeH
#define TreeH

#include "Dictionary.h"
#define max(a,b) a<b? b : a

// тип объекта
enum TypeObject {ObjVar, ObjFunc, ObjEmpty};

// тип данных
enum TDataType {TYPE_EMPTY, TYPE_UNKNOWN, TYPE_INTEGER, TYPE_DOUBLE, TYPE_CHAR};

// значение данных
union TDataValue {
    int DataAsInt;
    double DataAsDouble;
};

// позиция
struct Position {
    int cur;
    int str;
    int pos;
};

// вершина дерева
struct Node {
    TypeLexem Id;
    TypeObject TypeObj;
    TDataType DataType;
    TDataValue DataValue;

    Position PosInText;
    int Param;
};

// класс семантического дерева
class Tree {

private:
    Node * node;
    Tree * parent, * left, * right;

public:
    static Tree *Root;
    static Tree *Cur;

    static bool FLInt;
    static bool FLRet;

    // Конструкторы и деструктор
    Tree(Tree *, Tree *,Tree *,Node *);
    Tree(Node *);
    Tree();
    ~Tree();

    // Создание потомков
    void AddLeft (Node *);
    void AddRight (Node *);
    void AddLeft ();
    void AddRight ();

    // Работа с деревом
    Tree * FindUp (Tree *, TypeLexem);
    Tree * FindUp (TypeLexem id);
    Tree * FindRightLeft(Tree *, TypeLexem);
    Tree * FindRightLeft(TypeLexem);
    Tree * FindUpOneLevel (Tree *, TypeLexem);
    Tree * GetUp(Tree *);

    // Работа с позицией
    void SetPosInText (int, int, int);
    void GetPosInText (int *,int *, int *);

    // Семантические подпрограммы
    void SetCur(Tree *);
    Tree * GetCur();

    Tree * SemIncludePar(TypeLexem, TDataType);
    Tree * SemIncludeFunc(TypeLexem, TDataType);

    void SemSetType(Tree *, TDataType);
    Tree * SemGetType(TypeLexem a);
    Tree * SemGetFunct(TypeLexem a);

    void SemSetParamCount(Tree *, int);
    void SemParamEqual(Tree *, int);

    int CheckRepeat(Tree *, TypeLexem);
    bool CheckUno(TDataType);

    TDataType SemGetDataType(Tree *);
    TDataType SemGetFunctionReturnType(Tree *);

    void DeleteAfterBlock(Tree* addr);
    void FreeAfterBlock(Tree* addr);

    int GetIntValue(Tree* addr);
    unsigned GetDoubleValue(Tree* addr);
    TDataValue * GetDataValue(Tree* addr);
    void SetVarValue(Tree* addr,TDataValue value);
    Tree* SemCopyFunc(Tree* addr);
    void SemDelExeFunc(Tree* addr);
    void SemSetExeFuncParam(Tree* addr, TDataValue Value, int pos);
    Tree * SemGetBlockStart(Tree* addr);
    void SemFuncReturnValue(TDataValue);

    // Вывод
    void Print();
    void PrintError(const char *, char *);
};
#endif
