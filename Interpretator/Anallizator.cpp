#include "Dictionary.h"
#include "Anallizator.h"
#include "Tree.h"
#include <stdio.h>

/*Конструктор*/
TAnallizator::TAnallizator(TScanner *scanner, Tree* tree){
    Scanner = scanner;
    SemanticTree  = tree;
}

/*Программа*/
void TAnallizator::Programm(){
    Tree::Cur = SemanticTree ;
    Tree::Root = SemanticTree ;
    Tree::FLInt = true;
    Tree::FLRet = false;

    TypeLexem lexem;

    int type_lexem;
    int cur, str, pos;

    // запуск программы анализатора
    while (true){
        Scanner->GetCurrent(&cur, &str, &pos);
        type_lexem = Scanner->Scanner(lexem);

        // конец программы
        if (type_lexem == TEnd)
            return;

        // определение типа данных
        if (type_lexem != TInt && type_lexem != TDouble)
            Scanner->PrintError("Ожидался тип данных (int или double)", true);

        // определение индентификатора или main
        type_lexem = Scanner->Scanner(lexem);
        if (type_lexem != TIdent && type_lexem != TMain)
           Scanner->PrintError("Ожидался идентификатор", true);

        // если функция
        type_lexem = Scanner->Scanner(lexem);
        if (type_lexem == TParent_O){
           Scanner->SetCurrent(cur, str, pos);
           Function();
        }
        else
            //если нет, то описание переменных
            if (type_lexem == TAssign || type_lexem == TSemicolon || type_lexem == TComma) {
               Scanner->SetCurrent(cur, str, pos);
               VariableDescription();
            }
            // вывод ошибки при неправильном коде
            else
                Scanner->PrintError("Ожидалось одно из (\'=\' \';\' \',\' \'(\')", true);
    }
}

/*Функция*/
void TAnallizator::Function(){
    TypeLexem lexem;
    TDataType type_data;

    int type_lexem;
    int lexem_id;
    int cur, pos, str;

    type_lexem = Scanner->Scanner(lexem);

    // определяем тип функции
    if(type_lexem == TInt)
        type_data = TYPE_INTEGER;

    if (type_lexem == TDouble)
        type_data = TYPE_DOUBLE;

    // запоминаем индентификатор функции
    type_lexem = Scanner->Scanner(lexem);
    lexem_id = type_lexem;

    Tree * vertex = SemanticTree ->SemIncludeFunc(lexem, type_data);     // добавляем в дерево функцию

    type_lexem = Scanner->Scanner(lexem);                               // (
    type_lexem = Scanner->Scanner(lexem);                               // Ident || )

    // заносим параметры функции
    int counter = 0;
    while(type_lexem != TParent_C) {

        if (type_lexem != TInt && type_lexem != TDouble)
            Scanner->PrintError("Ожидался тип данных (int или double)", true);

        // определили тип переменной
        if(type_lexem == TInt)
            type_data = TYPE_INTEGER;

        if (type_lexem == TDouble)
            type_data = TYPE_DOUBLE;

        // занесли в дерево
        type_lexem = Scanner->Scanner(lexem);
        SemanticTree ->SemIncludePar(lexem, type_data);
        counter++;

        type_lexem = Scanner->Scanner(lexem);
        if (type_lexem != TComma && type_lexem != TParent_C)
            Scanner->PrintError("Ожидался ) или ,", true);

        if (type_lexem == TComma)
            type_lexem = Scanner->Scanner(lexem);
    }

    SemanticTree->SemSetParamCount(vertex, counter); // запомнили число параметров

    // вернулись
    Scanner->GetCurrent(&cur, &pos, &str);
    vertex->SetPosInText(cur, pos, str);

    // если это главная функция, открываем block
    if (lexem_id == TMain) {
        Block();
    }
    // если это обычная функция, работаем через флаг
    else {
        Tree::FLInt = false;
        Block();
        Tree::FLInt = true;
    }

    SemanticTree->SetCur(vertex);
    return;
}

