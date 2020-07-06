#ifndef DictionaryH
#define DictionaryH

#include <iostream>
#include <fstream>
#include <string.h>

#define MAX_LENGHT_CODE 10000                   // максимальная длина текста
#define MAX_LENGHT_LEXEM 100                    // максимальная длина лексемы
#define COUNT_KEY_WORD 5                        // число ключевых слов

typedef char TypeLexem[MAX_LENGHT_LEXEM];
typedef char TypeCode[MAX_LENGHT_CODE];

// Ключевые слова
#define TInt        1
#define TDouble     2
#define TWhile      3
#define TMain       4
#define TReturn     5

// Идентификаторы и константы
#define TIdent          20
#define TConstInt_10    30
#define TConstInt_8     31
#define TConstInt_16    32
#define TConstChar      33

// Специальные знаки
#define TParent_O   40
#define TParent_C   41
#define TBrace_O    42
#define TBrace_C    43
#define TComma      44

// Знаки операций
#define TStar        60
#define TRSlash       61
#define TPercent     62
#define TPlus        63
#define TMinus       64
#define TShift_L     65
#define TShift_R     66
#define TGreat       67
#define TLess        68
#define TGreat_EQ    69
#define TLess_EQ     70
#define TEQ          71
#define TNot_EQ      72
#define TAssign      73
#define TSemicolon   74
#define TDPlus       75
#define TDMinus      76

// Программные константы
#define TEnd   100
#define TErr   200

#endif
