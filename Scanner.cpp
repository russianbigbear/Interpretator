#include "Scanner.h"
using namespace std;

static TypeLexem Keyword[COUNT_KEY_WORD] = { "int", "double", "while", "main", "return"};
static int IndexKeyword[COUNT_KEY_WORD]= {TInt, TDouble, TWhile, TMain, TReturn};

/*Чтение кода из File*/
void TScanner::ReadFile(string File) {
    int size_text = 0;

    FILE* FileStream = fopen(File.c_str(),"r");

    if (!FileStream)
        PrintError("Отсутствует файл с исходным кодом!", false);

    for(int i = 0; !feof(FileStream); i++, size_text++) {
        fscanf(FileStream,"%c",&InputCode[i]);

        if (i>=MAX_LENGHT_CODE - 1)
            PrintError("Превышен размер исходного кода!", false);
    }

    InputCode[size_text]='\0';

    fclose(FileStream);
}

/*Задать указатель на текущюю позицию*/
void TScanner::SetCurrent(int _cur, int _pos, int _str) {
    Cur = _cur; Pos = _pos; Str = _str;
}

/*Получить указатель на текущюю позицию*/
void TScanner::GetCurrent(int *_cur, int *_pos, int *_str) {
        *_cur = Cur; *_pos = Pos; *_str = Str;
}

/*Конструктор*/
TScanner:: TScanner(const char * FileName) {
    ReadFile(FileName);
    SetCurrent(0, 0, 1);
}