/*Описания переменных*/
void TAnallizator::VariableDescription(){
    TypeLexem lexem;
    TypeLexem lexem_name;
    TDataType type_data;
    int type_lexem;

    // определили тип переменной
    type_lexem = Scanner->Scanner(lexem);

    if(type_lexem == TInt)
        type_data = TYPE_INTEGER;

    if (type_lexem == TDouble)
        type_data = TYPE_DOUBLE;

    do {
        //определяем id
        type_lexem = Scanner->Scanner(lexem);
        memcpy(lexem_name, lexem, sizeof(lexem));

        if (type_lexem != TIdent)
            Scanner->PrintError("Ожидался идентификатор", true);

        Tree* vertex = SemanticTree ->SemIncludePar(lexem, type_data); // заносим

        // определяем, присваивание или нет
        type_lexem = Scanner->Scanner(lexem);
        if (type_lexem == TAssign){
            TDataType expression_type;
            TDataValue data_value;

            //вызываем присваивание
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

            //заносим значение переменной и делаем вывод
            SemanticTree ->SetVarValue(vertex, data_value);
            logAssing(lexem_name, type_data, data_value);

            type_lexem = Scanner->Scanner(lexem);
        }

    } while (type_lexem == TComma); // повторяем пока ,

    // ожидаем ;
    if (type_lexem != TSemicolon)
        Scanner->PrintError("Ожидалось ; ", true);
}

/*Блок*/
void TAnallizator::Block(){
    TypeLexem lexem;
    int type_lexem;

    int cur, str, pos;

    // заносим в дерево
    type_lexem = Scanner->Scanner(lexem);
    SemanticTree ->AddLeft();

    Tree * vertex = SemanticTree ->GetCur();
    SemanticTree ->AddRight();

    // ожидаем {
    if (type_lexem != TBrace_O)
        Scanner->PrintError("Ожидалось { ", true);

    Scanner->GetCurrent(&cur, &str, &pos);
    type_lexem = Scanner->Scanner(lexem);

    // если } конец блока
    if(type_lexem == TBrace_C)
        return;

    // если не }, то работаем дальше
    while (true) {

        // определяем переменные
        Scanner->SetCurrent(cur, str, pos);
        type_lexem = Scanner->Scanner(lexem);
        if (type_lexem == TInt || type_lexem == TDouble){
            Scanner->SetCurrent(cur, str, pos);
            VariableDescription();
        }
        // если нет, то идем к оператору
        else {
            Scanner->SetCurrent(cur, str, pos);
            Operator();
        }

        // ждем }
        Scanner->GetCurrent(&cur, &str, &pos);
        type_lexem = Scanner->Scanner(lexem);
        if(type_lexem == TBrace_C) {
            Tree * newCur = SemanticTree ->GetUp(SemanticTree ->GetUp(vertex));
            SemanticTree ->FreeAfterBlock(newCur);
            SemanticTree ->SetCur(newCur);
            return;
        }

        // конец программы
        if(type_lexem == TEnd)
            break;
    }

    Scanner->PrintError("Ожидалось } ", true);
}

/*Оператор*/
void TAnallizator::Operator(){
    TypeLexem lexem;
    int type_lexem;
    int cur, str, pos;

    TDataType expression_type;
    TDataValue *data_value;

    // ожидаем return
    Scanner->GetCurrent(&cur, &str, &pos);
    type_lexem = Scanner->Scanner(lexem);
    if (type_lexem == TReturn){
        Scanner->SetCurrent(cur, str, pos);
        Return();
    }
    else
        // если {  в блок
        if (type_lexem == TBrace_O){
            Scanner->SetCurrent(cur, str, pos);
            Block();
        }
        else
            // если while в while
            if (type_lexem == TWhile) {
                Scanner->SetCurrent(cur, str, pos);
                While();
            }
            else
                //если индентификатор
                if (type_lexem == TIdent){
                    Tree* vertex = SemanticTree->FindUp(lexem); // заносим

                    //если не null и стоит флаг интерпретатора
                    if (vertex == nullptr && Tree::FLInt) {
                        type_lexem = Scanner->Scanner(lexem);

                        // если получили ( или нет, то ошибка
                        if (type_lexem == TParent_O)
                            SemanticTree ->PrintError("Необъявленная функция", nullptr);
                        else
                            SemanticTree ->PrintError("Необъявленная переменная", nullptr);

                        Scanner->PrintError();
                    }

                    data_value = SemanticTree ->GetDataValue(vertex);
                    type_lexem = Scanner->Scanner(lexem);

                    // если присваивание
                    if (type_lexem == TAssign){
                        Scanner->SetCurrent(cur, str, pos);
                        Assign();
                    }
                    else {
                        Scanner->SetCurrent(cur, str, pos);

                        A5(&expression_type, data_value);

                        type_lexem = Scanner->Scanner(lexem);

                        // ожидаем ;
                        if (type_lexem != TSemicolon)
                            Scanner->PrintError("Ожидалось ; ", true);
                    }
                }
                else
                    // если ,
                    if (type_lexem == TSemicolon){
                        Scanner->SetCurrent(cur, str, pos);
                        type_lexem = Scanner->Scanner(lexem);
                    }
                    else {
                        data_value = new TDataValue();
                        Scanner->SetCurrent(cur, str, pos);

                        A5(&expression_type,data_value);

                        type_lexem = Scanner->Scanner(lexem);

                        // ожидаем ;
                        if (type_lexem != TSemicolon)
                            Scanner->PrintError("Ожидалось ; ", true);
                    }
}

