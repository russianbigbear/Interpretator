#include <string.h>
#include "Dictionary.h"
#include "Scanner.h"
#include "Tree.h"

Tree* Tree::Cur= static_cast<Tree*>(nullptr);
Tree* Tree::Root= static_cast<Tree*>(nullptr);
bool Tree::FLInt = true;
bool Tree::FLRet = false;

/*Деструктор*/
Tree::~Tree(){}

/*Конструктор для узла со связями и данными*/
Tree::Tree (Tree * _left, Tree * _right, Tree * _parrent, Node * _node) {
    if(!Tree::FLInt)
        return;

    node = new Node();

    parent = _parrent;
    left = _left;
    right = _right;

    // установили ссылки
    if (_node != nullptr)
        memcpy(node, _node, sizeof(Node));
    else
        memcpy(node, &"-fake-", sizeof(Node));
}

/*Конструктор нового узла с родителем*/
Tree::Tree (Node * _node) {
    if(!Tree::FLInt)
        return;

    node = new Node();
    parent = this;
    left = right = nullptr;

    // установили ссылки
    memcpy(node, _node, sizeof(Node));
}

/*Конструктор создает новый узел*/
Tree::Tree() {
    if(!Tree::FLInt)
        return;

    node = new Node();
    parent = left = right = nullptr;

    node->Param = 0;
    node->TypeObj = ObjEmpty;
    node->DataType = TYPE_EMPTY;
    node->DataValue.DataAsInt = 0;

    // установили ссылки
    memcpy(node->Id,&("-ROOT-"), sizeof(Node));
}

/*Создание левого потомка от this*/
void Tree::AddLeft (Node * Data) {
    if(!Tree::FLInt)
        return;

     // новая вершина
    Tree *tree = new Tree(nullptr, nullptr, this, Data);
    Cur->left = tree;
}

/*Создание правого потомка от this*/
void Tree::AddRight(Node * Data) {
    if(!Tree::FLInt)
        return;

    // новая вершина
    Tree *tree = new Tree(nullptr, nullptr, this, Data);
    Cur->right =  tree;
}

/*Создание левого потомка*/
void Tree::AddLeft () {
    if(!Tree::FLInt)
        return;

    // новая вершина
    Tree *tree= new Tree(nullptr, nullptr, this->Cur, nullptr);

    tree->node->Param = 0;
    tree->node->TypeObj = ObjEmpty;
    tree->node->DataType = this->node->DataType;
    tree->node->DataValue.DataAsInt = 0;

    Cur->left = tree;
    Cur = Cur->left;
}

/*Создание правого потомка*/
void Tree::AddRight() {
    if(!Tree::FLInt)
        return;

    // новая вершина
    Tree *tree= new Tree(nullptr,nullptr,this->Cur,nullptr);

    tree->node->Param = 0;
    tree->node->TypeObj = ObjEmpty;
    tree->node->DataType = this->node->DataType;
    tree->node->DataValue.DataAsInt = 0;

    Cur->right = tree;
    Cur = Cur->right;
}

/*Поиск данных в дереве, начиная от this вверх */
Tree * Tree::FindUp(TypeLexem type_lexem) {
    if(!Tree::FLInt)
        return nullptr;

    return FindUp(this->Cur, type_lexem);
}

/* поиск данных в дереве от From вверх*/
Tree * Tree::FindUp(Tree *From, TypeLexem type_lexem) {
    if(!Tree::FLInt)
        return nullptr;

    Tree *tree = From;

    // текущая вершина поиска
    while((tree != nullptr) && (memcmp(type_lexem, tree->node->Id, max(strlen(tree->node->Id), strlen(type_lexem))) != 0))
        tree = tree->parent;

    // поднялись
    return tree;
}

/*Поиск прямых потомков вершины this*/
Tree * Tree::FindRightLeft(TypeLexem type_lexem) {
    if(!Tree::FLInt)
        return nullptr;

    return FindRightLeft(this, type_lexem);
}

