#include "Scanner.h"
using namespace std;

static TypeLexem Keyword[COUNT_KEY_WORD] = { "int", "double", "while", "main", "return"};
static int IndexKeyword[COUNT_KEY_WORD]= {TInt, TDouble, TWhile, TMain, TReturn};

/*������ ���� �� File*/
void TScanner::ReadFile(string File) {
    int size_text = 0;

    FILE* FileStream = fopen(File.c_str(),"r");

    if (!FileStream)
        PrintError("����������� ���� � �������� �����!", false);

    for(int i = 0; !feof(FileStream); i++, size_text++) {
        fscanf(FileStream,"%c",&InputCode[i]);

        if (i>=MAX_LENGHT_CODE - 1)
            PrintError("�������� ������ ��������� ����!", false);
    }

    InputCode[size_text]='\0';

    fclose(FileStream);
}

/*������ ��������� �� ������� �������*/
void TScanner::SetCurrent(int _cur, int _pos, int _str) {
    Cur = _cur; Pos = _pos; Str = _str;
}

/*�������� ��������� �� ������� �������*/
void TScanner::GetCurrent(int *_cur, int *_pos, int *_str) {
        *_cur = Cur; *_pos = Pos; *_str = Str;
}

/*�����������*/
TScanner:: TScanner(const char * FileName) {
    ReadFile(FileName);
    SetCurrent(0, 0, 1);
}

