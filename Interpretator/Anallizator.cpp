#include "Dictionary.h"
#include "Anallizator.h"
#include "Tree.h"
#include <stdio.h>

/*�����������*/
TAnallizator::TAnallizator(TScanner *scanner, Tree* tree){
    Scanner = scanner;
    SemanticTree  = tree;
}

/*���������*/
void TAnallizator::Programm(){
    Tree::Cur = SemanticTree ;
    Tree::Root = SemanticTree ;
    Tree::FLInt = true;
    Tree::FLRet = false;

    TypeLexem lexem;

    int type_lexem;
    int cur, str, pos;

    // ������ ��������� �����������
    while (true){
        Scanner->GetCurrent(&cur, &str, &pos);
        type_lexem = Scanner->Scanner(lexem);

        // ����� ���������
        if (type_lexem == TEnd)
            return;

        // ����������� ���� ������
        if (type_lexem != TInt && type_lexem != TDouble)
            Scanner->PrintError("�������� ��� ������ (int ��� double)", true);

        // ����������� ��������������� ��� main
        type_lexem = Scanner->Scanner(lexem);
        if (type_lexem != TIdent && type_lexem != TMain)
           Scanner->PrintError("�������� �������������", true);

        // ���� �������
        type_lexem = Scanner->Scanner(lexem);
        if (type_lexem == TParent_O){
           Scanner->SetCurrent(cur, str, pos);
           Function();
        }
        else
            //���� ���, �� �������� ����������
            if (type_lexem == TAssign || type_lexem == TSemicolon || type_lexem == TComma) {
               Scanner->SetCurrent(cur, str, pos);
               VariableDescription();
            }
            // ����� ������ ��� ������������ ����
            else
                Scanner->PrintError("��������� ���� �� (\'=\' \';\' \',\' \'(\')", true);
    }
}

/*�������*/
void TAnallizator::Function(){
    TypeLexem lexem;
    TDataType type_data;

    int type_lexem;
    int lexem_id;
    int cur, pos, str;

    type_lexem = Scanner->Scanner(lexem);

    // ���������� ��� �������
    if(type_lexem == TInt)
        type_data = TYPE_INTEGER;

    if (type_lexem == TDouble)
        type_data = TYPE_DOUBLE;

    // ���������� �������������� �������
    type_lexem = Scanner->Scanner(lexem);
    lexem_id = type_lexem;

    Tree * vertex = SemanticTree ->SemIncludeFunc(lexem, type_data);     // ��������� � ������ �������

    type_lexem = Scanner->Scanner(lexem);                               // (
    type_lexem = Scanner->Scanner(lexem);                               // Ident || )

    // ������� ��������� �������
    int counter = 0;
    while(type_lexem != TParent_C) {

        if (type_lexem != TInt && type_lexem != TDouble)
            Scanner->PrintError("�������� ��� ������ (int ��� double)", true);

        // ���������� ��� ����������
        if(type_lexem == TInt)
            type_data = TYPE_INTEGER;

        if (type_lexem == TDouble)
            type_data = TYPE_DOUBLE;

        // ������� � ������
        type_lexem = Scanner->Scanner(lexem);
        SemanticTree ->SemIncludePar(lexem, type_data);
        counter++;

        type_lexem = Scanner->Scanner(lexem);
        if (type_lexem != TComma && type_lexem != TParent_C)
            Scanner->PrintError("�������� ) ��� ,", true);

        if (type_lexem == TComma)
            type_lexem = Scanner->Scanner(lexem);
    }

    SemanticTree->SemSetParamCount(vertex, counter); // ��������� ����� ����������

    // ���������
    Scanner->GetCurrent(&cur, &pos, &str);
    vertex->SetPosInText(cur, pos, str);

    // ���� ��� ������� �������, ��������� block
    if (lexem_id == TMain) {
        Block();
    }
    // ���� ��� ������� �������, �������� ����� ����
    else {
        Tree::FLInt = false;
        Block();
        Tree::FLInt = true;
    }

    SemanticTree->SetCur(vertex);
    return;
}

/*�������� ����������*/
void TAnallizator::VariableDescription(){
    TypeLexem lexem;
    TypeLexem lexem_name;
    TDataType type_data;
    int type_lexem;

    // ���������� ��� ����������
    type_lexem = Scanner->Scanner(lexem);

    if(type_lexem == TInt)
        type_data = TYPE_INTEGER;

    if (type_lexem == TDouble)
        type_data = TYPE_DOUBLE;

    do {
        //���������� id
        type_lexem = Scanner->Scanner(lexem);
        memcpy(lexem_name, lexem, sizeof(lexem));

        if (type_lexem != TIdent)
            Scanner->PrintError("�������� �������������", true);

        Tree* vertex = SemanticTree ->SemIncludePar(lexem, type_data); // �������

        // ����������, ������������ ��� ���
        type_lexem = Scanner->Scanner(lexem);
        if (type_lexem == TAssign){
            TDataType expression_type;
            TDataValue data_value;

            //�������� ������������
            A1(&expression_type, &data_value);

            switch(type_data){
                case TYPE_DOUBLE:{
                    if(data_value.DataAsInt)
                        data_value.DataAsDouble = data_value.DataAsInt;
                    else
                        data_value.DataAsDouble = data_value.DataAsDouble;
                    break;
                }
                case TYPE_INTEGER: {
                    if(data_value.DataAsInt)
                        data_value.DataAsInt = data_value.DataAsInt;
                    else
                        data_value.DataAsInt = data_value.DataAsDouble;
                    break;
                }
                default: break;
            }

            //������� �������� ���������� � ������ �����
            SemanticTree ->SetVarValue(vertex, data_value);
            logAssing(lexem_name, type_data, data_value);

            type_lexem = Scanner->Scanner(lexem);
        }

    } while (type_lexem == TComma); // ��������� ���� ,

    // ������� ;
    if (type_lexem != TSemicolon)
        Scanner->PrintError("��������� ; ", true);
}

