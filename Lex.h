#ifndef LEX_H_INCLUDED
#define LEX_H_INCLUDED

#include "head.h"
#include "Symbol_Table.h"

#define keywordNum    54
#define reskeywordNum 21

ifstream Readin(string FILE);
void Read_File(ifstream& File);
void Lex_Aanlysis_main(string File);

//extern string keyword[keywordNum];//�ؼ��ּ���
extern string keywordID[keywordNum];//�ؼ���ID

extern const string reskeyword[reskeywordNum];
extern const string reskeywordID[reskeywordNum];

extern vector<string> LEX_File;//Դ�ļ�

#endif