/*������ �������*/
int TScanner::Scanner(TypeLexem lexem) // ��������� �������
{
    int i;                                              // ������� ����� �������
    for ( i = 0; i < MAX_LENGHT_LEXEM; i++)             // �������� ���� �������
        lexem[i] = 0;
    i  = 0;                                             // ������� ����������� � ������� i

start:
    // ������� ����. ��������
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

    // ������� ������������
    if ( (InputCode[Cur]=='/') && (InputCode[Cur+1]=='/') ) {
        Cur+=2;
        Pos+=2;

        while ((InputCode[Cur]!='\n')&&(InputCode[Cur]!='#')){
            Cur++;
            Pos++;
        }

        goto start;
     }

     // ����� �����
    if (InputCode[Cur]=='\0') {
        lexem[0]='#';
        return TEnd;
    }

    // ��������� �� �����
    if (InputCode[Cur] <= '9' && InputCode[Cur] >= '0') {
        //���� 0
        if(InputCode[Cur] == '0') {
            //���� ��������� x
            if(InputCode[Cur + 1] == 'x') {
                Cur += 2;
                Pos += 2;

                // �� ��� int � 16 ��
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
            // ���� ��������� �� x
            else {
                Cur++;
                Pos+=2;

                // �� ��� int � 8 ��
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
        // ���� �� 0, �� int � 10��
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
        // ��������� �� ��������������
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


                // �������� �� �������� �����:
                for (int j = 0; j <COUNT_KEY_WORD; j++)
                        if (strcmp(lexem, Keyword[j]) == 0)
                            return IndexKeyword[j];

                return TIdent;
        }
        else
            // ��������� �� char
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
                        PrintError("�������� ����������� char", true);
                        return TErr;
                }
                Cur++;
                return TConstChar;
            }
            else
                // ��������� �� ���������
                if (InputCode[Cur]=='*') {
                    lexem[i++]=InputCode[Cur++];
                    Pos++;
                    return TStar;
                }
                else
                    // ��������� �� ����� �������
                    if (InputCode[Cur]=='/')
                    {
                        lexem[i++]=InputCode[Cur++];
                        Pos++;
                        return TRSlash;
                    }
                    else
                        // ��������� �� ���������� �������
                        if (InputCode[Cur]=='%'){
                            lexem[i++]=InputCode[Cur++];
                            Pos++;
                            return TPercent;
                        }
                        else
                            // ��������� �� ;
                            if (InputCode[Cur]==';'){
                                lexem[i++]=InputCode[Cur++];
                                Pos++;
                                return TSemicolon;
                            }
                            else
                                // ��������� �� (
                                if (InputCode[Cur]=='('){
                                    lexem[i++]=InputCode[Cur++];
                                    Pos++;
                                    return TParent_O;
                                }
                                else
                                    // ��������� �� )
                                    if (InputCode[Cur]==')'){
                                        lexem[i++]=InputCode[Cur++];
                                        Pos++;
                                        return TParent_C;
                                    }
                                    else
                                        // ��������� �� {
                                        if (InputCode[Cur]=='{') {
                                            lexem[i++]=InputCode[Cur++];
                                            Pos++;
                                            return TBrace_O;
                                        }
                                        else
                                            // ��������� �� }
                                            if (InputCode[Cur]=='}'){
                                                lexem[i++]=InputCode[Cur++];
                                                Pos++;
                                                return TBrace_C;
                                            }
                                            else
                                                // ��������� �� ,
                                                if (InputCode[Cur]==','){
                                                    lexem[i++]=InputCode[Cur++];
                                                    Pos++;
                                                    return TComma;
                                                }
                                                else
                                                    // ��������� �� +
                                                    if (InputCode[Cur]=='+') {
                                                        lexem[i++]=InputCode[Cur++];
                                                        Pos++;

                                                        // ��������� �� ++
                                                        if (InputCode[Cur]=='+') {
                                                            lexem[i++]=InputCode[Cur++];
                                                            Pos++;

                                                            return TDPlus;
                                                        }
                                                        return TPlus;
                                                    }
                                                    else
                                                        // ��������� �� -
                                                        if (InputCode[Cur]=='-') {
                                                            lexem[i++]=InputCode[Cur++];
                                                            Pos++;

                                                            // ��������� �� --
                                                            if (InputCode[Cur]=='-') {
                                                                lexem[i++]=InputCode[Cur++];
                                                                Pos++;
                                                                return TDMinus;
                                                            }
                                                            return TMinus;
                                                        }
                                                        else
                                                            // ��������� �� <
                                                            if (InputCode[Cur]=='<') {
                                                                lexem[i++]=InputCode[Cur++];
                                                                Pos++;

                                                                // ��������� �� <=
                                                                if (InputCode[Cur]=='=') {
                                                                    lexem[i++]=InputCode[Cur++];
                                                                     Pos++;
                                                                     return TLess_EQ;
                                                                }
                                                                else
                                                                    // ��������� �� <<
                                                                    if (InputCode[Cur]=='<') {
                                                                        lexem[i++]=InputCode[Cur++];
                                                                        Pos++;
                                                                        return TShift_L;
                                                                }
                                                                return TLess;
                                                            }
                                                            else
                                                                // ��������� �� >
                                                                if (InputCode[Cur]=='>') {
                                                                    lexem[i++]=InputCode[Cur++];
                                                                    Pos++;

                                                                    // ��������� �� >=
                                                                    if (InputCode[Cur]=='=') {
                                                                        lexem[i++]=InputCode[Cur++];
                                                                        Pos++;
                                                                        return TGreat_EQ;
                                                                    }
                                                                    else
                                                                        // ��������� �� >>
                                                                        if (InputCode[Cur]=='>') {
                                                                            lexem[i++]=InputCode[Cur++];
                                                                            Pos++;
                                                                            return TShift_R;
                                                                        }
                                                                    return TGreat;
                                                                }
                                                                else
                                                                    // ��������� �� =
                                                                    if (InputCode[Cur]=='=') {
                                                                        lexem[i++]=InputCode[Cur++];
                                                                        Pos++;

                                                                        // ��������� �� ==
                                                                        if (InputCode[Cur]=='=') {
                                                                            lexem[i++]=InputCode[Cur++];
                                                                            Pos++;
                                                                            return TEQ;
                                                                        }
                                                                        return TAssign;
                                                                    }
                                                                    else
                                                                        // ��������� �� !
                                                                        if (InputCode[Cur]=='!') {
                                                                            lexem[i++]=InputCode[Cur++];
                                                                            Pos++;

                                                                            // ��������� �� !=
                                                                            if (InputCode[Cur]=='=') {
                                                                                lexem[i++]=InputCode[Cur++];
                                                                                Pos++;
                                                                                return TNot_EQ;
                                                                            }
                                                                            PrintError("�������� ������",true);
                                                                            return TErr;
                                                                        }
                                                                        // ��������� �� ��������� �������
                                                                        else {
                                                                            lexem[i++]=InputCode[Cur++];
                                                                            Pos++;

                                                                            PrintError("�������� ������",true); // ������
                                                                            return TErr;
                                                                        }
}

/*����� ������ � �����������*/
void TScanner::PrintError(const char *Error, bool State)
{
     if (!State)
        printf("������: %s. (������ - %d ������� - %d)\n", Error, Str, Pos);
     else
        printf("������: %s. �������� ������ (������ - %d ������� - %d)\n", Error, Str, Pos);
     exit(0);
}

/*����� ������ ��� ����������*/
void TScanner::PrintError() {
    printf("(������ - %d ������� - %d)\n", Str, Pos);
    exit(0);
}