/*����*/
void TAnallizator::Block(){
    TypeLexem lexem;
    int type_lexem;

    int cur, str, pos;

    // ������� � ������
    type_lexem = Scanner->Scanner(lexem);
    SemanticTree ->AddLeft();

    Tree * vertex = SemanticTree ->GetCur();
    SemanticTree ->AddRight();

    // ������� {
    if (type_lexem != TBrace_O)
        Scanner->PrintError("��������� { ", true);

    Scanner->GetCurrent(&cur, &str, &pos);
    type_lexem = Scanner->Scanner(lexem);

    // ���� } ����� �����
    if(type_lexem == TBrace_C)
        return;

    // ���� �� }, �� �������� ������
    while (true) {

        // ���������� ����������
        Scanner->SetCurrent(cur, str, pos);
        type_lexem = Scanner->Scanner(lexem);
        if (type_lexem == TInt || type_lexem == TDouble){
            Scanner->SetCurrent(cur, str, pos);
            VariableDescription();
        }
        // ���� ���, �� ���� � ���������
        else {
            Scanner->SetCurrent(cur, str, pos);
            Operator();
        }

        // ���� }
        Scanner->GetCurrent(&cur, &str, &pos);
        type_lexem = Scanner->Scanner(lexem);
        if(type_lexem == TBrace_C) {
            Tree * newCur = SemanticTree ->GetUp(SemanticTree ->GetUp(vertex));
            SemanticTree ->FreeAfterBlock(newCur);
            SemanticTree ->SetCur(newCur);
            return;
        }

        // ����� ���������
        if(type_lexem == TEnd)
            break;
    }

    Scanner->PrintError("��������� } ", true);
}

/*��������*/
void TAnallizator::Operator(){
    TypeLexem lexem;
    int type_lexem;
    int cur, str, pos;

    TDataType expression_type;
    TDataValue *data_value;

    // ������� return
    Scanner->GetCurrent(&cur, &str, &pos);
    type_lexem = Scanner->Scanner(lexem);
    if (type_lexem == TReturn){
        Scanner->SetCurrent(cur, str, pos);
        Return();
    }
    else
        // ���� {  � ����
        if (type_lexem == TBrace_O){
            Scanner->SetCurrent(cur, str, pos);
            Block();
        }
        else
            // ���� while � while
            if (type_lexem == TWhile) {
                Scanner->SetCurrent(cur, str, pos);
                While();
            }
            else
                //���� ��������������
                if (type_lexem == TIdent){
                    Tree* vertex = SemanticTree->FindUp(lexem); // �������

                    //���� �� null � ����� ���� ��������������
                    if (vertex == nullptr && Tree::FLInt) {
                        type_lexem = Scanner->Scanner(lexem);

                        // ���� �������� ( ��� ���, �� ������
                        if (type_lexem == TParent_O)
                            SemanticTree ->PrintError("������������� �������", nullptr);
                        else
                            SemanticTree ->PrintError("������������� ����������", nullptr);

                        Scanner->PrintError();
                    }

                    data_value = SemanticTree ->GetDataValue(vertex);
                    type_lexem = Scanner->Scanner(lexem);

                    // ���� ������������
                    if (type_lexem == TAssign){
                        Scanner->SetCurrent(cur, str, pos);
                        Assign();
                    }
                    else {
                        Scanner->SetCurrent(cur, str, pos);

                        A5(&expression_type, data_value);

                        type_lexem = Scanner->Scanner(lexem);

                        // ������� ;
                        if (type_lexem != TSemicolon)
                            Scanner->PrintError("��������� ; ", true);
                    }
                }
                else
                    // ���� ,
                    if (type_lexem == TSemicolon){
                        Scanner->SetCurrent(cur, str, pos);
                        type_lexem = Scanner->Scanner(lexem);
                    }
                    else {
                        data_value = new TDataValue();
                        Scanner->SetCurrent(cur, str, pos);

                        A5(&expression_type,data_value);

                        type_lexem = Scanner->Scanner(lexem);

                        // ������� ;
                        if (type_lexem != TSemicolon)
                            Scanner->PrintError("��������� ; ", true);
                    }
}

/*�������*/
void TAnallizator::Return(){
    TypeLexem lexem;
    int type_lexem;

    type_lexem = Scanner->Scanner(lexem); // return

    TDataType expression_type;
    TDataValue data_value;

    A1(&expression_type, &data_value);

    SemanticTree ->SemFuncReturnValue(data_value);
    type_lexem = Scanner->Scanner(lexem);

    if (type_lexem != TSemicolon)
        Scanner->PrintError("��������� ;", true);

    // ���� ���� ������������� true, �� ���� �������� true
    if(Tree::FLInt == true)
        Tree::FLRet = true;

    Tree::FLInt = false;
}