/*Поиск прямых потомков вершины From*/
Tree * Tree::FindRightLeft(Tree *From, TypeLexem type_lexem) {
    if(!Tree::FLInt)
        return nullptr;

    Tree *tree = From->right;

    // текущая вершина поиска
    while((tree!=nullptr) && (memcmp(type_lexem, tree->node->Id, max(strlen(tree->node->Id),strlen(type_lexem)))!=0))
        tree = tree->left;

    // обходим по левым связям
    return tree;
}

/*Поиск на одном уровне*/
Tree * Tree::FindUpOneLevel(Tree *From, TypeLexem type_lexem){
    if(!Tree::FLInt)
        return nullptr;

    Tree *tree = From;

    // текущая вершина поиска
    while((tree!=nullptr) && (tree->parent != nullptr )&&( tree->parent->right != tree)) {
        if (memcmp(type_lexem, tree->node->Id, max(strlen(tree->node->Id), strlen(type_lexem))) == 0)
            return tree; // нaшли совпадающий id

        // поднимаемся
        tree = tree->parent;
    }

    return nullptr;
}

/*Получение родителя*/
Tree* Tree::GetUp(Tree * address){
    if (address)
        return address->parent;
    else
        return nullptr;
}

/*Установить позицию*/
void Tree::SetPosInText(int cur, int pos, int str) {
    if(!Tree::FLInt)
        return;

    this->node->PosInText.cur = cur;
    this->node->PosInText.pos = pos;
    this->node->PosInText.str = str;
}

/*Получить позицию*/
void Tree::GetPosInText(int *cur, int *pos, int *str) {
    if(!Tree::FLInt)
        return;

    *cur = this->node->PosInText.cur;
    *pos = this->node->PosInText.pos;
    *str = this->node->PosInText.str;
}

/*Установить текущий узел дерева*/
void Tree::SetCur(Tree *tree){
    if(!Tree::FLInt)
        return;
    Cur = tree;
}

/*Получить текущий узел дерева*/
Tree * Tree::GetCur(void){
    if(!Tree::FLInt)
        return nullptr;
    return Cur;
}

/*Занесение идентификатора в таблицу с типом*/
Tree * Tree::SemIncludePar(TypeLexem type_lexem, TDataType type_data){
    if(!Tree::FLInt)
        return nullptr;

    if (CheckRepeat(Cur, type_lexem))
        PrintError("Повторное описание идентификатора ",type_lexem);

    Node node;

    memcpy(node.Id, type_lexem, strlen(type_lexem) + 1);

    node.TypeObj = ObjVar;
    node.DataType = type_data;
    node.Param = 0;

    // количество параметров функции
    Cur->AddLeft(&node);

    // сделали вершину - переменную
    Cur = Cur->left;
    return Cur;
}

/*Занесение функции в таблицу с типом*/
Tree * Tree::SemIncludeFunc(TypeLexem type_lexem, TDataType type_data){
    if(!Tree::FLInt)
        return nullptr;

    if (CheckRepeat(Cur, type_lexem))
        PrintError("Повторное описание идентификатора ", type_lexem);

    Tree *vertex;
    Node node;

    memcpy(node.Id,type_lexem,strlen(type_lexem)+1);

    node.TypeObj = ObjFunc;
    node.Param = 0;
    node.DataType = type_data;

    Cur->AddLeft(&node);

    // сделали вершину - функцию
    Cur = Cur->left;
    vertex = Cur;

    // это точка возврата после выхода из функции
    memcpy(&node.Id,&"-func_exit-",22);

    node.DataType=TYPE_EMPTY;
    node.Param=0;

    Cur->AddRight (&node);

    // сделали пустую вершину
    Cur = Cur->right;

    return vertex;
}

/*Установка типа для переменной по адресу*/
void Tree::SemSetType(Tree* address, TDataType type_data){
    if(!Tree::FLInt)
        return ;

    address->node->DataType = type_data;
}

