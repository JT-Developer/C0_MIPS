#pragma once
#include "head.h"
#include "Lex.h"

extern string syn_keywordID[keywordNum];//�ؼ���ID
extern map<string, string> String_List;

void Syntax_Analysis_main();

///�﷨����
void syn_add();//+/-
void syn_multiply();//*//
void syn_relation();//��ϵ
void syn_letter();//��ĸ
void syn_digit();//����
void syn_notZeroDigit();//�����㿪ͷ����
void syn_character();//�ַ�
void syn_string();//�ַ���
void Syntax_Analysis();//����
void syn_const_dec();//��������
void syn_const_def();//��������
void syn_noSignInt();//�޷�������
void syn_int();//����
void syn_Fractional();//С��
void syn_realNumber();//ʵ��
void syn_identifier();//��ʶ�����Զ�����ţ�
void syn_headFile();//ͷ���������൱�ں���������
void syn_var_dec();//��������
void syn_var_def();//��������
void syn_enum();//��ö�ٳ���
void syn_typeidentifier();//���ͱ�ʶ��
void syn_functionwithreturn();//�з���ֵ�����Ķ���
void syn_functionwithoutreturn();//�޷���ֵ�����Ķ���
void syn_mixsentence();//������
void syn_parameter();//����
void syn_parameter_table();//������
void syn_main();//������
void syn_expression();//���ʽ
void syn_term();//��
void syn_factor();//����
void syn_sentence();//���
void syn_assign_sen();//��ֵ���
void syn_condition_sen();//�������
void syn_condition();//��������
void syn_loop_sen();//ѭ�����
void syn_situation_sen();//������
void syn_situation_table();//�����
void syn_situation_sen_son();//��������
void syn_default();//default
void syn_functionwithreturn_recall();//�����з���ֵ�ĺ���
void syn_functionwithoutreturn_recall();//�����޷���ֵ�ĺ���
void syn_value_parameter_table();//ֵ������
void syn_sentence_colomn();//�����
void syn_scanf();//��
void syn_printf();//д
void syn_return();//����