/*�������� While*/
void TAnallizator::While(){
    bool localFlInt = Tree::FLInt;

    TypeLexem lexem;
    int type_lexem;
    int cur, str, pos;

    // while
    type_lexem = Scanner->Scanner(lexem);
    Scanner->GetCurrent(&cur, &str, &pos);

Start:
    type_lexem = Scanner->Scanner(lexem);

    // ���� �� (, ������
    if (type_lexem != TParent_O)
        Scanner->PrintError("��������� (", true);

    TDataType expression_type;
    TDataValue data_value;

    A1(&expression_type, &data_value);

    // ���� ���� ������ true � ���� ��������, �� ������ true
    if(Tree::FLInt && (data_value.DataAsInt != 0))
        Tree::FLInt = true;
    else
        Tree::FLInt = false;

    type_lexem = Scanner->Scanner(lexem);
    if (type_lexem != TParent_C)
        Scanner->PrintError("��������� )", true);

    // ���� � ��������
    Operator();

    // ����������� ����� ���� ���� true
    if(Tree::FLInt){
        Scanner->SetCurrent(cur, str, pos);
        goto Start;
    }

    Tree::FLInt = localFlInt;

    if (Tree::FLRet)
        Tree::FLInt = false;

    return;
}

/*������������*/
void TAnallizator::Assign(){
    TypeLexem lexem;
    TypeLexem lexem_name;
    int type_lexem;

    Tree *vertex;
    TDataType VType, // ������ ����� ���������
            EType; // ������ ����� ���������

    TDataValue* data_value = new TDataValue();

    // ��������������
    type_lexem = Scanner->Scanner(lexem);
    memcpy(lexem_name, lexem, sizeof (lexem));

    // �������� ��� ������� � ��� ������
    vertex = SemanticTree->SemGetType(lexem);
    VType = SemanticTree->SemGetDataType(vertex);

    // =
    type_lexem = Scanner->Scanner(lexem);

    A1(&EType, data_value);

    switch(VType){
        case TYPE_INTEGER: {
            switch(EType){
                case TYPE_INTEGER:{
                    data_value->DataAsInt = data_value->DataAsInt;
                    break;
                }
                case TYPE_DOUBLE:{
                    data_value->DataAsInt = data_value->DataAsDouble;
                    break;
                }
                default: break;
            }
            break;
         }
         case TYPE_DOUBLE: {
            switch(EType){
                case TYPE_INTEGER:{
                    data_value->DataAsDouble = data_value->DataAsInt;
                    break;
                }
                case TYPE_DOUBLE:{
                    data_value->DataAsDouble = data_value->DataAsDouble;
                    break;
                }
                default: break;
            }
            break;
        }
        default: break;
    }

    SemanticTree->SetVarValue(vertex, *data_value); // ��������
    logAssing(lexem_name, VType, *data_value);

    // �������� �� ;
    type_lexem = Scanner->Scanner(lexem);
    if (type_lexem != TSemicolon)
        Scanner->PrintError("��������� ;", true);

    return;
}

/*����� �������*/
void TAnallizator::CallFunction(TDataValue* data_value){
    TypeLexem lexem;
    int type_lexem;
    int cur_f, str_f, pos_f;
    int cur_s, str_s, pos_s;
    int count;

    // �������������
    type_lexem = Scanner->Scanner(lexem);

    Tree *vertex = SemanticTree ->SemGetFunct(lexem); // ��������

    type_lexem = Scanner->Scanner(lexem); // (
    count = 0;

    Scanner->GetCurrent(&cur_f, &str_f, &pos_f);
    type_lexem = Scanner->Scanner(lexem);

    // ���� )
    if (type_lexem != TParent_C)
        Scanner->SetCurrent(cur_f, str_f, pos_f);

    Tree * exeFunc = SemanticTree->SemCopyFunc(vertex); // ��������

    // ���� �� ������ )
    while(type_lexem != TParent_C) {
        count++;
        TDataType expression_type;
        TDataValue data_value;

        A1(&expression_type, &data_value);

        SemanticTree ->SemSetExeFuncParam(exeFunc, data_value,count);

        // ������� , ��� )
        type_lexem = Scanner->Scanner(lexem);
        if (type_lexem != TComma && type_lexem != TParent_C)
            Scanner->PrintError("��������� , ��� )", true);
    }

    // ���������� ��� ����������
    SemanticTree ->SemParamEqual(vertex, count);
    Tree *cur_tree = nullptr;
    Tree *start_tree = nullptr;

    Scanner->GetCurrent(&cur_s, &str_s, &pos_s);

    // ���������� �� �������
    if (SemanticTree ->FLRet == false) {
        if (SemanticTree ->FLInt ){
            Scanner->GetCurrent(&cur_s, &str_s, &pos_s);
            exeFunc->GetPosInText(&cur_f, &str_f, &pos_f);
            Scanner->SetCurrent(cur_f, pos_f, str_f);

            cur_tree = SemanticTree ->GetCur();
            start_tree = SemanticTree ->SemGetBlockStart(exeFunc);

            SemanticTree ->SetCur(start_tree);
            Block();
        }

        // ���������� ��������
        if (SemanticTree ->FLInt == false  &&  SemanticTree ->FLRet == true) {
            SemanticTree ->FLInt = true;
            SemanticTree ->FLRet = false;

            data_value->DataAsInt = SemanticTree ->GetDataValue(exeFunc)->DataAsInt;

            SemanticTree ->SetCur(cur_tree);
            Scanner->SetCurrent(cur_s, str_s, pos_s);
            SemanticTree ->SemDelExeFunc(exeFunc);
        }
    }
}