/*Работа сканера*/
int TScanner::Scanner(TypeLexem lexem) // программа сканера
{
    int i;                                              // текущая длина лексемы
    for ( i = 0; i < MAX_LENGHT_LEXEM; i++)             // очистили поле лексемы
        lexem[i] = 0;
    i  = 0;                                             // лексема заполняется с позиции i

start:
    // пропуск незн. символов
    while((InputCode[Cur]==' ' ) || (InputCode[Cur]=='\n') || (InputCode[Cur]=='\t')) {
        if (InputCode[Cur]==' ' ) {
            Cur++;
            Pos++;
        }

        if (InputCode[Cur]=='\n') {
            Cur++;
            Str++;
            Pos = 0;
        }

        if (InputCode[Cur]=='\t') {
            Cur++;
            Pos+= 4;
        }
    }

    // пропуск комментариев
    if ( (InputCode[Cur]=='/') && (InputCode[Cur+1]=='/') ) {
        Cur+=2;
        Pos+=2;

        while ((InputCode[Cur]!='\n')&&(InputCode[Cur]!='#')){
            Cur++;
            Pos++;
        }

        goto start;
     }

     // конец файла
    if (InputCode[Cur]=='\0') {
        lexem[0]='#';
        return TEnd;
    }

    // проверяем на цифру
    if (InputCode[Cur] <= '9' && InputCode[Cur] >= '0') {
        //если 0
        if(InputCode[Cur] == '0') {
            //если следующяя x
            if(InputCode[Cur + 1] == 'x') {
                Cur += 2;
                Pos += 2;

                // то это int в 16 сс
                while (( (InputCode[Cur] <= '9') && (InputCode[Cur]>='0')) || ( (InputCode[Cur] <='F') && (InputCode[Cur] >= 'A')))
                    if (i < MAX_LENGHT_LEXEM - 1) {
                        lexem[i++] = InputCode[Cur++];
                        Pos++;
                    }
                    else {
                        Cur++;
                        Pos++;
                    }
                return TConstInt_16;
            }
            // если следующяя не x
            else {
                Cur++;
                Pos+=2;

                // то это int в 8 сс
                while (( (InputCode[Cur]<='7' )&& (InputCode[Cur]>='0')))
                    if (i<MAX_LENGHT_LEXEM-1) {
                        lexem[i++]=InputCode[Cur++];
                        Pos++;
                    }
                    else {
                        Cur++;
                        Pos++;
                    }
                return TConstInt_8;
            }
        }
        // если не 0, то int в 10сс
        while (( (InputCode[Cur]<='9') && (InputCode[Cur]>='0')))
            if (i<MAX_LENGHT_LEXEM-1) {
                lexem[i++]=InputCode[Cur++];
                Pos++;
            }
            else {
                Cur++;
                Pos++;
            }
        return TConstInt_10;
    }
    else
        // проверяем на индентификатор
        if ( ((InputCode[Cur]>='a') && (InputCode[Cur]<='z')) || ((InputCode[Cur]>='A') && (InputCode[Cur]<='Z')) ) {
                lexem[i++]=InputCode[Cur++];
                Pos++;

                while ( ((InputCode[Cur]<='9')&&(InputCode[Cur]>='0')) ||
                        ((InputCode[Cur]>='a')&&(InputCode[Cur]<='z')) ||
                        ((InputCode[Cur]>='A')&&(InputCode[Cur]<='Z')) ||
                        InputCode[Cur] == '_')

                        if (i < MAX_LENGHT_LEXEM - 1) {
                            lexem[i++] = InputCode[Cur++];
                            Pos++;
                        }
                        else {
                            Cur++;
                            Pos++;
                        }


                // проверка на ключевое слово:
                for (int j = 0; j <COUNT_KEY_WORD; j++)
                        if (strcmp(lexem, Keyword[j]) == 0)
                            return IndexKeyword[j];

                return TIdent;
        }
        else
            // проверяем на char
            if (InputCode[Cur] == '\''){
                Cur++;
                Pos++;

                while ((InputCode[Cur] != '\'')
                       && (InputCode[Cur] != '\n')
                       && (InputCode[Cur] != '#')) {

                        if(i<MAX_LENGHT_LEXEM) {
                            lexem[i++]=InputCode[Cur++];
                            Pos++;
                        }
                        else {
                            Cur++;
                            Pos++;
                        }
                }
                if (InputCode[Cur] != '\'') {
                        PrintError("Неверное обозначение char", true);
                        return TErr;
                }
                Cur++;
                return TConstChar;
            }
            else
                // проверяем на умножение
                if (InputCode[Cur]=='*') {
                    lexem[i++]=InputCode[Cur++];
                    Pos++;
                    return TStar;
                }
                else
                    // проверяем на целое деление
                    if (InputCode[Cur]=='/')
                    {
                        lexem[i++]=InputCode[Cur++];
                        Pos++;
                        return TRSlash;
                    }
                    else
                        // проверяем на остаточное деление
                        if (InputCode[Cur]=='%'){
                            lexem[i++]=InputCode[Cur++];
                            Pos++;
                            return TPercent;
                        }
                        else
                            // проверяем на ;
                            if (InputCode[Cur]==';'){
                                lexem[i++]=InputCode[Cur++];
                                Pos++;
                                return TSemicolon;
                            }
                            else
                                // проверяем на (
                                if (InputCode[Cur]=='('){
                                    lexem[i++]=InputCode[Cur++];
                                    Pos++;
                                    return TParent_O;
                                }
                                else
                                    // проверяем на )
                                    if (InputCode[Cur]==')'){
                                        lexem[i++]=InputCode[Cur++];
                                        Pos++;
                                        return TParent_C;
                                    }
                                    else
                                        // проверяем на {
                                        if (InputCode[Cur]=='{') {
                                            lexem[i++]=InputCode[Cur++];
                                            Pos++;
                                            return TBrace_O;
                                        }
                                        else
                                            // проверяем на }
                                            if (InputCode[Cur]=='}'){
                                                lexem[i++]=InputCode[Cur++];
                                                Pos++;
                                                return TBrace_C;
                                            }
                                            else
                                                // проверяем на ,
                                                if (InputCode[Cur]==','){
                                                    lexem[i++]=InputCode[Cur++];
                                                    Pos++;
                                                    return TComma;
                                                }
                                                else
                                                    // проверяем на +
                                                    if (InputCode[Cur]=='+') {
                                                        lexem[i++]=InputCode[Cur++];
                                                        Pos++;

                                                        // проверяем на ++
                                                        if (InputCode[Cur]=='+') {
                                                            lexem[i++]=InputCode[Cur++];
                                                            Pos++;

                                                            return TDPlus;
                                                        }
                                                        return TPlus;
                                                    }
                                                    else
                                                        // проверяем на -
                                                        if (InputCode[Cur]=='-') {
                                                            lexem[i++]=InputCode[Cur++];
                                                            Pos++;

                                                            // проверяем на --
                                                            if (InputCode[Cur]=='-') {
                                                                lexem[i++]=InputCode[Cur++];
                                                                Pos++;
                                                                return TDMinus;
                                                            }
                                                            return TMinus;
                                                        }
                                                        else
                                                            // проверяем на <
                                                            if (InputCode[Cur]=='<') {
                                                                lexem[i++]=InputCode[Cur++];
                                                                Pos++;

                                                                // проверяем на <=
                                                                if (InputCode[Cur]=='=') {
                                                                    lexem[i++]=InputCode[Cur++];
                                                                     Pos++;
                                                                     return TLess_EQ;
                                                                }
                                                                else
                                                                    // проверяем на <<
                                                                    if (InputCode[Cur]=='<') {
                                                                        lexem[i++]=InputCode[Cur++];
                                                                        Pos++;
                                                                        return TShift_L;
                                                                }
                                                                return TLess;
                                                            }
                                                            else
                                                                // проверяем на >
                                                                if (InputCode[Cur]=='>') {
                                                                    lexem[i++]=InputCode[Cur++];
                                                                    Pos++;

                                                                    // проверяем на >=
                                                                    if (InputCode[Cur]=='=') {
                                                                        lexem[i++]=InputCode[Cur++];
                                                                        Pos++;
                                                                        return TGreat_EQ;
                                                                    }
                                                                    else
                                                                        // проверяем на >>
                                                                        if (InputCode[Cur]=='>') {
                                                                            lexem[i++]=InputCode[Cur++];
                                                                            Pos++;
                                                                            return TShift_R;
                                                                        }
                                                                    return TGreat;
                                                                }
                                                                else
                                                                    // проверяем на =
                                                                    if (InputCode[Cur]=='=') {
                                                                        lexem[i++]=InputCode[Cur++];
                                                                        Pos++;

                                                                        // проверяем на ==
                                                                        if (InputCode[Cur]=='=') {
                                                                            lexem[i++]=InputCode[Cur++];
                                                                            Pos++;
                                                                            return TEQ;
                                                                        }
                                                                        return TAssign;
                                                                    }
                                                                    else
                                                                        // проверяем на !
                                                                        if (InputCode[Cur]=='!') {
                                                                            lexem[i++]=InputCode[Cur++];
                                                                            Pos++;

                                                                            // проверяем на !=
                                                                            if (InputCode[Cur]=='=') {
                                                                                lexem[i++]=InputCode[Cur++];
                                                                                Pos++;
                                                                                return TNot_EQ;
                                                                            }
                                                                            PrintError("Неверный символ",true);
                                                                            return TErr;
                                                                        }
                                                                        // проверяем на ошибочные символы
                                                                        else {
                                                                            lexem[i++]=InputCode[Cur++];
                                                                            Pos++;

                                                                            PrintError("Неверный символ",true); // ошибка
                                                                            return TErr;
                                                                        }
}

/*Вывод ошибки с параметрами*/
void TScanner::PrintError(const char *Error, bool State)
{
     if (!State)
        printf("Ошибка: %s. (строка - %d позиция - %d)\n", Error, Str, Pos);
     else
        printf("Ошибка: %s. Неверный символ (строка - %d позиция - %d)\n", Error, Str, Pos);
     exit(0);
}

/*Вывод ошибки без параметров*/
void TScanner::PrintError() {
    printf("(строка - %d позиция - %d)\n", Str, Pos);
    exit(0);
}