/*Возврат*/
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
        Scanner->PrintError("Ожидалось ;", true);

    // если флаг инерпретатора true, то флаг возврата true
    if(Tree::FLInt == true)
        Tree::FLRet = true;

    Tree::FLInt = false;
}

/*Оператор While*/
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

    // если не (, ошибка
    if (type_lexem != TParent_O)
        Scanner->PrintError("Ожидалось (", true);

    TDataType expression_type;
    TDataValue data_value;

    A1(&expression_type, &data_value);

    // если флаг интерп true и есть значение, то ставим true
    if(Tree::FLInt && (data_value.DataAsInt != 0))
        Tree::FLInt = true;
    else
        Tree::FLInt = false;

    type_lexem = Scanner->Scanner(lexem);
    if (type_lexem != TParent_C)
        Scanner->PrintError("Ожидалось )", true);

    // идем в оператор
    Operator();

    // возращаемся назад если флаг true
    if(Tree::FLInt){
        Scanner->SetCurrent(cur, str, pos);
        goto Start;
    }

    Tree::FLInt = localFlInt;

    if (Tree::FLRet)
        Tree::FLInt = false;

    return;
}

/*Присваивание*/
void TAnallizator::Assign(){
    TypeLexem lexem;
    TypeLexem lexem_name;
    int type_lexem;

    Tree *vertex;
    TDataType VType, // первая часть выражения
            EType; // вторая часть выражения

    TDataValue* data_value = new TDataValue();

    // индентификатор
    type_lexem = Scanner->Scanner(lexem);
    memcpy(lexem_name, lexem, sizeof (lexem));

    // получаем тип лексемы и тип данных
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

    SemanticTree->SetVarValue(vertex, *data_value); // значение
    logAssing(lexem_name, VType, *data_value);

    // проверка на ;
    type_lexem = Scanner->Scanner(lexem);
    if (type_lexem != TSemicolon)
        Scanner->PrintError("Ожидалось ;", true);

    return;
}