/*���������*/
void TAnallizator::A1(TDataType * data_type, TDataValue* data_value){
    TDataType VType, EType;
    TDataValue* value = new TDataValue();

    // �������� �������� ������ �����
    A2(&VType, data_value);

    TypeLexem lexem;
    int operation;
    int cur, str, pos;

    Scanner->GetCurrent(&cur, &str, &pos);
    operation = Scanner->Scanner(lexem);

    // ���� ��� ����� �� �������� ���������, �� �����
    if ((operation != TGreat && operation != TLess && operation != TGreat_EQ && operation != TLess_EQ && operation != TEQ && operation != TNot_EQ)){
        Scanner->SetCurrent(cur, str, pos);
        *data_type = VType;
        return;
    }

    // ���� ����, �� ���������
    while(true){
        // �������� �������� ������ �����
        A2(&EType, value);

        // ����������
        if (Tree::FLInt){
            switch (VType) {
            case TYPE_INTEGER:
                switch (EType) {
                case TYPE_INTEGER:
                    switch (operation) {
                        case TGreat:     {
                           data_value->DataAsInt = data_value->DataAsInt > value->DataAsInt;
                           logChange(nullptr,"��������� \'������\' (>)", VType, *data_value);
                           break;}
                        case TLess:    {
                           data_value->DataAsInt = data_value->DataAsInt < value->DataAsInt;
                           logChange(nullptr,"��������� \'������\' (<)", VType, *data_value);
                           break;}
                        case TGreat_EQ:     {
                           data_value->DataAsInt = data_value->DataAsInt >= value->DataAsInt;
                           logChange(nullptr,"��������� \'������ ��� �����\' (>=)", VType, *data_value);
                           break;}
                        case TLess_EQ:    {
                           data_value->DataAsInt = data_value->DataAsInt <= value->DataAsInt;
                           logChange(nullptr,"��������� \'������ ��� �����\' (<=)", VType, *data_value);
                           break;}
                        case TEQ:     {
                           data_value->DataAsInt = data_value->DataAsInt == value->DataAsInt;
                           logChange(nullptr,"��������� \'�����\' (==)", VType, *data_value);
                           break;}
                        case TNot_EQ:    {
                           data_value->DataAsInt = data_value->DataAsInt != value->DataAsInt;
                           logChange(nullptr,"��������� \'�� �����\' (!=)", VType, *data_value);
                           break;}
                    }
                    break;
                case TYPE_DOUBLE:
                    switch (operation) {
                        case TGreat:     {
                           data_value->DataAsInt = data_value->DataAsInt > value->DataAsDouble;
                           logChange(nullptr,"��������� \'������\' (>)", VType, *data_value);
                           break;}
                        case TLess:    {
                           data_value->DataAsInt = data_value->DataAsInt < value->DataAsDouble;
                           logChange(nullptr,"��������� \'������\' (<)", VType, *data_value);
                           break;}
                        case TGreat_EQ:     {
                           data_value->DataAsInt = data_value->DataAsInt >= value->DataAsDouble;
                           logChange(nullptr,"��������� \'������ ��� �����\' (>=)", VType, *data_value);
                           break;}
                        case TLess_EQ:    {
                           data_value->DataAsInt = data_value->DataAsInt <= value->DataAsDouble;
                           logChange(nullptr,"��������� \'������ ��� �����\' (<=)", VType, *data_value);
                           break;}
                        case TEQ:     {
                           data_value->DataAsInt = data_value->DataAsInt == value->DataAsDouble;
                           logChange(nullptr,"��������� \'�����\' (==)", VType, *data_value);
                           break;}
                        case TNot_EQ:    {
                           data_value->DataAsInt = data_value->DataAsInt != value->DataAsDouble;
                           logChange(nullptr,"��������� \'�� �����\' (!=)", VType, *data_value);
                           break;}
                    }
                    break;
                default:break;
                }
                break;
            case TYPE_DOUBLE:
                switch (EType) {
                case TYPE_INTEGER:
                    switch (operation) {
                        case TGreat:     {
                           data_value->DataAsInt = data_value->DataAsDouble > value->DataAsInt;
                           logChange(nullptr,"��������� \'������\' (>)", VType, *data_value);
                           break;}
                        case TLess:    {
                           data_value->DataAsInt = data_value->DataAsDouble < value->DataAsInt;
                           logChange(nullptr,"��������� \'������\' (<)", VType, *data_value);
                           break;}
                        case TGreat_EQ:     {
                           data_value->DataAsInt = data_value->DataAsDouble >= value->DataAsInt;
                           logChange(nullptr,"��������� \'������ ��� �����\' (>=)", VType, *data_value);
                           break;}
                        case TLess_EQ:    {
                           data_value->DataAsInt = data_value->DataAsDouble <= value->DataAsInt;
                           logChange(nullptr,"��������� \'������ ��� �����\' (<=)", VType, *data_value);
                           break;}
                        case TEQ:     {
                           data_value->DataAsInt = data_value->DataAsDouble == value->DataAsInt;
                           logChange(nullptr,"��������� \'�����\' (==)", VType, *data_value);
                           break;}
                        case TNot_EQ:    {
                           data_value->DataAsInt = data_value->DataAsDouble != value->DataAsInt;
                           logChange(nullptr,"��������� \'�� �����\' (!=)", VType, *data_value);
                           break;}
                    }
                    break;
                case TYPE_DOUBLE:
                    switch (operation) {
                        case TGreat:     {
                           data_value->DataAsInt = data_value->DataAsDouble > value->DataAsDouble;
                           logChange(nullptr,"��������� \'������\' (>)", VType, *data_value);
                           break;}
                        case TLess:    {
                           data_value->DataAsInt = data_value->DataAsDouble < value->DataAsDouble;
                           logChange(nullptr,"��������� \'������\' (<)", VType, *data_value);
                           break;}
                        case TGreat_EQ:     {
                           data_value->DataAsInt = data_value->DataAsDouble >= value->DataAsDouble;
                           logChange(nullptr,"��������� \'������ ��� �����\' (>=)", VType, *data_value);
                           break;}
                        case TLess_EQ:    {
                           data_value->DataAsInt = data_value->DataAsDouble <= value->DataAsDouble;
                           logChange(nullptr,"��������� \'������ ��� �����\' (<=)", VType, *data_value);
                           break;}
                        case TEQ:     {
                           data_value->DataAsInt = data_value->DataAsDouble == value->DataAsDouble;
                           logChange(nullptr,"��������� \'�����\' (==)", VType, *data_value);
                           break;}
                        case TNot_EQ:    {
                           data_value->DataAsInt = data_value->DataAsDouble != value->DataAsDouble;
                           logChange(nullptr,"��������� \'�� �����\' (!=)", VType, *data_value);
                           break;}
                    }
                    break;
                default:break;
                }
                break;
            default:break;
            }

        }

        Scanner->GetCurrent(&cur, &str, &pos);
        operation = Scanner->Scanner(lexem);

        if ((operation != TGreat && operation != TLess && operation != TGreat_EQ && operation != TLess_EQ && operation != TEQ && operation != TNot_EQ)){
            Scanner->SetCurrent(cur,str,pos);
            *data_type = VType;
            return;
        }
    }
}

