#pragma once
#include "head.h"
#include "Asm_Code.h"
#define MAX_NUM 100000

extern ostream cout;
extern Quadruple Optim_IR_List_1[MAX_NUM];


void Optimize_main();//�Ż�����
void Block_Output(string op, string src1, string src2, string res, int block);//������Ԫʽ
void IR_Optim_Output(string op, string src1, string src2, string res, int block,int isrepeat);//������Ԫʽ

extern int blocknum;
extern vector<pair<int, int>> blockpos;
extern vector<string> blockvar[MAX_NUM];
