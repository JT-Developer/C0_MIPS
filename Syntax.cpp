/****************************************************/
/* �ļ�: Optim.cpp                                  */
/* ����: 1.�����黮��                                */
/*       2.����������                                */
/*       3.��������                                 */
/*       5.���������ӱ��ʽ                          */
/*       6.�����Ż�                                 */
/****************************************************/

#include "head.h"

#include "Syntax.h"
#include "Symbol_Table.h"
#include "Error.h"
#include "IR.h"
#include "Asm_Code.h"

#define cout Syntax_out

using namespace std;

ofstream Syntax_out("./Syntax.txt");

map<string, bool> SYN_Const_Var;
map<string, bool> SYN_Local_Var;
map<string, bool> SYN_Global_Var;

map<string, bool> Return_Func;
map<string, bool> No_Return_Func;

map<string, string> String_List;

string glbtemp;
string functionname = "";
int syn_number = Lex_Table.size();
int syn_indexInlex_symbolTable = 0;
int syn_preindexInlex_symbolTable = 0;
string symbol = "";
string presymbol = "";
int syn_ID = 0;
int syn_preID = 0;

int syn_errorline = -1;
int syn_preerrorline = -1;

int syn_errorcolon = -1;
int syn_preerrorcolon = -1;

string syn_keywordID[keywordNum] =
{
	"lex_error_id","lex_int_id","lex_char_id","lex_float_id","lex_double_id","lex_bool_id",
	"lex_void_id","lex_const_id","lex_if_id","lex_else_id","lex_for_id","lex_while_id",
	"lex_do_id","lex_switch_id","lex_case_id","lex_default_id","lex_printf_id","lex_scanf_id",
	"lex_return_id","lex_main_id","lex_indentifer","lex_add_id","lex_subtract_id","lex_multiply_id",
	"lex_divide_id","lex_percent_id","lex_less_id","lex_greater_id","lex_lessequal_id","lex_greaterequal_id",
	"lex_equalequal_id","lex_notequal_id","lex_equal_id","lex_comma_id","lex_semicolon_id","lex_colon_id",
	"lex_dot_id","lex_singlemark_id","lex_doublemark_id","lex_leftparenthesis_id","lex_rightparenthesis_id",
	"lex_leftmiddle_id","lex_rightmiddle_id","lex_leftbraces_id","lex_rightbraces_id","lex_NotZeroHeadNum_id",
	"lex_string_id","lex_character_id","lex_letter_id","lex_ZeroToNine","lex_Zero_id","lex_OneToNine_id",
	"lex_ZeroHeadNum_id","lex_struct_id"
};//�ؼ���ID


void syn_nextsymbol()//����һ��
{
	syn_ID = Lex_Table[syn_indexInlex_symbolTable].lex_symbol_ID;
	symbol = Lex_Table[syn_indexInlex_symbolTable].lex_symbol;
	syn_errorline = Lex_Table[syn_indexInlex_symbolTable].lex_indexOfFile;
	syn_errorcolon = Lex_Table[syn_indexInlex_symbolTable].lex_indexOfLine;
	syn_indexInlex_symbolTable++;
	cout << setw(25) << left << syn_keywordID[syn_ID];
	cout << setw(10) << left << syn_ID << symbol << endl;
}

void syn_read_ahead()//Ԥ��һ��symbol
{
	cout << "***[readAhead]***" << "   ";
	syn_nextsymbol();
}
void syn_record()//��¼��ǰλ��
{
	syn_preID = syn_ID;
	syn_preindexInlex_symbolTable = syn_indexInlex_symbolTable;
	syn_preerrorline = syn_errorline;
	syn_preerrorcolon = syn_errorcolon;
	presymbol = symbol;
}
void syn_rollback()//���˵�֮ǰ��λ��
{
	cout << "***[rollBack]***" << endl;
	syn_ID = syn_preID;
	syn_indexInlex_symbolTable = syn_preindexInlex_symbolTable;
	syn_errorline = syn_preerrorline;
	syn_errorcolon = syn_preerrorcolon;
	symbol = presymbol;
}

/**
	��ڳ��������ȫ���ĳ������̣��Ƿ�������ڲ���
	������ ::= �ۣ�����˵�����ݣۣ�����˵������ {���з���ֵ�������壾|���޷���ֵ�������壾}����������
**/
void Syntax_Analysis()
{
	cout << "*�﷨����������ʼ" << endl;
	if (syn_ID == lex_const_id)//����˵��
	{
		syn_const_dec();
	}
	if (syn_ID == lex_int_id || syn_ID == lex_char_id)//����˵��
	{
		syn_record();
		syn_read_ahead();
		syn_read_ahead();
		if (syn_ID != lex_leftparenthesis_id)
		{
			syn_rollback();
			syn_var_dec();
		}
		else
		{
			syn_rollback();
		}
	}
	//{���з���ֵ�������壾|���޷���ֵ�������壾}
	syn_record();
	syn_read_ahead();
	int remsynID = syn_ID;
	syn_read_ahead();
	while (remsynID != lex_main_id && syn_ID == lex_leftparenthesis_id)
	{
		syn_rollback();
		if (syn_ID == lex_int_id || syn_ID == lex_char_id)
		{
			syn_functionwithreturn();
		}
		else
		{
			syn_functionwithoutreturn();
		}
		syn_record();
		syn_read_ahead();
		remsynID = syn_ID;
		syn_read_ahead();
	}
	syn_rollback();
	// ����������
	syn_record();
	syn_read_ahead();
	if (syn_ID != lex_main_id)
	{
		Grammar_Error(err_main, syn_errorline, syn_errorcolon);
		return;
	}
	else
	{
		syn_rollback();
		syn_main();
	}
	cout << "*�﷨�������������" << endl;
}

void Syntax_Analysis_main()
{
	SYN_Const_Var.clear();
	SYN_Local_Var.clear();
	SYN_Global_Var.clear();
	//state = syn_sta;
	//syn_init();
	syn_nextsymbol();
	Syntax_Analysis();
}

