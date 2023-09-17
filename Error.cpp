#include "head.h"
#include "Error.h"

using namespace std;

int width = 5;
bool Error_Flag = false;

void Grammar_Error(int err_ID, int err_line, int err_coloumn)
{
	Error_Flag = true;
	cout << setw(width) << left << "Error: " << setw(width) << left << err_ID;
	cout << setw(width) << left << "Line: " << setw(width) << left << err_line;
	cout << setw(width) << left << "Position: " << setw(width) << left << err_coloumn;
	cout << setw(width) << left << "Content: ";
	switch (err_ID)
	{
	case err_const_redefination: //�㶨�����ظ�����
		cout << "constant redefinition!" << endl;
		break;
	case err_var_redefination:  //�����ظ�����
		cout << "variable redefinition!" << endl;
		break;
	case err_function_redefination:  //�����ظ�����
		cout << "funtion redefinition!" << endl;
		break;
	case err_lacksemicolon:  //ȱ���ֺ�
		cout << "lack of \';\' !" << endl;
		break;
	case err_var_undefination:  //����δ����
		cout << "variable undefined!" << endl;
		break;
	case err_function_undefination:  //����δ����
		cout << "funtion undefined!" << endl;
		break;
	case err_const_assign:  //��ͼ���ĳ�����ֵ
		cout << "constant reassignment!" << endl;
		break;
	case err_addorsub:  //ȱ���Ӽ��������
		cout << "lack of \'+|-\' !" << endl;
		break;
	case err_mulordiv:  //ȱ���˳��������
		cout << "lack of \'*|/\' !" << endl;
		break;
	case err_relation:  //ȱ����ϵ�����
		cout << "lack of \'relation\' !" << endl;
		break;
	case err_const:  //ȱ���������������
		cout << "lack of \'const\' !" << endl;
		break;
	case err_intorchar:  //�������ȱ��int����char��ʶ��
		cout << "lack of \'int | char\' !" << endl;
		break;
	case err_ass:  //ȱ�����������
		cout << "lack of \'=\' !" << endl;
		break;
	case err_number:  //����δ��⵽
		cout << "should be a number !" << endl;
		break;
	case err_int:  //intδ��⵽
		cout << "should be an int type !" << endl;
		break;
	case err_identifier:  //��ʶ��δ��⵽
		cout << "should be an identifier !" << endl;
		break;
	case err_leftparenthesis:  //ȱ�٣�
		cout << "lack of \'(\' !" << endl;
		break;
	case err_rightparenthesis:  //ȱ�� ��
		cout << "lack of \')\' !" << endl;
		break;
	case err_leftmiddle:  //ȱ��[
		cout << "lack of \'[\' !" << endl;
		break;
	case err_rightmiddle:  //ȱ��]
		cout << "lack of \']\' !" << endl;
		break;
	case err_leftbraces:  //ȱ��{
		cout << "lack of \'{\' !" << endl;
		break;
	case err_rightbraces:   //ȱ��}
		cout << "lack of \'}\' !" << endl;
		break;
	case err_void:  //ȱ�� void
		cout << "lack of \'void\' !" << endl;
		break;
	case err_main:  //ȱ��main
		cout << "lack of \'main\' !" << endl;
		break;
	case err_printf:  //ȱ��printf
		cout << "lack of \'printf\' !" << endl;
		break;
	case err_scanf:  //ȱ��scanf
		cout << "lack of \'scanf\' !" << endl;
		break;
	case err_colon:  //ȱ��:
		cout << "lack of \':\' !" << endl;
		break;
	case err_case:  //ȱ��case
		cout << "lack of \'case\' !" << endl;
		break;
	case err_switch:  //ȱ��switch
		cout << "lack of \'switch\' !" << endl;
		break;
	case err_while:  //ȱ��while
		cout << "lack of \'while\' !" << endl;
		break;
	case err_if:  //ȱ�� if
		cout << "lack of \'if\' !" << endl;
		break;
	default:  //Ĭ�ϴ���
		cout << "default error!" << endl;
	}
	//goto Error;

}