/*������*/
void TAnallizator::A2(TDataType * data_type, TDataValue* data_value){
    TDataType VType, EType;
    TDataValue* value = new TDataValue();

    A3(&VType, data_value);

    TypeLexem lexem;
    int type_lexem;
    int cur, str, pos;

    Scanner->GetCurrent(&cur, &str, &pos);
    type_lexem = Scanner->Scanner(lexem);

    // ���� �� ������, �����
    if (type_lexem != TShift_L && type_lexem != TShift_R){
        Scanner->SetCurrent(cur,str,pos);
        *data_type = VType;
        return;
    }

    // ����� ��������
    while(true){
        A3(&EType,value);

        if (Tree::FLInt){
            if(VType == TYPE_DOUBLE){
                SemanticTree ->PrintError("�������� ������ ���������", nullptr);
                Scanner->PrintError();
            }
            else{
                switch (type_lexem) {
                case TShift_L: {
                   data_value->DataAsInt = data_value->DataAsInt << value->DataAsInt;
                   logChange(nullptr,"����� ����� (<<)", VType, *data_value);
                   break;
                }
                case TShift_R:    {
                   data_value->DataAsInt = data_value->DataAsInt >> value->DataAsInt;
                   logChange(nullptr,"����� ������ (>>))", VType, *data_value);
                   break;
                }
                }
            }
        }

        Scanner->GetCurrent(&cur, &str, &pos);
        type_lexem = Scanner->Scanner(lexem);

        if (type_lexem != TShift_L && type_lexem != TShift_R){
            Scanner->SetCurrent(cur,str,pos);
            *data_type = VType;
            return;
        }
    }
}