/**
	������˵���� ::=  const���������壾;{ const���������壾;}
**/
void syn_const_dec()//����˵��
{
	cout << "*�﷨����������˵����ʼ" << endl;
	if (syn_ID != lex_const_id)//����ǲ���const
	{
		Grammar_Error(err_const, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	syn_const_def();
	syn_nextsymbol();
	while (syn_ID == lex_const_id)
	{
		syn_nextsymbol();
		syn_const_def();//����������
		syn_nextsymbol();
	}
	cout << "*�﷨����������˵������" << endl;
}
/**
	���������壾   ::=   int����ʶ��������������{,����ʶ��������������}
						  | float����ʶ��������ʵ����{,����ʶ��������ʵ����}
						  | char����ʶ���������ַ���{,����ʶ���������ַ���}
**/
void syn_const_def()//��������
{
	cout << "*�﷨�������������忪ʼ" << endl;
	string op, s1, s2, res;
	string type;
	op = op_const;
	if (syn_ID != lex_int_id && syn_ID != lex_float_id && syn_ID != lex_char_id)
	{
		Grammar_Error(err_intorchar, syn_errorline, syn_errorcolon);
		return;
	}
	else//if(symID == INT_SYMID)///int����ʶ��������������{,����ʶ��������������}
	{
		if (syn_ID == lex_int_id)
		{
			type = op_int;
		}
		else///char
		{
			type = op_char;
		}
		syn_nextsymbol();
		if (SYN_Const_Var.find(symbol) == SYN_Const_Var.end())
		{
			SYN_Const_Var[symbol] = true;
		}
		else
		{
			Grammar_Error(err_const_redefination, syn_errorline, syn_errorcolon);
			return;
		}
		res = symbol;
		syn_identifier();
		if (syn_ID != lex_equal_id)
		{
			Grammar_Error(err_assign, syn_errorline, syn_errorcolon);
			return;
		}
		op = op_const;
		s1 = type;
		s2 = op_space;
		IR_Output(op, s1, s2, res);
		syn_nextsymbol();
		if (syn_ID == lex_subtract_id)
		{
			op = op_sub;
			s1 = op_zero;
			syn_nextsymbol();
			s2 = symbol;
			IR_Output(op, s1, s2, res);
		}
		else
		{
			op = op_ass;
			s1 = symbol;
			s2 = op_space;
			IR_Output(op, s1, s2, res);
		}
		syn_nextsymbol();
		while (syn_ID == lex_comma_id)//�����һ�ж���
		{
			syn_nextsymbol();
			if (SYN_Const_Var.find(symbol) == SYN_Const_Var.end())
			{
				SYN_Const_Var[symbol] = true;
			}
			else
			{
				Grammar_Error(err_const_redefination, syn_errorline, syn_errorcolon);
				return;
			}
			res = symbol;
			syn_identifier();
			if (syn_ID != lex_equal_id)
			{
				Grammar_Error(err_assign, syn_errorline, syn_errorcolon);
				return;
			}
			op = op_const;
			s1 = type;
			s2 = op_space;
			IR_Output(op, s1, s2, res);
			syn_nextsymbol();
			if (syn_ID == lex_subtract_id)
			{
				op = op_sub;
				s1 = op_zero;
				syn_nextsymbol();
				s2 = symbol;
				IR_Output(op, s1, s2, res);
			}
			else
			{
				op = op_ass;
				s1 = symbol;
				s2 = op_space;
				IR_Output(op, s1, s2, res);
			}
			syn_nextsymbol();
		}
	}
	cout << "*�﷨�����������������" << endl;
}
/**
	������ͷ����   ::=  int����ʶ���� |float ����ʶ����|char����ʶ����
**/
string funcname;//��¼�з���ֵ�ĺ�����
string functype;//��¼�з���ֵ�ĺ�������
void syn_headFile()//ͷ������
{
	cout << "*�﷨������ͷ��������ʼ" << endl;
	if (syn_ID == lex_int_id || syn_ID == lex_char_id || syn_ID == lex_float_id)
	{
		if (syn_ID == lex_int_id)
		{
			functype = op_int;
		}
		else
		{
			functype = op_char;
		}
		syn_nextsymbol();
		if (Return_Func.find(symbol) == Return_Func.end())//��麯���Ƿ��Ѿ������
		{
			//�����û����ͼ�¼����
			Return_Func[symbol] = true;
			functionname = symbol;
		}
		else
		{
			//����ظ������˾ͱ���
			Grammar_Error(err_intorchar, syn_errorline, syn_errorcolon);
			return;
		}
		funcname = symbol;
		syn_identifier();
	}
	else
	{
		Grammar_Error(err_intorchar, syn_errorline, syn_errorcolon);
		return;
	}
	cout << "*�﷨������ͷ����������" << endl;
}
/**
	������˵����  ::= ���������壾;{���������壾;}
**/
void syn_var_dec()//����˵��
{
	cout << "*�﷨����������������ʼ" << endl;
	syn_var_def();//����������
	if (syn_ID != lex_semicolon_id)
	{
		Grammar_Error(err_lacksemicolon, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	while (syn_ID == lex_int_id || syn_ID == lex_char_id)
	{
		syn_record();//��¼��ǰ��symbol
		syn_read_ahead();//Ԥ��һ��symbol
		syn_read_ahead();//��Ԥ��һ��symbol
		if (syn_ID == lex_leftparenthesis_id)//���������˵���Ǻ������壬�Ȼ��ˣ�Ȼ���������˵��
		{
			syn_rollback();
			break;
		}
		//�����Ȼ��ˣ��������б�������
		syn_rollback();
		syn_var_def();
		if (syn_ID != lex_semicolon_id)
		{
			Grammar_Error(err_lacksemicolon, syn_errorline, syn_errorcolon);//���ǣ�����
			return;
		}
		syn_nextsymbol();
	}
	cout << "*�﷨������������������" << endl;
}
/**
	���������壾  ::= �����ͱ�ʶ����(����ʶ����|����ʶ������[�����޷�����������]��){,(����ʶ����|����ʶ������[�����޷�����������]�� )}
**/
void syn_var_def()//��������
{
	cout << "*�﷨�������������忪ʼ" << endl;
	string op, s1, s2, res;//��Ԫʽ
	int remsymID = syn_ID;//��¼��������
	syn_typeidentifier();
	//���ұ������Ƿ��Ѿ������
	if (functionname == "")//ȫ�ֱ�������
	{
		//���������ȫ�ֱ����Ѿ������������
		if (SYN_Const_Var.find(symbol) != SYN_Const_Var.end() || SYN_Global_Var.find(symbol) != SYN_Global_Var.end())
		{
			Grammar_Error(err_var_redefination, syn_errorline, syn_errorcolon);
			return;
		}
		else//���û�ж���������¼����
		{
			SYN_Global_Var[symbol] = true;
		}
	}
	else//�ֲ���������
	{
		//����ֲ������Ѿ���������򱨴�
		if (SYN_Local_Var.find(symbol) != SYN_Local_Var.end())
		{
			Grammar_Error(err_var_redefination, syn_errorline, syn_errorcolon);
			return;
		}
		//���δ�������¼����
		else
		{
			SYN_Local_Var[symbol] = true;
		}
	}
	res = symbol;
	syn_identifier();
	if (syn_ID == lex_leftmiddle_id)//�������[��˵��������
	{
		if (remsymID == lex_int_id)
		{
			op = op_inta;
		}
		else
		{
			op = op_chara;
		}
		syn_nextsymbol();
		s1 = symbol;
		syn_noSignInt();
		if (syn_ID == lex_rightmiddle_id)
		{
			syn_nextsymbol();
		}
		else
		{
			Grammar_Error(err_rightmiddle, syn_errorline, syn_errorcolon);
			return;
		}
		s2 = op_space;
		IR_Output(op, s1, s2, res);
	}
	else//����������
	{
		if (remsymID == lex_int_id)
		{
			op = op_int;
		}
		else
		{
			op = op_char;
		}
		s1 = op_space;
		s2 = op_space;
		IR_Output(op, s1, s2, res);
	}
	while (syn_ID == lex_comma_id)//����ǣ����ظ�������������������
	{
		syn_nextsymbol();
		if (functionname == "")
		{
			if (SYN_Const_Var.find(symbol) != SYN_Const_Var.end() || SYN_Global_Var.find(symbol) != SYN_Global_Var.end())
			{
				Grammar_Error(err_var_redefination, syn_errorline, syn_errorcolon);
				return;
			}
			else
			{
				SYN_Global_Var[symbol] = true;
			}
		}
		else
		{
			if (SYN_Local_Var.find(symbol) != SYN_Local_Var.end())
			{
				Grammar_Error(err_var_redefination, syn_errorline, syn_errorcolon);
				return;
			}
			else
			{
				SYN_Local_Var[symbol] = true;
			}
		}
		res = symbol;
		syn_identifier();
		if (syn_ID == lex_leftmiddle_id)
		{
			if (remsymID == lex_int_id)
			{
				op = op_inta;
			}
			else
			{
				op = op_chara;
			}
			syn_nextsymbol();
			s1 = symbol;
			syn_noSignInt();
			if (syn_ID == lex_rightmiddle_id)
			{
				syn_nextsymbol();
			}
			else
			{
				Grammar_Error(err_rightmiddle, syn_errorline, syn_errorcolon);
				return;
			}
			s2 = op_space;
			IR_Output(op, s1, s2, res);
		}
		else
		{
			if (remsymID == lex_int_id)
			{
				op = op_int;
			}
			else
			{
				op = op_char;
			}
			s1 = op_space;
			s2 = op_space;
			IR_Output(op, s1, s2, res);
		}
	}
	cout << "*�﷨�����������������" << endl;
}
/**
	��������    ::= ��������
**/
void syn_parameter()//����
{
	cout << "*�﷨������������ʼ" << endl;
	syn_parameter_table();//��������
	cout << "*�﷨��������������" << endl;
}
/**
	��������    ::=  �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����}| ���գ�
**/
void syn_parameter_table()//������
{
	cout << "*�﷨������������ʼ" << endl;
	string op, s1, s2, res;//��Ԫʽ
	op = op_para;
	s1 = symbol;
	syn_typeidentifier();//�����ͱ�ʶ��
	res = symbol;
	if (SYN_Local_Var.find(symbol) != SYN_Local_Var.end())//���ֲ������Ƿ����
	{
		Grammar_Error(err_var_redefination, syn_errorline, syn_errorcolon);
		return;
	}
	else
	{
		SYN_Local_Var[res] = true;
	}
	syn_identifier();//����ʶ��
	s2 = op_space;
	IR_Output(op, s1, s2, res);//������Ԫʽ
	while (syn_ID == lex_comma_id)//����ǣ�˵�����в������ظ���������
	{
		syn_nextsymbol();
		op = op_para;
		s1 = symbol;
		syn_typeidentifier();
		res = symbol;
		if (SYN_Local_Var.find(symbol) != SYN_Local_Var.end())
		{
			Grammar_Error(err_var_redefination, syn_errorline, syn_errorcolon);
			return;
		}
		else
		{
			SYN_Local_Var[res] = true;
		}
		syn_identifier();
		s2 = op_space;
		IR_Output(op, s1, s2, res);
	}
	cout << "*�﷨���������������" << endl;
}
/**
	���з���ֵ�������壾  ::=  ������ͷ������(������������)�� ��{����������䣾��}��
**/
void syn_functionwithreturn()//�з���ֵ��������
{
	cout << "*�﷨�������з���ֵ�������忪ʼ" << endl;
	SYN_Local_Var.clear();
	string op, s1, s2, res;//��Ԫʽ
	syn_headFile();//ֱ�ӽ�ͷ������
	op = op_func;
	s1 = functype;
	s2 = op_space;
	res = funcname;
	//syn_functionwithreturnname[funcname]=true;
	if (syn_ID != lex_leftparenthesis_id)//��
	{
		Grammar_Error(err_leftparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	if(syn_ID != lex_rightparenthesis_id)
	{
		//syn_rollback();
		syn_parameter();//������
		IR_Output(op, s1, s2, res);
	}
	else
	{
		//����䣬�޲κ���
		IR_Output(op, s1, s2, res);
	}
	if (syn_ID != lex_rightparenthesis_id)//��
	{
		Grammar_Error(err_rightparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	if (syn_ID != lex_leftbraces_id)//{
	{
		Grammar_Error(err_leftbraces, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	syn_mixsentence();//���������
	if (syn_ID != lex_rightbraces_id)//}
	{
		Grammar_Error(err_rightbraces, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	//getMidCode(op,s1,s2,res);
	IR_Output(op_exits, op_space, op_space, res);
	cout << "*�﷨�������з���ֵ�����������" << endl;
}
/**
	���޷���ֵ�������壾  ::= void����ʶ������(������������)����{����������䣾��}��
**/
void syn_functionwithoutreturn()//�޷���ֵ��������
{
	cout << "*�﷨�������޷���ֵ�������忪ʼ" << endl;
	SYN_Local_Var.clear();
	string op, s1, s2, res;
	if (syn_ID != lex_void_id)
	{
		Grammar_Error(err_void, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	if (No_Return_Func.find(symbol) == No_Return_Func.end())
	{
		No_Return_Func[symbol] = true;
		functionname = symbol;
	}
	else
	{
		Grammar_Error(err_function_redefination, syn_errorline, syn_errorcolon);
		return;
	}
	op = op_func;
	s1 = op_space;
	s2 = op_space;
	res = symbol;
	//syn_identifier();
	syn_nextsymbol();
	if (syn_ID != lex_leftparenthesis_id)//(
	{
		Grammar_Error(err_leftparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	if (syn_ID != lex_rightparenthesis_id)
	{
		//syn_rollback();
		syn_parameter();//������
		IR_Output(op, s1, s2, res);
	}
	else
	{
		//����䣬�޲κ���
		IR_Output(op, s1, s2, res);
	}
	if (syn_ID != lex_rightparenthesis_id)
	{
		Grammar_Error(err_rightparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	if (syn_ID != lex_leftbraces_id)
	{
		Grammar_Error(err_leftbraces, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	syn_mixsentence();

	if (syn_ID != lex_rightbraces_id)
	{
		Grammar_Error(err_rightbraces, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	IR_Output(op_ret, op_space, op_space, op_space);
	IR_Output(op_exits, op_space, op_space, res);
	cout << "*�﷨�������޷���ֵ�����������" << endl;
}
/**
	��������䣾   ::=  �ۣ�����˵�����ݣۣ�����˵�����ݣ�����У�
**/

void syn_mixsentence()//�������
{
	cout << "*�﷨������������俪ʼ" << endl;
	if (syn_ID == lex_const_id)//�������const������볣������
	{
		syn_const_dec();
	}
	if (syn_ID == lex_int_id || syn_ID == lex_float_id || syn_ID == lex_char_id)//�������int��float��char����������
	{
		syn_var_dec();
	}
	syn_sentence_colomn();//�������
	cout << "*�﷨����������������" << endl;
}
/**
	����������    ::= void main��(����)����{����������䣾��}��
**/
void syn_main()//������
{
	cout << "*�﷨��������������ʼ" << endl;
	SYN_Local_Var.clear();
	string op, s1, s2, res;//��Ԫʽ
	if (syn_ID != lex_void_id)
	{
		Grammar_Error(err_void, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	if (syn_ID != lex_main_id)//main
	{
		Grammar_Error(err_main, syn_errorline, syn_errorcolon);
		return;
	}
	string remsymbol = symbol;
	op = op_func;
	s1 = op_space;
	s2 = op_space;
	res = symbol;
	IR_Output(op, s1, s2, res);
	syn_nextsymbol();
	if (syn_ID != lex_leftparenthesis_id)//��
	{
		Grammar_Error(err_leftparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	if (syn_ID != lex_rightparenthesis_id)//��
	{
		Grammar_Error(err_rightparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	if (syn_ID != lex_leftbraces_id)//{
	{
		Grammar_Error(err_leftbraces, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	syn_mixsentence();//���������
	if (syn_ID != lex_rightbraces_id)//}
	{
		Grammar_Error(err_rightbraces, syn_errorline, syn_errorcolon);
		return;
	}
	op = op_exits;
	s1 = op_space;
	s2 = op_space;
	res = remsymbol;
	IR_Output(op, s1, s2, res);
	cout << "*�﷨����������������" << endl;
}

/**
	�����ʽ��    ::= �ۣ������ݣ��{���ӷ�����������}
**/
void syn_expression()//���ʽ
{
	cout << "*�﷨���������ʽ��ʼ" << endl;
	string op, s1, s2, res;//��Ԫʽ
	if (syn_ID == lex_add_id || syn_ID == lex_subtract_id)//+or-
	{
		int remsynID = syn_ID;
		syn_add();
		syn_term();
		if (remsynID == lex_add_id)
		{
			res = glbtemp;
		}
		else
		{
			op = op_sub;
			s1 = op_zero;
			s2 = glbtemp;
			res = genvar();
			IR_Output(op, s1, s2, res);//������Ԫʽ����-x��Ϊ0-x
		}
	}
	else//*or/
	{
		syn_term();//������
		res = glbtemp;
	}
	while (syn_ID == lex_add_id || syn_ID == lex_subtract_id)
	{
		if (syn_ID == lex_add_id)
		{
			op = op_add;
		}
		else
		{
			op = op_sub;
		}
		syn_add();
		syn_term();
		s1 = res;//��һ�����ʽ�Ľ����Ϊ��һ��
		s2 = glbtemp;
		res = genvar();//����һ����ʱ����
		IR_Output(op, s1, s2, res);//������Ԫʽ
	}
	glbtemp = res;
	cout << "*�﷨���������ʽ����" << endl;
}
/**
	���     ::= �����ӣ�{���˷�������������ӣ�}
**/
void syn_term()//��
{
	cout << "*�﷨�������ʼ" << endl;
	string op, s1, s2, res;//��Ԫʽ
	syn_factor();//����
	res = glbtemp;
	while (syn_ID == lex_multiply_id || syn_ID == lex_divide_id)//*or/
	{
		if (syn_ID == lex_multiply_id)
		{
			op = op_mul;
		}
		else
		{
			op = op_div;
		}
		syn_multiply();
		syn_factor();
		s1 = res;
		s2 = glbtemp;

		res = genvar();
		IR_Output(op, s1, s2, res);
	}
	
	glbtemp = res;
	cout << "*�﷨�����������" << endl;
}
/**
	�����ӣ�    ::= ����ʶ����
				  |����ʶ������[�������ʽ����]��
				  |��(�������ʽ����)��
				  |��������
				  |��ʵ����
				  |���ַ���
				  |���з���ֵ����������䣾
**/
void syn_factor()//����
{
	cout << "*�﷨���������ӿ�ʼ" << endl;
	string op, s1, s2, res;
	if (syn_ID == lex_indentifer_id)
	{
		syn_record();//��¼��ǰsymbol
		syn_read_ahead();//Ԥ��һ��symbol
		if (syn_ID == lex_leftmiddle_id)// ����ʶ������[�������ʽ����]��
		{
			syn_rollback();//����
			//�������Ƿ����
			if (SYN_Const_Var.find(symbol) == SYN_Const_Var.end() && SYN_Local_Var.find(symbol) == SYN_Local_Var.end() && SYN_Global_Var.find(symbol) == SYN_Global_Var.end())
			{
				//Ϊ�����򱨴�
				Grammar_Error(err_var_redefination, syn_errorline, syn_errorcolon);
				return;
			}
			syn_identifier();//����ʶ��
			syn_nextsymbol();//[
			syn_expression();//�����ʽ
			if (syn_ID != lex_rightmiddle_id)//]
			{
				Grammar_Error(err_rightmiddle, syn_errorline, syn_errorcolon);
				return;
			}
			syn_nextsymbol();
		}
		else if (syn_ID == lex_leftparenthesis_id) //���з���ֵ����������䣾
		{
			syn_rollback();//����
			/*if(syn_functionwithreturnname.find(symbol)==syn_functionwithreturnname.end());
			{
				//�������Ϊ������δ����
				errorcontent(err_function_undefination,syn_errorline,syn_errorcolon);
				return;
			}*/
			syn_functionwithreturn_recall();//���з���ֵ�����������
		}
		else
		{
			syn_rollback();//����
			glbtemp = symbol;
			syn_identifier();//����ʶ��
		}
	}
	else if (syn_ID == lex_leftparenthesis_id)//��(�������ʽ����)��
	{
		syn_nextsymbol();//��
		syn_expression();//�����ʽ
		if (syn_ID != lex_rightparenthesis_id)
		{
			Grammar_Error(err_rightparenthesis, syn_errorline, syn_errorcolon);
			return;
		}
		syn_nextsymbol();
	}
	else if (syn_ID == lex_character_id)//���ַ���
	{
		glbtemp = symbol;
		syn_character();
	}
	else
	{
		if (syn_ID == lex_NotZeroHeadNum_id) //��������|��ʵ����
		{
			glbtemp = symbol;
			syn_int();
		}
		if (syn_ID == lex_dot_id)
		{
			glbtemp = symbol;
			syn_Fractional();//С������
		}
	}
	cout << "*�﷨���������ӽ���" << endl;
}
/**
	����䣾    ::= ��������䣾
				 ����ѭ����䣾
				 |�������䣾
				 ��������䣾;
				 ����д��䣾;
				 ����������䣾;
				 | ��{��������У���}��
				 ������ֵ��䣾;
				 �����з���ֵ����������䣾;
				 |���޷���ֵ����������䣾;
				 �����գ�;
**/
void syn_sentence()//���
{
	cout << "*�﷨��������俪ʼ" << endl;
	string op, s1, s2, res;
	if (syn_ID == lex_if_id)//��������䣾
	{
		syn_condition_sen();
	}
	else if (syn_ID == lex_while_id || syn_ID == lex_do_id || syn_ID == lex_for_id)// ��ѭ����䣾
	{
		syn_loop_sen();
	}
	else if (syn_ID == lex_switch_id)//�������䣾
	{
		syn_situation_sen();
	}
	else if (syn_ID == lex_leftbraces_id)//��{��������У���}��
	{
		if (syn_ID != lex_leftbraces_id)//{
		{
			Grammar_Error(err_leftbraces, syn_errorline, syn_errorcolon);
			return;
		}
		syn_nextsymbol();
		syn_sentence_colomn();//��<�����>
		if (syn_ID != lex_rightbraces_id)//}
		{
			Grammar_Error(err_rightbraces, syn_errorline, syn_errorcolon);
			return;
		}
		syn_nextsymbol();
	}
	else if (syn_ID == lex_scanf_id)//������䣾;
	{
		syn_scanf();
		if (syn_ID != lex_semicolon_id)//��
		{
			Grammar_Error(err_semicolon, syn_errorline, syn_errorcolon);
			return;
		}
		syn_nextsymbol();
	}
	else if (syn_ID == lex_printf_id)//��д��䣾;
	{
		syn_printf();
		if (syn_ID != lex_semicolon_id)//��
		{
			Grammar_Error(err_semicolon, syn_errorline, syn_errorcolon);
			return;
		}
		syn_nextsymbol();
	}
	else if (syn_ID == lex_return_id)//��������䣾;
	{
		syn_return();
		if (syn_ID != lex_semicolon_id)
		{
			Grammar_Error(err_semicolon, syn_errorline, syn_errorcolon);
			return;
		}
		syn_nextsymbol();
	}
	else if (syn_ID == lex_indentifer_id)
	{
		syn_record();
		syn_read_ahead();
		if (syn_ID == lex_equal_id || syn_ID == lex_leftmiddle_id)//����ֵ��䣾;
		{
			syn_rollback();//����
			//����������ڱ�������ͳ��������ж�û���֣��򱨴�δ����
			if (SYN_Const_Var.find(symbol) == SYN_Const_Var.end() && SYN_Local_Var.find(symbol) == SYN_Local_Var.end() && SYN_Global_Var.find(symbol) == SYN_Global_Var.end())
			{
				Grammar_Error(err_var_undefination, syn_errorline, syn_errorcolon);
				return;
			}
			if (SYN_Const_Var.find(symbol) != SYN_Const_Var.end())//��������ֵ������
			{
				Grammar_Error(err_const_assign, syn_errorline, syn_errorcolon);
				return;
			}
			syn_assign_sen();//��<��ֵ���>
			if (syn_ID != lex_semicolon_id)
			{
				Grammar_Error(err_semicolon, syn_errorline, syn_errorcolon);
				return;
			}
			syn_nextsymbol();
		}
		else if (Return_Func.find(symbol) != Return_Func.end())//���з���ֵ����������䣾;
		{
			syn_functionwithreturn_recall();
			if (syn_ID != lex_semicolon_id)
			{
				Grammar_Error(err_semicolon, syn_errorline, syn_errorcolon);
				return;
			}
			syn_nextsymbol();
		}
		else if (No_Return_Func.find(symbol) != No_Return_Func.end())//���޷���ֵ����������䣾;
		{
			syn_functionwithoutreturn_recall();
			if (syn_ID != lex_semicolon_id)
			{
				Grammar_Error(err_semicolon, syn_errorline, syn_errorcolon);
				return;
			}
			syn_nextsymbol();
		}
		else
		{
			///��
		}
	}
	cout << "*�﷨������������" << endl;
}
/**
	������У�   ::= ������䣾��
**/
void syn_sentence_colomn()//�����
{
	cout << "*�﷨����������п�ʼ" << endl;
	while (syn_ID != lex_rightbraces_id)
	{
		syn_sentence();
	}
	cout << "*�﷨����������н���" << endl;
}
/**
	����ֵ��䣾   ::=  ����ʶ�����������ʽ��
					 |����ʶ������[�������ʽ����]��=�����ʽ��
**/
void syn_assign_sen()//��ֵ���
{
	cout << "*�﷨��������ֵ��俪ʼ" << endl;
	string op, s1, s2, res;
	if (syn_ID != lex_indentifer_id)
	{
		Grammar_Error(err_identifier, syn_errorline, syn_errorcolon);
		return;
	}
	//����������ڱ�������ͳ��������ж�û���֣��򱨴�δ����
	if (SYN_Const_Var.find(symbol) == SYN_Const_Var.end() && SYN_Local_Var.find(symbol) == SYN_Local_Var.end() && SYN_Global_Var.find(symbol) == SYN_Global_Var.end())
	{
		Grammar_Error(err_var_undefination, syn_errorline, syn_errorcolon);
		return;
	}
	//�����������ֵ�򱨴�
	if (SYN_Const_Var.find(symbol) != SYN_Const_Var.end())
	{
		Grammar_Error(err_const_assign, syn_errorline, syn_errorcolon);
		return;
	}
	res = symbol;
	syn_identifier();
	if (syn_ID == lex_equal_id)
	{
		syn_record();
		syn_read_ahead();
		syn_read_ahead();
		if(syn_ID == lex_leftmiddle_id)//�����鸳ֵ������
		{
			syn_rollback();
			op = op_arryass;	
			syn_nextsymbol();
			s2 = symbol;
			syn_expression();
			s1 = glbtemp;
			IR_Output(op, s1, s2, res);//������Ԫʽ
		}
		else//����һֵ��������
		{
			syn_rollback();
			op = op_ass;
			syn_nextsymbol();
			syn_expression();
			s1 = glbtemp;
			s2 = op_space;
			IR_Output(op, s1, s2, res);//������Ԫʽ
		}

	}
	else if (syn_ID == lex_leftmiddle_id)//��ֵ������
	{
		op = op_assarry;
		syn_nextsymbol();
		syn_expression();
		s2 = glbtemp;
		if (syn_ID != lex_rightmiddle_id)
		{
			Grammar_Error(err_rightmiddle, syn_errorline, syn_errorcolon);
			return;
		}
		syn_nextsymbol();
		syn_nextsymbol();
		syn_expression();
		s1 = glbtemp;
		IR_Output(op, s1, s2, res);//������Ԫʽ
	}
	cout << "*�﷨��������ֵ������" << endl;
}
/**
	��������䣾  ::=  if ��(������������)������䣾��else����䣾��
**/
string cop, cs1, cs2;
void syn_condition_sen()//�������
{
	cout << "*�﷨������������俪ʼ" << endl;
	string op, s1, s2, res;//��Ԫʽ
	//LABEL�Ĺ����Ƕ���������ŵ����ͣ����������ŵĶ����Ժ�ƫ�������ɸ����������λ��ȷ����
	string label1 = genlable();//����һ���µ�label
	string label2 = genlable();//����һ���µ�label
	if (syn_ID != lex_if_id)//if
	{
		Grammar_Error(err_if, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	if (syn_ID != lex_leftparenthesis_id)//��
	{
		Grammar_Error(err_leftparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	syn_condition();//������
	/// skip if
	op = cop;
	s1 = cs1;
	s2 = cs2;
	res = label1;
	IR_Output(op, s1, s2, res);
	if (syn_ID != lex_rightparenthesis_id)//��
	{
		Grammar_Error(err_rightparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	syn_sentence();
	/// skip else
	op = op_jmp;
	s1 = op_space;
	s2 = op_space;
	res = label2;
	IR_Output(op, s1, s2, res);
	/// set label1
	op = op_lab;
	s1 = op_space;
	s2 = op_space;
	res = label1;
	IR_Output(op, s1, s2, res);
	if (syn_ID == lex_else_id)
	{
		syn_nextsymbol();
		syn_sentence();
	}
	/// set label2
	op = op_lab;
	s1 = op_space;
	s2 = op_space;
	res = label2;
	IR_Output(op, s1, s2, res);
	cout << "*�﷨����������������" << endl;
}
/**
	��������    ::=  �����ʽ������ϵ������������ʽ��
				  �������ʽ�� //���ʽΪ0����Ϊ�٣�����Ϊ��
**/
void syn_condition()//����
{
	cout << "*�﷨������������ʼ" << endl;
	string op, s1, s2, res;
	syn_expression();
	s1 = glbtemp;
	if (syn_ID == lex_less_id || syn_ID == lex_lessequal_id || syn_ID == lex_greater_id || syn_ID == lex_greaterequal_id || syn_ID == lex_equalequal_id || syn_ID == lex_notequal_id)
	{
		if (syn_ID == lex_less_id)//<
		{
			op = op_bet;
			syn_relation();
			syn_expression();
			s2 = glbtemp;
			res = op_space;
		}
		else if (syn_ID == lex_lessequal_id)//<=
		{
			op = op_bt;
			syn_relation();
			syn_expression();
			s2 = glbtemp;
			res = op_space;
		}
		else if (syn_ID == lex_greater_id)//>
		{
			op = op_set;
			syn_relation();
			syn_expression();
			s2 = glbtemp;
			res = op_space;
		}
		else if (syn_ID == lex_greaterequal_id)//>=
		{
			op = op_st;
			syn_relation();
			syn_expression();
			s2 = glbtemp;
			res = op_space;
		}
		else if (syn_ID == lex_notequal_id)// !=
		{
			//op=op_neq;
			op = op_eql;
			syn_relation();
			syn_expression();
			s2 = glbtemp;
			res = op_space;
		}
		else if (syn_ID == lex_equalequal_id)//==
		{
			//op=op_eql;
			op = op_neq;
			syn_relation();
			syn_expression();
			s2 = glbtemp;
			res = op_space;
		}
	}
	else
	{
		op = mips_beq;
		s2 = op_zero;
		res = op_space;
	}
	cop = op;
	cs1 = s1;
	cs2 = s2;
	cout << "*�﷨��������������" << endl;
}
/**
	��for��ֵ��䣾   ::=  ����ʶ�����������ʽ��
					 |����ʶ������[�������ʽ����]��=�����ʽ��
**/
string forop, fors1, fors2, forres;
void syn_forassign()//for��ֵ���
{
	cout << "*�﷨������for��ֵ��俪ʼ" << endl;
	string op, s1, s2, res;
	if (syn_ID != lex_indentifer_id)
	{
		Grammar_Error(err_identifier, syn_errorline, syn_errorcolon);
		return;
	}
	//����������ڱ�������ͳ��������ж�û���֣��򱨴�δ����
	if (SYN_Const_Var.find(symbol) == SYN_Const_Var.end() && SYN_Local_Var.find(symbol) == SYN_Local_Var.end() && SYN_Global_Var.find(symbol) == SYN_Global_Var.end())
	{
		Grammar_Error(err_var_undefination, syn_errorline, syn_errorcolon);
		return;
	}
	// �����������ֵ�򱨴�
	if (SYN_Const_Var.find(symbol) != SYN_Const_Var.end())
	{
		Grammar_Error(err_const_assign, syn_errorline, syn_errorcolon);
		return;
	}
	res = symbol;
	syn_identifier();
	if (syn_ID == lex_equal_id)
	{
		op = op_ass;
		syn_nextsymbol();
		syn_expression();
		s1 = glbtemp;
		s2 = op_space;
	}
	else if (syn_ID == lex_leftmiddle_id)//��ֵ������
	{
		op = op_assarry;
		syn_nextsymbol();
		syn_expression();
		s2 = glbtemp;
		if (syn_ID != lex_rightmiddle_id)
		{
			Grammar_Error(err_rightmiddle, syn_errorline, syn_errorcolon);
			return;
		}
		syn_nextsymbol();
		if (syn_ID != lex_equal_id)
		{
			Grammar_Error(err_ass, syn_errorline, syn_errorcolon);
			return;
		}
		syn_nextsymbol();
		syn_expression();
		s1 = glbtemp;
	}
	forop = op;
	fors1 = s1;
	fors2 = s2;
	forres = res;
	cout << "*�﷨������for��ֵ������" << endl;
}
/**
	��ѭ����䣾   ::=  while ��(������������)������䣾
						|do����䣾while ��(������������)��
**/
void syn_loop_sen()//ѭ�����
{
	cout << "*�﷨������ѭ����俪ʼ" << endl;
	string op, s1, s2, res;
	string label1 = genlable();//�����µ�label
	string label2 = genlable();//�����µ�label
	/// while ��(������������)������䣾
	if (syn_ID == lex_while_id)//while
	{
		if (syn_ID != lex_while_id)
		{
			Grammar_Error(err_while, syn_errorline, syn_errorcolon);
			return;
		}
		syn_nextsymbol();
		if (syn_ID != lex_leftparenthesis_id)
		{
			Grammar_Error(err_leftparenthesis, syn_errorline, syn_errorcolon);
			return;
		}
		//set label2
		IR_Output(op_lab, op_space, op_space, label2);
		syn_nextsymbol();
		syn_condition();
		op = cop;
		s1 = cs1;
		s2 = cs2;
		res = label1;
		//jump to label1
		IR_Output(op, s1, s2, res);
		if (syn_ID != lex_rightparenthesis_id)
		{
			Grammar_Error(err_rightparenthesis, syn_errorline, syn_errorcolon);
			return;
		}
		syn_nextsymbol();
		syn_sentence();/// ��<���>
		//jump to label2
		IR_Output(op_jmp, op_space, op_space, label2);//����ѭ��
		//set label1
		IR_Output(op_lab, op_space, op_space, label1);//����ѭ��
	}
	/// do����䣾while ��(������������)��
	else if (syn_ID == lex_do_id)//do
	{
		if (syn_ID != lex_do_id)
		{
			Grammar_Error(err_do, syn_errorline, syn_errorcolon);
			return;
		}
		IR_Output(op_lab, op_space, op_space, label1);//label1
		syn_nextsymbol();
		syn_sentence();
		if (syn_ID != lex_leftparenthesis_id)
		{
			Grammar_Error(err_leftparenthesis, syn_errorline, syn_errorcolon);
			return;
		}
		syn_nextsymbol();
		syn_condition();//����
		op = cop;
		s1 = cs1;
		s2 = cs2;
		res = label2;
		IR_Output(op, s1, s2, res);//break
		IR_Output(op_jmp, op_space, op_space, label1);//continue
		IR_Output(op_lab, op_space, op_space, label2);//label2
		if (syn_ID != lex_rightparenthesis_id)
		{
			Grammar_Error(err_rightparenthesis, syn_errorline, syn_errorcolon);
			return;
		}
		syn_nextsymbol();
	}
	cout << "*�﷨������ѭ��������" << endl;
}
/**
	�������䣾  ::=  switch ��(�������ʽ����)�� ��{�����������ȱʡ�� ��}��
**/

string switchtemp;
void syn_situation_sen()//������
{
	cout << "*�﷨�����������俪ʼ" << endl;
	if (syn_ID != lex_switch_id)//switch
	{
		Grammar_Error(err_switch, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	if (syn_ID != lex_leftparenthesis_id)//��
	{
		Grammar_Error(err_leftparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	syn_expression();
	switchtemp = glbtemp;
	if (syn_ID != lex_rightparenthesis_id)//��
	{
		Grammar_Error(err_rightparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	if (syn_ID != lex_leftbraces_id)//{
	{
		Grammar_Error(err_leftbraces, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	syn_situation_table();//�������
	if (syn_ID == lex_default_id)
	{
		syn_default();//default
	}
	if (syn_ID != lex_rightbraces_id)//}
	{
		Grammar_Error(err_rightbraces, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	cout << "*�﷨���������������" << endl;
}
/**
	�������   ::=  ���������䣾{���������䣾}
**/
void syn_situation_table()//�����
{
	cout << "*�﷨�����������ʼ" << endl;
	syn_situation_sen_son();
	while (1)
	{
		syn_situation_sen_son();
		if (syn_ID == lex_default_id || syn_ID == lex_rightbraces_id)//��ʱ��ֹ
		{
			break;
		}
	}
	cout << "*�﷨��������������" << endl;
}
/**
	���������䣾  ::=  case����ö�ٳ�����������䣾
**/
void syn_situation_sen_son()//��������
{
	cout << "*�﷨�������������俪ʼ" << endl;
	if (syn_ID != lex_case_id)//case
	{
		Grammar_Error(err_case, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	string label = genlable();
	IR_Output(op_neq, symbol, switchtemp, label);
	syn_enum();//����ö�ٳ���
	if (syn_ID != lex_colon_id)//��
	{
		Grammar_Error(err_colon, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	syn_sentence();//�����
	IR_Output(op_lab, op_space, op_space, label);
	cout << "*�﷨�����������������" << endl;
}
/**
	��ȱʡ��   ::=  default : ����䣾
				  |���գ�
**/
void syn_default()//ȱʡ
{
	cout << "*�﷨������ȱʡ��ʼ" << endl;
	if (syn_ID != lex_default_id)
	{
		return;
	}
	syn_nextsymbol();
	if (syn_ID != lex_colon_id)//:
	{
		Grammar_Error(err_colon, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	syn_sentence();//�����
	cout << "*�﷨������ȱʡ����" << endl;
}
/**
	���з���ֵ����������䣾 ::= ����ʶ������(����ֵ��������)��
**/
void syn_functionwithreturn_recall()//�з���ֵ�����������
{
	cout << "*�﷨�������з���ֵ����������俪ʼ" << endl;
	string op, s1, s2, res;//��Ԫʽ
	if (syn_ID != lex_indentifer_id)
	{
		Grammar_Error(err_identifier, syn_errorline, syn_errorcolon);
		return;
	}
	op = op_call;
	s1 = symbol;
	s2 = op_space;
	res = genvar();//������ʱ�����洢����ֵ
	syn_nextsymbol();
	if (syn_ID != lex_leftparenthesis_id)//��
	{
		Grammar_Error(err_leftparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	syn_value_parameter_table();//��ֵ������
	if (syn_ID != lex_rightparenthesis_id)//��
	{
		Grammar_Error(err_rightparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	IR_Output(op, s1, s2, res);//������Ԫʽ
	glbtemp = res;//���������ȫ����ʱ����
	cout << "*�﷨�������з���ֵ��������������" << endl;
}
/**
	���޷���ֵ����������䣾 ::= ����ʶ������(����ֵ��������)��
**/
void syn_functionwithoutreturn_recall()//�޷���ֵ�����������
{
	cout << "*�﷨�������޷���ֵ����������俪ʼ" << endl;
	string op, s1, s2, res;//��Ԫʽ
	if (syn_ID != lex_indentifer_id)
	{
		Grammar_Error(err_identifier, syn_errorline, syn_errorcolon);
		return;
	}
	op = op_call;
	s1 = symbol;
	s2 = op_space;
	res = op_space;
	if (syn_ID != lex_leftparenthesis_id)//��
	{
		Grammar_Error(err_leftparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	syn_value_parameter_table();//��ֵ������
	if (syn_ID != lex_rightparenthesis_id)//��
	{
		Grammar_Error(err_rightparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	IR_Output(op, s1, s2, res);//������Ԫʽ
	cout << "*�﷨�������޷���ֵ��������������" << endl;
}
/**
	��ֵ������   ::= �����ʽ��{,�����ʽ��}
					�����գ�
**/
stack<string>parastack;
void syn_value_parameter_table()//ֵ������//û�д���<��>�����
{
	cout << "*�﷨������ֵ������ʼ" << endl;
	string op, s1, s2, res;
	op = op_callpara;
	s1 = op_space;
	s2 = op_space;
	syn_expression();//��<���ʽ>
	res = glbtemp;//���������ȫ����ʱ����
	parastack.push(res);
	while (syn_ID == lex_comma_id)//��
	{
		syn_nextsymbol();
		syn_expression();
		res = glbtemp;
		parastack.push(res);
	}
	while (!parastack.empty())//������������ѹջ
	{
		res = parastack.top();
		parastack.pop();
		IR_Output(op, s1, s2, res);//������Ԫʽ
	}
	cout << "*�﷨������ֵ���������" << endl;
}
/**
	������䣾    ::=  scanf ��(������ʶ����{,����ʶ����}��)��
**/
void syn_scanf()//�����
{
	cout << "*�﷨����������俪ʼ" << endl;
	string op, s1, s2, res;
	if (syn_ID != lex_scanf_id)//scanf
	{
		Grammar_Error(err_scanf, syn_errorline, syn_errorcolon);
		return;
	}
	op = op_scanf;
	syn_nextsymbol();
	if (syn_ID != lex_leftparenthesis_id)//��
	{
		Grammar_Error(err_leftparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	//�������Ƿ�����������û������򱨴�
	if (SYN_Const_Var.find(symbol) == SYN_Const_Var.end() && SYN_Local_Var.find(symbol) == SYN_Local_Var.end() && SYN_Global_Var.find(symbol) == SYN_Global_Var.end())
	{
		Grammar_Error(err_var_undefination, syn_errorline, syn_errorcolon);
		return;
	}
	//�����������ֵ�򱨴�
	if (SYN_Const_Var.find(symbol) != SYN_Const_Var.end())
	{
		Grammar_Error(err_const_assign, syn_errorline, syn_errorcolon);
		return;
	}
	s1 = op_space;
	s2 = op_space;
	res = symbol;
	syn_identifier();
	IR_Output(op, s1, s2, res);//������Ԫʽ
	while (syn_ID == lex_comma_id)//����ǣ�˵���ж������������ظ���������
	{
		syn_nextsymbol();
		res = symbol;
		syn_identifier();
		IR_Output(op, s1, s2, res);
	}
	if (syn_ID != lex_rightparenthesis_id)//��
	{
		Grammar_Error(err_rightparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	cout << "*�﷨��������������" << endl;
}
/**
	��д��䣾    ::= printf ��(�� ���ַ�����,�����ʽ�� ��)��
				  | printf ��(�����ַ����� ��)��
				  | printf ��(�������ʽ����)��
**/
void syn_printf()//д���
{
	cout << "*�﷨������д��俪ʼ" << endl;
	string op, s1, s2, res;
	if (syn_ID != lex_printf_id)//printf
	{
		Grammar_Error(err_printf, syn_errorline, syn_errorcolon);
		return;
	}
	op = op_printf;
	syn_nextsymbol();
	if (syn_ID != lex_leftparenthesis_id)//��
	{
		Grammar_Error(err_leftparenthesis, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	if (syn_ID == lex_string_id)//string
	{
		s1 = symbol;
		if (String_List.find(symbol) == String_List.end())
		{
			String_List[symbol] = genstring();
		}
		syn_string();
		if (syn_ID == lex_comma_id)
		{
			syn_nextsymbol();
			syn_expression();
			s2 = glbtemp;
			if (syn_ID != lex_rightparenthesis_id)//��
			{
				Grammar_Error(err_rightparenthesis, syn_errorline, syn_errorcolon);
				return;
			}
			syn_nextsymbol();
		}
		else if (syn_ID == lex_rightparenthesis_id)//��
		{
			s2 = op_space;
			syn_nextsymbol();
		}
		else
		{
			Grammar_Error(err_rightparenthesis, syn_errorline, syn_errorcolon);
			return;
		}
	}
	else
	{
		s1 = op_space;
		syn_expression();
		s2 = glbtemp;
		if (syn_ID != lex_rightparenthesis_id)//��
		{
			Grammar_Error(err_rightparenthesis, syn_errorline, syn_errorcolon);
			return;
		}
		syn_nextsymbol();
	}
	IR_Output(op, s1, s2, res);//������Ԫʽ
	cout << "*�﷨������д������" << endl;
}
/**
	��������䣾   ::=  return[��(�������ʽ����)��]

	��������䣾   ::=  return[�����ʽ��]
**/
void syn_return()//�������
{
	cout << "*�﷨������������俪ʼ" << endl;
	string op, s1, s2, res;
	if (syn_ID != lex_return_id)//return
	{
		Grammar_Error(err_return, syn_errorline, syn_errorcolon);
		return;
	}
	op = op_ret;
	s1 = op_space;
	s2 = op_space;
	syn_nextsymbol();
	//return �����ʽ����
	if (syn_ID == lex_leftparenthesis_id)//��
	{
		syn_nextsymbol();
		syn_expression();
		res = glbtemp;//��ֵ����ȫ����ʱ����
		if (syn_ID != lex_rightparenthesis_id)//��
		{
			Grammar_Error(err_rightparenthesis, syn_errorline, syn_errorcolon);
			return;
		}
		syn_nextsymbol();
	}
	//return ���ʽ��
	else if (syn_ID == lex_indentifer_id || syn_ID == lex_NotZeroHeadNum_id || syn_ID == lex_character_id)
	{
		syn_expression();
		res = glbtemp;//��ֵ����ȫ����ʱ����
		if (syn_ID != lex_rightparenthesis_id)//��
		{
			Grammar_Error(err_rightparenthesis, syn_errorline, syn_errorcolon);
			return;
		}
		syn_nextsymbol();
	}
	//return
	else if (syn_ID != lex_semicolon_id)//��
	{
		Grammar_Error(err_semicolon, syn_errorline, syn_errorcolon);
		return;
	}
	//return��
	else
	{
		res = op_space;
	}
	IR_Output(op, s1, s2, res);
	cout << "*�﷨����������������" << endl;
}
/**
	���ӷ��������  ::= +��-
**/
void syn_add()//�ӷ������
{
	cout << "*�﷨�������ӷ��������ʼ" << endl;
	if (syn_ID != lex_add_id && syn_ID != lex_subtract_id)
	{
		Grammar_Error(err_addorsub, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	cout << "*�﷨�������ӷ����������" << endl;
}
/**
	���˷��������  ::= *��/
**/
void syn_multiply()//�˷������
{
	cout << "*�﷨�������˷��������ʼ" << endl;
	if (syn_ID != lex_multiply_id && syn_ID != lex_divide_id)
	{
		Grammar_Error(err_mulordiv, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	cout << "*�﷨�������˷����������" << endl;
}
/**
	����ϵ�������  ::=  <��<=��>��>=��!=��==
**/
void syn_relation()//��ϵ�����
{
	cout << "*�﷨��������ϵ�������ʼ" << endl;
	if (syn_ID != lex_less_id && syn_ID != lex_lessequal_id && syn_ID != lex_greater_id && syn_ID != lex_greaterequal_id && syn_ID != lex_notequal_id && syn_ID != lex_equalequal_id)
	{
		Grammar_Error(err_relation, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	cout << "*�﷨��������ϵ���������" << endl;
}
/**
	����ĸ��   ::= �ߣ�a����������z��A����������Z
**/
void syn_letter()//��ĸ
{
	cout << "*�﷨��������ĸ��ʼ" << endl;
	if (syn_ID != lex_letter_id)
	{
		Grammar_Error(err_letter, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	cout << "*�﷨��������ĸ����" << endl;
}
/**
	�����֣�   ::= �������������֣�
**/
void syn_digit()//����
{
	cout << "*�﷨���������ֿ�ʼ" << endl;
	if (syn_ID != lex_ZeroToNine)
	{
		Grammar_Error(err_digit, syn_errorline, syn_errorcolon);
		return;
	}
	if (symbol == "0")
	{
		syn_nextsymbol();
	}
	else
	{
		syn_notZeroDigit();
	}
	cout << "*�﷨���������ֽ���" << endl;
}
/**
	���������֣�  ::= ��������������
**/

void syn_notZeroDigit()//��������
{
	cout << "*�﷨�������������ֿ�ʼ" << endl;
	if (syn_ID != lex_ZeroToNine)
	{
		Grammar_Error(err_digit, syn_errorline, syn_errorcolon);
		return;
	}
	if (!(symbol >= "1" && symbol <= "9"))
	{
		Grammar_Error(err_notzero, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	cout << "*�﷨�������������ֽ���" << endl;
}
/**
	���ַ���    ::=  '���ӷ��������'��'���˷��������'��'����ĸ��'��'�����֣�'
**/
void syn_character()//�ַ�
{
	cout << "*�﷨�������ַ���ʼ" << endl;
	if (syn_ID != lex_singlemark_id)
	{
		Grammar_Error(err_singlemark, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	if (syn_ID == lex_add_id)
	{
		syn_add();
	}
	else if (syn_ID == lex_multiply_id)
	{
		syn_multiply();
	}
	else if (syn_ID == lex_letter_id)
	{
		syn_letter();
	}
	else if (syn_ID == lex_ZeroToNine)
	{
		syn_digit();
	}
	else
	{
		Grammar_Error(err_character, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	if (syn_ID != lex_singlemark_id)
	{
		Grammar_Error(err_singlemark, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	cout << "*�﷨�������ַ�����" << endl;
}
/**
	���ַ�����   ::=  "��ʮ���Ʊ���Ϊ32,33,35-126��ASCII�ַ���"
**/
void syn_string()//�ַ���
{
	cout << "*�﷨�������ַ�����ʼ" << endl;
	if (syn_ID != lex_string_id)
	{
		Grammar_Error(err_string, syn_errorline, syn_errorcolon);
		return;
	}
	syn_nextsymbol();
	cout << "*�﷨�������ַ�������" << endl;
}
/**
	���޷���������  ::= ���������֣��������֣���
	###���޷���������  ::= DIGIT_SYMID
**/
void syn_noSignInt()//�޷�������
{
	cout << "*�﷨�������޷���������ʼ" << endl;
	if (syn_ID == lex_NotZeroHeadNum_id)
	{
		syn_nextsymbol();
	}
	else
	{
		Grammar_Error(err_number, syn_errorline, syn_errorcolon);
		return;
	}
	cout << "*�﷨�������޷�����������" << endl;
}
/**
	��������        ::= �ۣ������ݣ��޷�������������
**/
void syn_int()//����
{
	cout << "*�﷨�������޷���������ʼ" << endl;
	if (syn_ID == lex_add_id || syn_ID == lex_subtract_id)
	{
		syn_nextsymbol();
		syn_noSignInt();
	}
	else if (syn_ID == lex_NotZeroHeadNum_id)
	{
		syn_noSignInt();//���޷�������
	}
	else
	{
		Grammar_Error(err_int, syn_errorline, syn_errorcolon);
		return;
	}
	cout << "*�﷨�������޷�����������" << endl;
}
/**
	��С�����֣�    ::= �����֣��������֣��������գ�
**/
void syn_Fractional()//С������
{
	cout << "*�﷨������С�����ֿ�ʼ" << endl;
	if (syn_ID == lex_ZeroHeadNum_id)
	{
		syn_nextsymbol();
	}
	else if (syn_ID == lex_NotZeroHeadNum_id)
	{
		syn_nextsymbol();
	}
	else
	{
		syn_nextsymbol();
	}
	cout << "*�﷨������С�����ֽ���" << endl;
}
/**
	��ʵ����        ::= �ۣ������ݣ�������[.��С�����֣�]
**/
void syn_realNumber()//ʵ��
{
	cout << "*�﷨������ʵ����ʼ" << endl;
	if (syn_ID == lex_add_id)
	{
		syn_add();
	}
	syn_int();
	if (syn_ID == lex_dot_id)
	{
		syn_nextsymbol();
		syn_Fractional();
	}
	cout << "*�﷨������ʵ������" << endl;
}
/**
	����ʶ����    ::=  ����ĸ��������ĸ���������֣���
**/
void syn_identifier()//��ʶ��
{
	cout << "*�﷨��������ʶ����ʼ" << endl;
	if (syn_ID == lex_indentifer_id)
	{
		syn_nextsymbol();
	}
	else
	{
		Grammar_Error(err_identifier, syn_errorline, syn_errorcolon);
		return;
	}
	cout << "*�﷨��������ʶ������" << endl;
}
/**
	����ö�ٳ�����   ::=  ��������|���ַ���
**/
void syn_enum()//��ö�ٳ���
{
	cout << "*�﷨��������ö�ٳ�����ʼ" << endl;
	if (syn_ID == lex_NotZeroHeadNum_id)
	{
		syn_int();
	}
	else if (syn_ID == lex_add_id || syn_ID == lex_multiply_id || syn_ID == lex_letter_id || syn_ID == lex_ZeroToNine)
	{
		syn_letter();
	}
	else
	{
		Grammar_Error(err_enumerabel, syn_errorline, syn_errorcolon);
		return;
	}
	cout << "*�﷨��������ö�ٳ�������" << endl;
}
/**
	�����ͱ�ʶ����      ::=  int | char
**/
void syn_typeidentifier()//��ʶ��
{
	cout << "*�﷨���������ͱ�ʶ����ʼ" << endl;
	if (syn_ID == lex_int_id || syn_ID == lex_float_id || syn_ID == lex_char_id)
	{
		syn_nextsymbol();
	}
	else
	{
		Grammar_Error(err_intorchar, syn_errorline, syn_errorcolon);
		return;
	}
	cout << "*�﷨���������ͱ�ʶ������" << endl;
}