/*Получениеbp из таблицы перемменой по типу*/
Tree * Tree::SemGetType(TypeLexem type_lexem){
    if(!Tree::FLInt)
        return nullptr;

    Tree * vertex = FindUp(Cur, type_lexem);

    if (vertex==nullptr)
        PrintError("Отсутствует описание идентификатора ", type_lexem);

    if (vertex->node->TypeObj == ObjFunc)
        PrintError("Неверное использование вызова функции ", type_lexem);

    return vertex;
}

/*Получение из таблицы функции*/
Tree * Tree::SemGetFunct(TypeLexem type_lexem){
    if(!Tree::FLInt)
        return nullptr;

    Tree * vertex = FindUp(Cur, type_lexem);

    if (vertex==nullptr)
        PrintError("Отсутствует описание функции ", type_lexem);

    if (vertex->node->TypeObj != ObjFunc)
        PrintError("Не является функцией идентификатор ", type_lexem);

    return vertex;
}

/*Установка числа формальных параметров*/
void Tree::SemSetParamCount(Tree *address, int count){
    if(!Tree::FLInt)
        return ;

    address->node->Param = count;
}

/*Проверка на число формальных параметров*/
void Tree::SemParamEqual(Tree *address, int count){
    if(!Tree::FLInt)
        return ;

    if (count != address->node->Param){
        PrintError("Неверное число параметров у функции ",address->node->Id);
        exit(1);
    }
}

/*Проверка на потвторение индентификатора*/
int Tree::CheckRepeat(Tree* address, TypeLexem type_data){
    if(!Tree::FLInt)
        return 0;

    if (FindUpOneLevel(address, type_data) == nullptr)
        return 0;

    return 1;
}

/*Проерка на  операцию*/
bool Tree::CheckUno(TDataType type_data){
    if(!Tree::FLInt)
        return false;

    if(type_data == TYPE_INTEGER)
        return true;

    if(type_data == TYPE_DOUBLE)
        return true;

    return false;
}

/*Получение типа данных*/
TDataType Tree::SemGetDataType(Tree *address){
    if(!Tree::FLInt)
        return TYPE_EMPTY;

    return address->node->DataType;
}

/*Получение возращаемого типа данных*/
TDataType Tree::SemGetFunctionReturnType(Tree *address){
    if(!Tree::FLInt)
        return TYPE_EMPTY;

    return  address->node->DataType;
}

/*Удалить ветку*/
void Tree::FreeAfterBlock(Tree *address){
    if(!Tree::FLInt)
        return;

    if (address->left){
        Tree::DeleteAfterBlock(address->left);
        delete address->left;
    }
    address->left = nullptr;
}

/*Рекурсивное удаление вершин */
void Tree::DeleteAfterBlock(Tree *address){
    if(!Tree::FLInt)
        return ;

    if (address->right) {
        DeleteAfterBlock(address->right);
        delete address->right;
        address->right = nullptr;
    }

    if (address->left){
        DeleteAfterBlock(address->left);
        delete address->left;
        address->left = nullptr;
    }
}

/*Получить значение int*/
int Tree::GetIntValue(Tree* address){
    if(!Tree::FLInt)
        return 0;

    return address->node->DataValue.DataAsInt;
}

/*Получить значение unsigned*/
unsigned Tree::GetDoubleValue(Tree* address){
    if(!Tree::FLInt)
        return 0;

    return address->node->DataValue.DataAsDouble;
}

/*Получить значение*/
TDataValue* Tree::GetDataValue(Tree* address){
    if(!Tree::FLInt)
        return nullptr;

    if(address)
        return &address->node->DataValue;

    return new TDataValue();
}