/*�������� � ���������*/
void TAnallizator::A3(TDataType * data_type, TDataValue* data_value){
    TDataType VType, EType;
    TDataValue* value = new TDataValue();

    A4(&VType, data_value);

    TypeLexem lexem;
    int type_lexem;
    int cur, str, pos;

    Scanner->GetCurrent(&cur, &str, &pos);
    type_lexem = Scanner->Scanner(lexem);

    // ���� �� + -, �����
    if (type_lexem != TPlus && type_lexem != TMinus){
        Scanner->SetCurrent(cur,str,pos);
        *data_type = VType;
        return;
    }

    // ����� ��������
    while(true){
        A4(&EType,value);

        if (Tree::FLInt){
            switch(VType){
                case TYPE_INTEGER: {
                    switch(EType){
                        case TYPE_INTEGER:{
                            switch (type_lexem) {
                                case TPlus: {
                                    data_value->DataAsInt = data_value->DataAsInt + value->DataAsInt;
                                    logChange(nullptr,"�������� (+)", VType, *data_value);
                                    break;
                                }
                                case TMinus: {
                                    data_value->DataAsInt = data_value->DataAsInt - value->DataAsInt;
                                    logChange(nullptr,"��������� (-))", VType, *data_value);
                                    break;
                                }
                                default: break;
                            }
                            break;
                        }
                        case TYPE_DOUBLE:{
                            switch (type_lexem) {
                                case TPlus: {
                                    data_value->DataAsInt = data_value->DataAsInt + value->DataAsDouble;
                                    logChange(nullptr,"�������� (+)", VType, *data_value);
                                    break;
                                }
                                case TMinus: {
                                    data_value->DataAsInt = data_value->DataAsInt - value->DataAsDouble;
                                    logChange(nullptr,"��������� (-))", VType, *data_value);
                                    break;
                                }
                                default: break;
                            }
                            break;
                        }
                        default: break;
                    }
                     break;
                }
                case TYPE_DOUBLE: {
                    switch(EType){
                        case TYPE_INTEGER:{
                            switch (type_lexem) {
                                case TPlus: {
                                    data_value->DataAsDouble = data_value->DataAsDouble + value->DataAsDouble;
                                    logChange(nullptr,"�������� (+)", VType, *data_value);
                                    break;
                                }
                                case TMinus: {
                                    data_value->DataAsDouble = data_value->DataAsDouble - value->DataAsDouble;
                                    logChange(nullptr,"��������� (-))", VType, *data_value);
                                    break;
                                }
                                default: break;
                            }
                            break;
                        }
                        case TYPE_DOUBLE:{
                            switch (type_lexem) {
                                case TPlus: {
                                    data_value->DataAsDouble = data_value->DataAsDouble + value->DataAsDouble;
                                    logChange(nullptr,"�������� (+)", VType, *data_value);
                                    break;
                                }
                                case TMinus: {
                                    data_value->DataAsDouble = data_value->DataAsDouble - value->DataAsDouble;
                                    logChange(nullptr,"��������� (-))", VType, *data_value);
                                    break;
                                }
                                default: break;
                            }
                             break;
                        }
                        default: break;
                    }
                     break;
                }
                default: break;
            }
        }


        Scanner->GetCurrent(&cur, &str, &pos);
        type_lexem = Scanner->Scanner(lexem);

        if (type_lexem != TPlus && type_lexem != TMinus){
            Scanner->SetCurrent(cur,str,pos);
            *data_type = VType;
            return;
        }
    }
}

/*���������, �������, ������� ������*/
void TAnallizator::A4(TDataType * data_type, TDataValue* data_value){
    TDataType VType, EType;
    TDataValue* value = new TDataValue();

    A5(&VType, data_value);

    TypeLexem lexem;
    int type_lexem;
    int cur, str, pos;

    Scanner->GetCurrent(&cur, &str, &pos);
    type_lexem = Scanner->Scanner(lexem);

    // ���� �� / % *, �����
    if (type_lexem != TStar && type_lexem != TRSlash && type_lexem != TPercent){
        Scanner->SetCurrent(cur,str,pos);
        *data_type = VType;
        return;
    }

    // ����� ��������
    while(true){
        A5(&EType,value);

        if (Tree::FLInt){
            switch(VType){
                case TYPE_INTEGER: {
                    switch(EType){
                        case TYPE_INTEGER:{
                            switch (type_lexem) {
                                case TStar:     {
                                    data_value->DataAsInt = data_value->DataAsInt * value->DataAsInt;
                                    logChange(nullptr,"��������� (*)", VType, *data_value);
                                    break;
                                }
                                case TRSlash: {
                                    data_value->DataAsInt = data_value->DataAsInt / value->DataAsInt;
                                    logChange(nullptr,"������� ������ (/))", VType, *data_value);
                                    break;
                                }
                                case TPercent: {
                                    data_value->DataAsInt = data_value->DataAsInt % value->DataAsInt;
                                    logChange(nullptr,"������� �� ������� (%))", VType, *data_value);
                                    break;
                                }
                                default: break;
                            }
                            break;
                        }
                        case TYPE_DOUBLE:{
                            switch (type_lexem) {
                                case TStar:     {
                                    data_value->DataAsInt = data_value->DataAsInt * value->DataAsDouble;
                                    logChange(nullptr,"��������� (*)", VType, *data_value);
                                    break;
                                }
                                case TRSlash: {
                                    data_value->DataAsInt = data_value->DataAsInt / value->DataAsDouble;
                                    logChange(nullptr,"������� ������ (/))", VType, *data_value);
                                    break;
                                }
                                case TPercent: {
                                    SemanticTree ->PrintError("�������� ������� �� ������� ���������", nullptr);
                                    Scanner->PrintError();
                                    break;
                                }
                                default: break;
                            }
                            break;
                        }
                        default: break;
                    }
                     break;
                }
                case TYPE_DOUBLE: {
                    switch(EType){
                        case TYPE_INTEGER:{
                            switch (type_lexem) {
                                case TStar:     {
                                    data_value->DataAsDouble = data_value->DataAsDouble * value->DataAsDouble;
                                    logChange(nullptr,"��������� (*)", VType, *data_value);
                                    break;
                                }
                                case TRSlash: {
                                    data_value->DataAsDouble = data_value->DataAsDouble / value->DataAsDouble;
                                    logChange(nullptr,"������� ������ (/))", VType, *data_value);
                                    break;
                                }
                                case TPercent: {
                                    SemanticTree ->PrintError("�������� ������� �� ������� ���������", nullptr);
                                    Scanner->PrintError();
                                    break;
                                }
                                default: break;
                            }
                            break;
                        }
                        case TYPE_DOUBLE:{
                            switch (type_lexem) {
                                case TStar:     {
                                    data_value->DataAsDouble = data_value->DataAsDouble * value->DataAsDouble;
                                    logChange(nullptr,"��������� (*)", VType, *data_value);
                                    break;
                                }
                                case TRSlash: {
                                    data_value->DataAsDouble = data_value->DataAsDouble / value->DataAsDouble;
                                    logChange(nullptr,"������� ������ (/))", VType, *data_value);
                                    break;
                                }
                                case TPercent: {
                                    SemanticTree ->PrintError("�������� ������� �� ������� ���������", nullptr);
                                    Scanner->PrintError();
                                    break;
                                }
                                default: break;
                            }
                             break;
                        }
                        default: break;
                    }
                     break;
                }
                default: break;
            }
        }

        Scanner->GetCurrent(&cur, &str, &pos);
        type_lexem = Scanner->Scanner(lexem);

        if (type_lexem != TStar && type_lexem != TRSlash && type_lexem != TPercent){
            Scanner->SetCurrent(cur,str,pos);
            *data_type = VType;
            return;
        }
    }
}

