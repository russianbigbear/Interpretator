#include <string.h>
#include "Dictionary.h"
#include "Scanner.h"
#include "Tree.h"

Tree* Tree::Cur= static_cast<Tree*>(nullptr);
Tree* Tree::Root= static_cast<Tree*>(nullptr);
bool Tree::FLInt = true;
bool Tree::FLRet = false;

/*����������*/
Tree::~Tree(){}

/*����������� ��� ���� �� ������� � �������*/
Tree::Tree (Tree * _left, Tree * _right, Tree * _parrent, Node * _node) {
    if(!Tree::FLInt)
        return;

    node = new Node();

    parent = _parrent;
    left = _left;
    right = _right;

    // ���������� ������
    if (_node != nullptr)
        memcpy(node, _node, sizeof(Node));
    else
        memcpy(node, &"-fake-", sizeof(Node));
}

/*����������� ������ ���� � ���������*/
Tree::Tree (Node * _node) {
    if(!Tree::FLInt)
        return;

    node = new Node();
    parent = this;
    left = right = nullptr;

    // ���������� ������
    memcpy(node, _node, sizeof(Node));
}

/*����������� ������� ����� ����*/
Tree::Tree() {
    if(!Tree::FLInt)
        return;

    node = new Node();
    parent = left = right = nullptr;

    node->Param = 0;
    node->TypeObj = ObjEmpty;
    node->DataType = TYPE_EMPTY;
    node->DataValue.DataAsInt = 0;

    // ���������� ������
    memcpy(node->Id,&("-ROOT-"), sizeof(Node));
}

/*�������� ������ ������� �� this*/
void Tree::AddLeft (Node * Data) {
    if(!Tree::FLInt)
        return;

     // ����� �������
    Tree *tree = new Tree(nullptr, nullptr, this, Data);
    Cur->left = tree;
}

/*�������� ������� ������� �� this*/
void Tree::AddRight(Node * Data) {
    if(!Tree::FLInt)
        return;

    // ����� �������
    Tree *tree = new Tree(nullptr, nullptr, this, Data);
    Cur->right =  tree;
}

/*�������� ������ �������*/
void Tree::AddLeft () {
    if(!Tree::FLInt)
        return;

    // ����� �������
    Tree *tree= new Tree(nullptr, nullptr, this->Cur, nullptr);

    tree->node->Param = 0;
    tree->node->TypeObj = ObjEmpty;
    tree->node->DataType = this->node->DataType;
    tree->node->DataValue.DataAsInt = 0;

    Cur->left = tree;
    Cur = Cur->left;
}

/*�������� ������� �������*/
void Tree::AddRight() {
    if(!Tree::FLInt)
        return;

    // ����� �������
    Tree *tree= new Tree(nullptr,nullptr,this->Cur,nullptr);

    tree->node->Param = 0;
    tree->node->TypeObj = ObjEmpty;
    tree->node->DataType = this->node->DataType;
    tree->node->DataValue.DataAsInt = 0;

    Cur->right = tree;
    Cur = Cur->right;
}

/*����� ������ � ������, ������� �� this ����� */
Tree * Tree::FindUp(TypeLexem type_lexem) {
    if(!Tree::FLInt)
        return nullptr;

    return FindUp(this->Cur, type_lexem);
}

/* ����� ������ � ������ �� From �����*/
Tree * Tree::FindUp(Tree *From, TypeLexem type_lexem) {
    if(!Tree::FLInt)
        return nullptr;

    Tree *tree = From;

    // ������� ������� ������
    while((tree != nullptr) && (memcmp(type_lexem, tree->node->Id, max(strlen(tree->node->Id), strlen(type_lexem))) != 0))
        tree = tree->parent;

    // ���������
    return tree;
}

/*����� ������ �������� ������� this*/
Tree * Tree::FindRightLeft(TypeLexem type_lexem) {
    if(!Tree::FLInt)
        return nullptr;

    return FindRightLeft(this, type_lexem);
}

/*����� ������ �������� ������� From*/
Tree * Tree::FindRightLeft(Tree *From, TypeLexem type_lexem) {
    if(!Tree::FLInt)
        return nullptr;

    Tree *tree = From->right;

    // ������� ������� ������
    while((tree!=nullptr) && (memcmp(type_lexem, tree->node->Id, max(strlen(tree->node->Id),strlen(type_lexem)))!=0))
        tree = tree->left;

    // ������� �� ����� ������
    return tree;
}