/*Установка значения*/
void Tree::SetVarValue(Tree *address, TDataValue value){
    if(!Tree::FLInt)
        return;

    switch (address->node->DataType) {
        case TYPE_INTEGER: {
            address->node->DataValue.DataAsInt = value.DataAsInt;
            break;
        }
        case TYPE_DOUBLE: {
            address->node->DataValue.DataAsDouble = value.DataAsDouble;
            break;
        }
        default: break;
    }
}

/*Копирование функции*/
Tree* Tree::SemCopyFunc(Tree *address){
    if(!Tree::FLInt)
        return nullptr;

    Node * node = new Node();
    memcpy(node->Id,address->node->Id,strlen(address->node->Id)+1);

    node->TypeObj = address->node->TypeObj;
    node->Param = address->node->Param;
    node->DataType = address->node->DataType;
    node->PosInText = address->node->PosInText;

    Tree* prot = Tree::FindUp(node->Id);
    Tree* exe = new Tree(prot->left, nullptr, prot, node);

    if(prot->left)
        prot->left->parent = exe;
    prot->left = exe;

    Tree* local_cur = exe;

    node = new Node();
    memcpy(node->Id, &"-func_exit-", 22);

    node->TypeObj = ObjEmpty;
    node->DataType = TYPE_EMPTY;
    node->Param = 0;

    local_cur->right = new Tree(nullptr, nullptr, local_cur, node);
    local_cur = local_cur ->right;

    Tree * local_proto_cur;
    local_proto_cur = address->right->left;

    while(local_proto_cur){
        node = new Node();
        memcpy(node->Id,local_proto_cur->node->Id,strlen(local_proto_cur->node->Id)+1);

        node->TypeObj=local_proto_cur->node->TypeObj;
        node->Param=local_proto_cur->node->Param;
        node->DataType = local_proto_cur->node->DataType;

        local_cur->left = new Tree(nullptr, nullptr, local_cur, node);
        local_cur = local_cur->left;
        local_proto_cur = local_proto_cur->left;
    }

    return exe;
}

/*Удаление функции*/
void Tree::SemDelExeFunc(Tree *address){
    if(!Tree::FLInt)
        return;

    Tree::DeleteAfterBlock(address->right);

    address->left->parent = address->parent;
    delete address;

    return;
}

/*Установить параметры функции*/
void Tree::SemSetExeFuncParam(Tree *address, TDataValue value, int pos){
    if(!Tree::FLInt)
        return;

    Tree* local_cur = address->right->left;

    while(--pos)
        local_cur = local_cur->left;

    local_cur->node->DataValue = value;
}

/*Получение стратового блока*/
Tree* Tree::SemGetBlockStart(Tree * address){
    if(!Tree::FLInt)
        return nullptr;

    Tree* local_cur = address->right->left;

    while(local_cur->left)
        local_cur = local_cur->left;

    return  local_cur;
}

/*Возращаемое значение функции*/
void Tree::SemFuncReturnValue(TDataValue value){
    if(!Tree::FLInt)
        return;

    Tree* tree = Cur;

    while((tree != nullptr) && (memcmp(&"-func_exit-", tree->node->Id, max(strlen(tree->node->Id),strlen("-func_exit-")))!=0) )
            tree=tree->parent;
    if (!tree)
        return;
    else
        tree->parent->node->DataValue = value;

}

/*Печать дерева*/
void Tree::Print() {
    if(!Tree::FLInt) return;
        printf("Вершина с данными %s ----->", node->Id );
        if (left !=nullptr) printf(" слева данные %s", left->node->Id );
        if (right!=nullptr) printf(" справа данные %s", right->node->Id );
        printf("\n");
        if (left!=nullptr) left->Print();
        if (right!=nullptr) right->Print();
}

/*Вывод ошибки с параметром*/
void Tree::PrintError(const char *err, char *a) {
    if(!Tree::FLInt)
        return;

    if (a == nullptr)
        printf("Синтаксическая ошибка : %s \n",err);
    else
        printf("Синтаксическая ошибка : %s. Неверный символ \'%s\' \n",err,a);
}