/*������� �������� ��������*/
void TAnallizator::A5(TDataType * data_type, TDataValue* data_value){
    TDataType VType;

    TypeLexem lexem;
    int type_lexem;
    int cur, str, pos;

    A6(&VType, data_value);

    Scanner->GetCurrent(&cur, &str, &pos);
    type_lexem = Scanner->Scanner(lexem);

    if (type_lexem == TDPlus || type_lexem == TDMinus){
        if (Tree::FLInt){
            switch(VType){
                case TYPE_INTEGER: {
                    switch (type_lexem) {
                        case TDPlus:     {
                            data_value->DataAsInt = data_value->DataAsInt + 1;
                            logChange(nullptr,"������� ������� ����(��������)", VType, *data_value);
                            break;
                        }
                        case TDMinus:    {
                            data_value->DataAsInt = data_value->DataAsInt - 1;
                            logChange(nullptr,"������� ������� �����(��������)", VType, *data_value);
                            break;
                        }
                        default: break;
                    }
                    break;
                }
                case TYPE_DOUBLE: {
                    switch (type_lexem) {
                        case TDPlus:     {
                            data_value->DataAsDouble = data_value->DataAsDouble + 1;
                            logChange(nullptr,"������� ������� ����(��������)", VType, *data_value);
                            break;
                        }
                        case TDMinus:    {
                            data_value->DataAsDouble = data_value->DataAsDouble - 1;
                            logChange(nullptr,"������� ������� �����(��������)", VType, *data_value);
                            break;
                        }
                        default: break;
                    }
                    break;
                }
                default: break;
            }
        }
    }
    else
        Scanner->SetCurrent(cur, str, pos);

    *data_type = VType;
}

/*������� �������� �������*/
void TAnallizator::A6(TDataType * data_type, TDataValue* data_value){
    TDataType VType;

    TypeLexem lexem;
    int type_lexem;
    int cur, str, pos;

    Scanner->GetCurrent(&cur, &str, &pos);
    type_lexem = Scanner->Scanner(lexem);

    bool flag_op = false;
    if (type_lexem == TPlus || type_lexem == TDPlus || type_lexem == TMinus || type_lexem == TDMinus)
        flag_op = true;
    else
        Scanner->SetCurrent(cur, str, pos);

    A7(&VType, data_value);

    if (flag_op){  
        if (Tree::FLInt){
            switch(VType){
                case TYPE_INTEGER: {
                    switch (type_lexem) {
                        case TPlus:     {
                            data_value->DataAsInt = + data_value->DataAsInt;
                            logChange(nullptr,"������� ����", VType, *data_value);
                            break;
                        }
                        case TDPlus:    {
                            data_value->DataAsInt = data_value->DataAsInt + 1;
                            logChange(nullptr,"������� ������� ����(�������)", VType, *data_value);
                            break;
                        }
                        case TMinus:    {
                            data_value->DataAsInt = - data_value->DataAsInt;
                            logChange(nullptr,"������� �����", VType, *data_value);
                            break;
                        }
                        case TDMinus:   {
                            data_value->DataAsInt = data_value->DataAsInt - 1;
                            logChange(nullptr,"������� ������� �����(�������)", VType, *data_value);
                            break;
                        }
                        default: break;
                    }
                    break;
                }
                case TYPE_DOUBLE: {
                    switch (type_lexem) {
                        case TPlus:     {
                            data_value->DataAsDouble = + data_value->DataAsDouble;
                            logChange(nullptr,"������� ����", VType, *data_value);
                            break;
                        }
                        case TDPlus:    {
                            data_value->DataAsDouble = data_value->DataAsDouble + 1;
                            logChange(nullptr,"������� ������� ����(�������)", VType, *data_value);
                            break;
                        }
                        case TMinus:    {
                            data_value->DataAsDouble = - data_value->DataAsDouble;
                            logChange(nullptr,"������� �����", VType, *data_value);
                            break;
                        }
                        case TDMinus:   {
                            data_value->DataAsDouble = data_value->DataAsDouble - 1;
                            logChange(nullptr,"������� ������� �����(�������)", VType, *data_value);
                            break;
                        }
                        default: break;
                    }
                    break;
                }
                default: break;
            }
        }
    }
    *data_type = VType;
}