/*����� �� ����� ������*/
Tree * Tree::FindUpOneLevel(Tree *From, TypeLexem type_lexem){
    if(!Tree::FLInt)
        return nullptr;

    Tree *tree = From;

    // ������� ������� ������
    while((tree!=nullptr) && (tree->parent != nullptr )&&( tree->parent->right != tree)) {
        if (memcmp(type_lexem, tree->node->Id, max(strlen(tree->node->Id), strlen(type_lexem))) == 0)
            return tree; // �a��� ����������� id

        // �����������
        tree = tree->parent;
    }

    return nullptr;
}

/*��������� ��������*/
Tree* Tree::GetUp(Tree * address){
    if (address)
        return address->parent;
    else
        return nullptr;
}

/*���������� �������*/
void Tree::SetPosInText(int cur, int pos, int str) {
    if(!Tree::FLInt)
        return;

    this->node->PosInText.cur = cur;
    this->node->PosInText.pos = pos;
    this->node->PosInText.str = str;
}

/*�������� �������*/
void Tree::GetPosInText(int *cur, int *pos, int *str) {
    if(!Tree::FLInt)
        return;

    *cur = this->node->PosInText.cur;
    *pos = this->node->PosInText.pos;
    *str = this->node->PosInText.str;
}

/*���������� ������� ���� ������*/
void Tree::SetCur(Tree *tree){
    if(!Tree::FLInt)
        return;
    Cur = tree;
}

/*�������� ������� ���� ������*/
Tree * Tree::GetCur(void){
    if(!Tree::FLInt)
        return nullptr;
    return Cur;
}

/*��������� �������������� � ������� � �����*/
Tree * Tree::SemIncludePar(TypeLexem type_lexem, TDataType type_data){
    if(!Tree::FLInt)
        return nullptr;

    if (CheckRepeat(Cur, type_lexem))
        PrintError("��������� �������� �������������� ",type_lexem);

    Node node;

    memcpy(node.Id, type_lexem, strlen(type_lexem) + 1);

    node.TypeObj = ObjVar;
    node.DataType = type_data;
    node.Param = 0;

    // ���������� ���������� �������
    Cur->AddLeft(&node);

    // ������� ������� - ����������
    Cur = Cur->left;
    return Cur;
}

/*��������� ������� � ������� � �����*/
Tree * Tree::SemIncludeFunc(TypeLexem type_lexem, TDataType type_data){
    if(!Tree::FLInt)
        return nullptr;

    if (CheckRepeat(Cur, type_lexem))
        PrintError("��������� �������� �������������� ", type_lexem);

    Tree *vertex;
    Node node;

    memcpy(node.Id,type_lexem,strlen(type_lexem)+1);

    node.TypeObj = ObjFunc;
    node.Param = 0;
    node.DataType = type_data;

    Cur->AddLeft(&node);

    // ������� ������� - �������
    Cur = Cur->left;
    vertex = Cur;

    // ��� ����� �������� ����� ������ �� �������
    memcpy(&node.Id,&"-func_exit-",22);

    node.DataType=TYPE_EMPTY;
    node.Param=0;

    Cur->AddRight (&node);

    // ������� ������ �������
    Cur = Cur->right;

    return vertex;
}

/*��������� ���� ��� ���������� �� ������*/
void Tree::SemSetType(Tree* address, TDataType type_data){
    if(!Tree::FLInt)
        return ;

    address->node->DataType = type_data;
}

/*���������bp �� ������� ���������� �� ����*/
Tree * Tree::SemGetType(TypeLexem type_lexem){
    if(!Tree::FLInt)
        return nullptr;

    Tree * vertex = FindUp(Cur, type_lexem);

    if (vertex==nullptr)
        PrintError("����������� �������� �������������� ", type_lexem);

    if (vertex->node->TypeObj == ObjFunc)
        PrintError("�������� ������������� ������ ������� ", type_lexem);

    return vertex;
}