/*Вызов функции*/
void TAnallizator::CallFunction(TDataValue* data_value){
    TypeLexem lexem;
    int type_lexem;
    int cur_f, str_f, pos_f;
    int cur_s, str_s, pos_s;
    int count;

    // индетификатор
    type_lexem = Scanner->Scanner(lexem);

    Tree *vertex = SemanticTree ->SemGetFunct(lexem); // получаем

    type_lexem = Scanner->Scanner(lexem); // (
    count = 0;

    Scanner->GetCurrent(&cur_f, &str_f, &pos_f);
    type_lexem = Scanner->Scanner(lexem);

    // если )
    if (type_lexem != TParent_C)
        Scanner->SetCurrent(cur_f, str_f, pos_f);

    Tree * exeFunc = SemanticTree->SemCopyFunc(vertex); // копируем

    // пока не дойдем )
    while(type_lexem != TParent_C) {
        count++;
        TDataType expression_type;
        TDataValue data_value;

        A1(&expression_type, &data_value);

        SemanticTree ->SemSetExeFuncParam(exeFunc, data_value,count);

        // ожидаем , или )
        type_lexem = Scanner->Scanner(lexem);
        if (type_lexem != TComma && type_lexem != TParent_C)
            Scanner->PrintError("Ожидалось , или )", true);
    }

    // сравниваем кол параметров
    SemanticTree ->SemParamEqual(vertex, count);
    Tree *cur_tree = nullptr;
    Tree *start_tree = nullptr;

    Scanner->GetCurrent(&cur_s, &str_s, &pos_s);

    // проходимся по функции
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

        // возвращаем значение
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

/*Сравнение*/
void TAnallizator::A1(TDataType * data_type, TDataValue* data_value){
    TDataType VType, EType;
    TDataValue* value = new TDataValue();

    // получаем значение первой части
    A2(&VType, data_value);

    TypeLexem lexem;
    int operation;
    int cur, str, pos;

    Scanner->GetCurrent(&cur, &str, &pos);
    operation = Scanner->Scanner(lexem);

    // если нет одной из операции сравнения, то выход
    if ((operation != TGreat && operation != TLess && operation != TGreat_EQ && operation != TLess_EQ && operation != TEQ && operation != TNot_EQ)){
        Scanner->SetCurrent(cur, str, pos);
        *data_type = VType;
        return;
    }

    // если есть, то выполняем
    while(true){
        // получаем значение второй части
        A2(&EType, value);

        // сравниваем
        if (Tree::FLInt){
            switch (VType) {
            case TYPE_INTEGER:
                switch (EType) {
                case TYPE_INTEGER:
                    switch (operation) {
                        case TGreat:     {
                           data_value->DataAsInt = data_value->DataAsInt > value->DataAsInt;
                           logChange(nullptr,"Сравнение \'больше\' (>)", VType, *data_value);
                           break;}
                        case TLess:    {
                           data_value->DataAsInt = data_value->DataAsInt < value->DataAsInt;
                           logChange(nullptr,"Сравнение \'меньше\' (<)", VType, *data_value);
                           break;}
                        case TGreat_EQ:     {
                           data_value->DataAsInt = data_value->DataAsInt >= value->DataAsInt;
                           logChange(nullptr,"Сравнение \'больше или равно\' (>=)", VType, *data_value);
                           break;}
                        case TLess_EQ:    {
                           data_value->DataAsInt = data_value->DataAsInt <= value->DataAsInt;
                           logChange(nullptr,"Сравнение \'меньше или равно\' (<=)", VType, *data_value);
                           break;}
                        case TEQ:     {
                           data_value->DataAsInt = data_value->DataAsInt == value->DataAsInt;
                           logChange(nullptr,"Сравнение \'равно\' (==)", VType, *data_value);
                           break;}
                        case TNot_EQ:    {
                           data_value->DataAsInt = data_value->DataAsInt != value->DataAsInt;
                           logChange(nullptr,"Сравнение \'не равно\' (!=)", VType, *data_value);
                           break;}
                    }
                    break;
                case TYPE_DOUBLE:
                    switch (operation) {
                        case TGreat:     {
                           data_value->DataAsInt = data_value->DataAsInt > value->DataAsDouble;
                           logChange(nullptr,"Сравнение \'больше\' (>)", VType, *data_value);
                           break;}
                        case TLess:    {
                           data_value->DataAsInt = data_value->DataAsInt < value->DataAsDouble;
                           logChange(nullptr,"Сравнение \'меньше\' (<)", VType, *data_value);
                           break;}
                        case TGreat_EQ:     {
                           data_value->DataAsInt = data_value->DataAsInt >= value->DataAsDouble;
                           logChange(nullptr,"Сравнение \'больше или равно\' (>=)", VType, *data_value);
                           break;}
                        case TLess_EQ:    {
                           data_value->DataAsInt = data_value->DataAsInt <= value->DataAsDouble;
                           logChange(nullptr,"Сравнение \'меньше или равно\' (<=)", VType, *data_value);
                           break;}
                        case TEQ:     {
                           data_value->DataAsInt = data_value->DataAsInt == value->DataAsDouble;
                           logChange(nullptr,"Сравнение \'равно\' (==)", VType, *data_value);
                           break;}
                        case TNot_EQ:    {
                           data_value->DataAsInt = data_value->DataAsInt != value->DataAsDouble;
                           logChange(nullptr,"Сравнение \'не равно\' (!=)", VType, *data_value);
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
                           logChange(nullptr,"Сравнение \'больше\' (>)", VType, *data_value);
                           break;}
                        case TLess:    {
                           data_value->DataAsInt = data_value->DataAsDouble < value->DataAsInt;
                           logChange(nullptr,"Сравнение \'меньше\' (<)", VType, *data_value);
                           break;}
                        case TGreat_EQ:     {
                           data_value->DataAsInt = data_value->DataAsDouble >= value->DataAsInt;
                           logChange(nullptr,"Сравнение \'больше или равно\' (>=)", VType, *data_value);
                           break;}
                        case TLess_EQ:    {
                           data_value->DataAsInt = data_value->DataAsDouble <= value->DataAsInt;
                           logChange(nullptr,"Сравнение \'меньше или равно\' (<=)", VType, *data_value);
                           break;}
                        case TEQ:     {
                           data_value->DataAsInt = data_value->DataAsDouble == value->DataAsInt;
                           logChange(nullptr,"Сравнение \'равно\' (==)", VType, *data_value);
                           break;}
                        case TNot_EQ:    {
                           data_value->DataAsInt = data_value->DataAsDouble != value->DataAsInt;
                           logChange(nullptr,"Сравнение \'не равно\' (!=)", VType, *data_value);
                           break;}
                    }
                    break;
                case TYPE_DOUBLE:
                    switch (operation) {
                        case TGreat:     {
                           data_value->DataAsInt = data_value->DataAsDouble > value->DataAsDouble;
                           logChange(nullptr,"Сравнение \'больше\' (>)", VType, *data_value);
                           break;}
                        case TLess:    {
                           data_value->DataAsInt = data_value->DataAsDouble < value->DataAsDouble;
                           logChange(nullptr,"Сравнение \'меньше\' (<)", VType, *data_value);
                           break;}
                        case TGreat_EQ:     {
                           data_value->DataAsInt = data_value->DataAsDouble >= value->DataAsDouble;
                           logChange(nullptr,"Сравнение \'больше или равно\' (>=)", VType, *data_value);
                           break;}
                        case TLess_EQ:    {
                           data_value->DataAsInt = data_value->DataAsDouble <= value->DataAsDouble;
                           logChange(nullptr,"Сравнение \'меньше или равно\' (<=)", VType, *data_value);
                           break;}
                        case TEQ:     {
                           data_value->DataAsInt = data_value->DataAsDouble == value->DataAsDouble;
                           logChange(nullptr,"Сравнение \'равно\' (==)", VType, *data_value);
                           break;}
                        case TNot_EQ:    {
                           data_value->DataAsInt = data_value->DataAsDouble != value->DataAsDouble;
                           logChange(nullptr,"Сравнение \'не равно\' (!=)", VType, *data_value);
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

/*Сдвиги*/
void TAnallizator::A2(TDataType * data_type, TDataValue* data_value){
    TDataType VType, EType;
    TDataValue* value = new TDataValue();

    A3(&VType, data_value);

    TypeLexem lexem;
    int type_lexem;
    int cur, str, pos;

    Scanner->GetCurrent(&cur, &str, &pos);
    type_lexem = Scanner->Scanner(lexem);

    // если не сдвиги, выход
    if (type_lexem != TShift_L && type_lexem != TShift_R){
        Scanner->SetCurrent(cur,str,pos);
        *data_type = VType;
        return;
    }

    // иначе работаем
    while(true){
        A3(&EType,value);

        if (Tree::FLInt){
            if(VType == TYPE_DOUBLE){
                SemanticTree ->PrintError("Операции сдвига запрещены", nullptr);
                Scanner->PrintError();
            }
            else{
                switch (type_lexem) {
                case TShift_L: {
                   data_value->DataAsInt = data_value->DataAsInt << value->DataAsInt;
                   logChange(nullptr,"Сдвиг влево (<<)", VType, *data_value);
                   break;
                }
                case TShift_R:    {
                   data_value->DataAsInt = data_value->DataAsInt >> value->DataAsInt;
                   logChange(nullptr,"Сдвиг вправо (>>))", VType, *data_value);
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

/*Сложение и вычитание*/
void TAnallizator::A3(TDataType * data_type, TDataValue* data_value){
    TDataType VType, EType;
    TDataValue* value = new TDataValue();

    A4(&VType, data_value);

    TypeLexem lexem;
    int type_lexem;
    int cur, str, pos;

    Scanner->GetCurrent(&cur, &str, &pos);
    type_lexem = Scanner->Scanner(lexem);

    // если не + -, выход
    if (type_lexem != TPlus && type_lexem != TMinus){
        Scanner->SetCurrent(cur,str,pos);
        *data_type = VType;
        return;
    }

    // иначе работаем
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
                                    logChange(nullptr,"Сложение (+)", VType, *data_value);
                                    break;
                                }
                                case TMinus: {
                                    data_value->DataAsInt = data_value->DataAsInt - value->DataAsInt;
                                    logChange(nullptr,"Вычитание (-))", VType, *data_value);
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
                                    logChange(nullptr,"Сложение (+)", VType, *data_value);
                                    break;
                                }
                                case TMinus: {
                                    data_value->DataAsInt = data_value->DataAsInt - value->DataAsDouble;
                                    logChange(nullptr,"Вычитание (-))", VType, *data_value);
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
                                    logChange(nullptr,"Сложение (+)", VType, *data_value);
                                    break;
                                }
                                case TMinus: {
                                    data_value->DataAsDouble = data_value->DataAsDouble - value->DataAsDouble;
                                    logChange(nullptr,"Вычитание (-))", VType, *data_value);
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
                                    logChange(nullptr,"Сложение (+)", VType, *data_value);
                                    break;
                                }
                                case TMinus: {
                                    data_value->DataAsDouble = data_value->DataAsDouble - value->DataAsDouble;
                                    logChange(nullptr,"Вычитание (-))", VType, *data_value);
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

/*Умножение, деление, деление нацело*/
void TAnallizator::A4(TDataType * data_type, TDataValue* data_value){
    TDataType VType, EType;
    TDataValue* value = new TDataValue();

    A5(&VType, data_value);

    TypeLexem lexem;
    int type_lexem;
    int cur, str, pos;

    Scanner->GetCurrent(&cur, &str, &pos);
    type_lexem = Scanner->Scanner(lexem);

    // если не / % *, выход
    if (type_lexem != TStar && type_lexem != TRSlash && type_lexem != TPercent){
        Scanner->SetCurrent(cur,str,pos);
        *data_type = VType;
        return;
    }

    // иначе работаем
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
                                    logChange(nullptr,"Умножение (*)", VType, *data_value);
                                    break;
                                }
                                case TRSlash: {
                                    data_value->DataAsInt = data_value->DataAsInt / value->DataAsInt;
                                    logChange(nullptr,"Деление нацело (/))", VType, *data_value);
                                    break;
                                }
                                case TPercent: {
                                    data_value->DataAsInt = data_value->DataAsInt % value->DataAsInt;
                                    logChange(nullptr,"Остаток от деление (%))", VType, *data_value);
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
                                    logChange(nullptr,"Умножение (*)", VType, *data_value);
                                    break;
                                }
                                case TRSlash: {
                                    data_value->DataAsInt = data_value->DataAsInt / value->DataAsDouble;
                                    logChange(nullptr,"Деление нацело (/))", VType, *data_value);
                                    break;
                                }
                                case TPercent: {
                                    SemanticTree ->PrintError("Операции остатка от деления запрещены", nullptr);
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
                                    logChange(nullptr,"Умножение (*)", VType, *data_value);
                                    break;
                                }
                                case TRSlash: {
                                    data_value->DataAsDouble = data_value->DataAsDouble / value->DataAsDouble;
                                    logChange(nullptr,"Деление нацело (/))", VType, *data_value);
                                    break;
                                }
                                case TPercent: {
                                    SemanticTree ->PrintError("Операции остатка от деления запрещены", nullptr);
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
                                    logChange(nullptr,"Умножение (*)", VType, *data_value);
                                    break;
                                }
                                case TRSlash: {
                                    data_value->DataAsDouble = data_value->DataAsDouble / value->DataAsDouble;
                                    logChange(nullptr,"Деление нацело (/))", VType, *data_value);
                                    break;
                                }
                                case TPercent: {
                                    SemanticTree ->PrintError("Операции остатка от деления запрещены", nullptr);
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

/*Унарные операции постфикс*/
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
                            logChange(nullptr,"Унарный двойной плюс(Постфикс)", VType, *data_value);
                            break;
                        }
                        case TDMinus:    {
                            data_value->DataAsInt = data_value->DataAsInt - 1;
                            logChange(nullptr,"Унарный двойной минус(Постфикс)", VType, *data_value);
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
                            logChange(nullptr,"Унарный двойной плюс(Постфикс)", VType, *data_value);
                            break;
                        }
                        case TDMinus:    {
                            data_value->DataAsDouble = data_value->DataAsDouble - 1;
                            logChange(nullptr,"Унарный двойной минус(Постфикс)", VType, *data_value);
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

/*Унарные орерации префикс*/
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
                            logChange(nullptr,"Унарный плюс", VType, *data_value);
                            break;
                        }
                        case TDPlus:    {
                            data_value->DataAsInt = data_value->DataAsInt + 1;
                            logChange(nullptr,"Унарный двойной плюс(Префикс)", VType, *data_value);
                            break;
                        }
                        case TMinus:    {
                            data_value->DataAsInt = - data_value->DataAsInt;
                            logChange(nullptr,"Унарный минус", VType, *data_value);
                            break;
                        }
                        case TDMinus:   {
                            data_value->DataAsInt = data_value->DataAsInt - 1;
                            logChange(nullptr,"Унарный двойной минус(Префикс)", VType, *data_value);
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
                            logChange(nullptr,"Унарный плюс", VType, *data_value);
                            break;
                        }
                        case TDPlus:    {
                            data_value->DataAsDouble = data_value->DataAsDouble + 1;
                            logChange(nullptr,"Унарный двойной плюс(Префикс)", VType, *data_value);
                            break;
                        }
                        case TMinus:    {
                            data_value->DataAsDouble = - data_value->DataAsDouble;
                            logChange(nullptr,"Унарный минус", VType, *data_value);
                            break;
                        }
                        case TDMinus:   {
                            data_value->DataAsDouble = data_value->DataAsDouble - 1;
                            logChange(nullptr,"Унарный двойной минус(Префикс)", VType, *data_value);
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

    // перевод значений констант
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
                Scanner->PrintError("Ожидалось )", true);

            *data_type = VType;
        }
        else
            // id
            if (type_lexem == TIdent) {
                Tree * v = SemanticTree ->Cur->FindUp(lexem);

                if (v==nullptr && Tree::FLInt){
                    type_lexem = Scanner->Scanner(lexem);

                    if (type_lexem == TParent_O)
                        SemanticTree ->PrintError("Необъявленная функция", nullptr);
                    else
                        SemanticTree ->PrintError("Необъявленная переменная", nullptr);

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
            Scanner->PrintError("Ожидалось одно из (Идентификатор, Константа, Вызов функции, Выражение в скобках)", true);
}

/*Вывод отладки присваивание*/
void TAnallizator::logAssing(char * id, TDataType type, TDataValue value){
    if(!Tree::FLInt) return;
    switch (type) {
    case TYPE_INTEGER:  {printf("Переменной \'%s\' \tтипа int \tприсвоено значение %d \n",id,value.DataAsInt); break;}
    case TYPE_DOUBLE: {printf("Переменной \'%s\' \tтипа double \tприсвоено значение %lf \n",id, value.DataAsDouble); break;}
    default:
        break;
    }
}

/*Вывод изменение*/
void TAnallizator::logChange(char * id,const char * op,TDataType type, TDataValue value){
    if(!Tree::FLInt) return;
    if(id)
        printf("Переменной \'%s\'",id);
    else
        printf("При вычислении");
    switch (type) {
    case TYPE_INTEGER:  {printf("\tтипа int \tпри выполнении %s \t получено значение %d \n",op,value.DataAsInt); break;}
    case TYPE_DOUBLE: {printf("\tтипа double \tпри выполнении %s \t получено значение %lf \n",op, value.DataAsDouble); break;}
    default:
        break;
    }
}