void TAnallizator::A7(TDataType * data_type, TDataValue* data_value){
    TDataType VType;

    TypeLexem lexem;
    int type_lexem;
    int cur, str, pos;

    Scanner->GetCurrent(&cur, &str, &pos);
    type_lexem = Scanner->Scanner(lexem);

    // ������� �������� ��������
    if (type_lexem == TConstInt_10 || type_lexem == TConstInt_8 || type_lexem == TConstInt_16 || type_lexem == TConstChar){
        switch (*data_type) {
            case TYPE_DOUBLE:{
                if (type_lexem == TConstChar){
                    *data_type = TYPE_CHAR;
                    char ch = lexem[0];
                    data_value->DataAsDouble = static_cast<double>(ch);
                }
                else{
                    *data_type = TYPE_INTEGER;
                    if (type_lexem == TConstInt_10)
                        data_value->DataAsDouble = static_cast<double>(strtoul(lexem,nullptr,10));
                    if (type_lexem == TConstInt_8)
                        data_value->DataAsDouble = static_cast<double>(strtoul(lexem,nullptr,8));
                    if (type_lexem == TConstInt_16)
                        data_value->DataAsDouble = static_cast<double>(strtoul(lexem,nullptr,16));
                }
                break;
            }
            default: {
                if (type_lexem == TConstChar){
                    *data_type = TYPE_CHAR;
                    char ch = lexem[0];
                    data_value->DataAsInt = static_cast<int>(ch);
                }
                else{
                    *data_type = TYPE_INTEGER;
                    if (type_lexem == TConstInt_10)
                        data_value->DataAsInt = static_cast<int>(strtoul(lexem,nullptr,10));
                    if (type_lexem == TConstInt_8)
                        data_value->DataAsInt = static_cast<int>(strtoul(lexem,nullptr,8));
                    if (type_lexem == TConstInt_16)
                        data_value->DataAsInt = static_cast<int>(strtoul(lexem,nullptr,16));
                }
                break;
            }
        }
    }
    else
        // )
        if(type_lexem == TParent_O){
            A1(&VType,data_value);
            type_lexem = Scanner->Scanner(lexem);

            if (type_lexem != TParent_C)
                Scanner->PrintError("��������� )", true);

            *data_type = VType;
        }
        else
            // id
            if (type_lexem == TIdent) {
                Tree * v = SemanticTree ->Cur->FindUp(lexem);

                if (v==nullptr && Tree::FLInt){
                    type_lexem = Scanner->Scanner(lexem);

                    if (type_lexem == TParent_O)
                        SemanticTree ->PrintError("������������� �������", nullptr);
                    else
                        SemanticTree ->PrintError("������������� ����������", nullptr);

                    Scanner->PrintError();
                }

                type_lexem = Scanner->Scanner(lexem);

                if (type_lexem == TParent_O) {
                    Scanner->SetCurrent(cur, str,pos);
                    CallFunction(data_value);
                    VType = SemanticTree ->SemGetFunctionReturnType(v);
                    *data_type = VType;
                }
                else {
                    Scanner->SetCurrent(cur,str,pos);

                    type_lexem = Scanner->Scanner(lexem);
                    VType = SemanticTree ->SemGetDataType(v);
                    *data_type = VType;

                    if(VType == TYPE_INTEGER)
                        data_value->DataAsInt = SemanticTree ->GetIntValue(v);

                    if(VType == TYPE_DOUBLE)
                        data_value->DataAsDouble = SemanticTree ->GetDoubleValue(v);
                }
            }
        else
            Scanner->PrintError("��������� ���� �� (�������������, ���������, ����� �������, ��������� � �������)", true);
}

/*����� ������� ������������*/
void TAnallizator::logAssing(char * id, TDataType type, TDataValue value){
    if(!Tree::FLInt) return;
    switch (type) {
    case TYPE_INTEGER:  {printf("���������� \'%s\' \t���� int \t��������� �������� %d \n",id,value.DataAsInt); break;}
    case TYPE_DOUBLE: {printf("���������� \'%s\' \t���� double \t��������� �������� %lf \n",id, value.DataAsDouble); break;}
    default:
        break;
    }
}

/*����� ���������*/
void TAnallizator::logChange(char * id,const char * op,TDataType type, TDataValue value){
    if(!Tree::FLInt) return;
    if(id)
        printf("���������� \'%s\'",id);
    else
        printf("��� ����������");
    switch (type) {
    case TYPE_INTEGER:  {printf("\t���� int \t��� ���������� %s \t �������� �������� %d \n",op,value.DataAsInt); break;}
    case TYPE_DOUBLE: {printf("\t���� double \t��� ���������� %s \t �������� �������� %lf \n",op, value.DataAsDouble); break;}
    default:
        break;
    }
}