/*��������� �� ������� �������*/
Tree * Tree::SemGetFunct(TypeLexem type_lexem){
    if(!Tree::FLInt)
        return nullptr;

    Tree * vertex = FindUp(Cur, type_lexem);

    if (vertex==nullptr)
        PrintError("����������� �������� ������� ", type_lexem);

    if (vertex->node->TypeObj != ObjFunc)
        PrintError("�� �������� �������� ������������� ", type_lexem);

    return vertex;
}

/*��������� ����� ���������� ����������*/
void Tree::SemSetParamCount(Tree *address, int count){
    if(!Tree::FLInt)
        return ;

    address->node->Param = count;
}

/*�������� �� ����� ���������� ����������*/
void Tree::SemParamEqual(Tree *address, int count){
    if(!Tree::FLInt)
        return ;

    if (count != address->node->Param){
        PrintError("�������� ����� ���������� � ������� ",address->node->Id);
        exit(1);
    }
}

/*�������� �� ����������� ���������������*/
int Tree::CheckRepeat(Tree* address, TypeLexem type_data){
    if(!Tree::FLInt)
        return 0;

    if (FindUpOneLevel(address, type_data) == nullptr)
        return 0;

    return 1;
}

/*������� ��  ��������*/
bool Tree::CheckUno(TDataType type_data){
    if(!Tree::FLInt)
        return false;

    if(type_data == TYPE_INTEGER)
        return true;

    if(type_data == TYPE_DOUBLE)
        return true;

    return false;
}

/*��������� ���� ������*/
TDataType Tree::SemGetDataType(Tree *address){
    if(!Tree::FLInt)
        return TYPE_EMPTY;

    return address->node->DataType;
}

/*��������� ������������ ���� ������*/
TDataType Tree::SemGetFunctionReturnType(Tree *address){
    if(!Tree::FLInt)
        return TYPE_EMPTY;

    return  address->node->DataType;
}

/*������� �����*/
void Tree::FreeAfterBlock(Tree *address){
    if(!Tree::FLInt)
        return;

    if (address->left){
        Tree::DeleteAfterBlock(address->left);
        delete address->left;
    }
    address->left = nullptr;
}

/*����������� �������� ������ */
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

/*�������� �������� int*/
int Tree::GetIntValue(Tree* address){
    if(!Tree::FLInt)
        return 0;

    return address->node->DataValue.DataAsInt;
}

/*�������� �������� unsigned*/
unsigned Tree::GetDoubleValue(Tree* address){
    if(!Tree::FLInt)
        return 0;

    return address->node->DataValue.DataAsDouble;
}

/*�������� ��������*/
TDataValue* Tree::GetDataValue(Tree* address){
    if(!Tree::FLInt)
        return nullptr;

    if(address)
        return &address->node->DataValue;

    return new TDataValue();
}

/*��������� ��������*/
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

/*����������� �������*/
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

/*�������� �������*/
void Tree::SemDelExeFunc(Tree *address){
    if(!Tree::FLInt)
        return;

    Tree::DeleteAfterBlock(address->right);

    address->left->parent = address->parent;
    delete address;

    return;
}

/*���������� ��������� �������*/
void Tree::SemSetExeFuncParam(Tree *address, TDataValue value, int pos){
    if(!Tree::FLInt)
        return;

    Tree* local_cur = address->right->left;

    while(--pos)
        local_cur = local_cur->left;

    local_cur->node->DataValue = value;
}

/*��������� ���������� �����*/
Tree* Tree::SemGetBlockStart(Tree * address){
    if(!Tree::FLInt)
        return nullptr;

    Tree* local_cur = address->right->left;

    while(local_cur->left)
        local_cur = local_cur->left;

    return  local_cur;
}

/*����������� �������� �������*/
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

/*������ ������*/
void Tree::Print() {
    if(!Tree::FLInt) return;
        printf("������� � ������� %s ----->", node->Id );
        if (left !=nullptr) printf(" ����� ������ %s", left->node->Id );
        if (right!=nullptr) printf(" ������ ������ %s", right->node->Id );
        printf("\n");
        if (left!=nullptr) left->Print();
        if (right!=nullptr) right->Print();
}

/*����� ������ � ����������*/
void Tree::PrintError(const char *err, char *a) {
    if(!Tree::FLInt)
        return;

    if (a == nullptr)
        printf("�������������� ������ : %s \n",err);
    else
        printf("�������������� ������ : %s. �������� ������ \'%s\' \n",err,a);
}


