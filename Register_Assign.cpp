#include "head.h"
#include "Optim.h"


vector<string> regtabel = { reg_t3,reg_t4,reg_t5,reg_t6,reg_t7,
							reg_s0,reg_s1,reg_s2,reg_s3,reg_s4,reg_s5,reg_s6,reg_s7,reg_t8,reg_t9,
							reg_k0,reg_k1 };

typedef struct
{
	string name;
	int cnt;
}VAR;

//�������ô�������
bool cmp(const VAR a, const VAR b)
{
	return a.cnt > b.cnt;
}

bool isvar(string s)//�ж��Ƿ��Ǳ���
{
	if (s == "")
	{
		return false;
	}
	if (s.length() > 5 && s[0] == '_' && (s[1] == 'v' || s[1] == 'V') && s[2] == 'a' && s[3] == 'r' && s[4] == '_')
	{
		return false;
	}
	if (s == "int" || s == "char" || s == "void")
	{
		return false;
	}
	if (s[0] >= '0' && s[0] <= '9')
	{
		return false;
	}
	if (s.length() > 7 && s[0] == '_' && (s[1] == 'l' || s[1] == 'L') && s[2] == 'a' && s[3] == 'b' && s[4] == 'e' && s[5] == 'l' && s[6] == '_')
	{
		return false;
	}
	return true;
}

// �Ĵ�������
map<string, string> var2reg[MAX_NUM];

//�Ĵ�������
void Register_Assign()
{
	int regnum = regtabel.size();
	string op, var1, var2, var3;
	map<string, int> varindex; //��������ֵ
	vector<VAR> vartabel;
	VAR tmp;
	int blkvarnum;
	for (int i = 1; i <= blocknum; i++)
	{
		varindex.clear();
		vartabel.clear();
		// ͳ��ÿ������ʹ�õĴ���
		blkvarnum = 0;
		for (int j = blockpos[i].first; j <= blockpos[i].second; j++)
		{
			op = IR_List[j].op;
			var1 = IR_List[j].src1;
			var2 = IR_List[j].src2;
			var3 = IR_List[j].res;

			if (IR_List[j].islableorfunc) continue;

			if (isvar(var1))
			{
				if (varindex.find(var1) != varindex.end())
				{
					vartabel[varindex[var1]].cnt++;
				}
				else
				{
					varindex[var1] = blkvarnum++;
					tmp.name = var1;
					tmp.cnt = 1;
					vartabel.push_back(tmp);
				}
			}
			if (isvar(var2))
			{
				if (varindex.find(var2) != varindex.end())
				{
					vartabel[varindex[var2]].cnt++;
				}
				else
				{
					varindex[var1] = blkvarnum++;
					tmp.name = var2;
					tmp.cnt = 1;
					vartabel.push_back(tmp);
				}
			}
			if (isvar(var3))
			{
				if (varindex.find(var3) != varindex.end())
				{
					vartabel[varindex[var3]].cnt++;
				}
				else
				{
					varindex[var1] = blkvarnum++;
					tmp.name = var3;
					tmp.cnt = 1;
					vartabel.push_back(tmp);
				}
			}
		}
		sort(vartabel.begin(), vartabel.end(), cmp);// ��ʹ�ô����������ɴ�С����
		var2reg[i].clear();
		for (int j = 0; j < blkvarnum; j++)// ����������Ĵ���
		{
			if (j < regnum)
			{
				//����Ĵ���
				var2reg[i][vartabel[j].name] = regtabel[j];
			}
			else
			{
				//δ����Ĵ���
				var2reg[i][vartabel[j].name] = vartabel[j].name;
			}
			blockvar[i].push_back(vartabel[j].name);
		}
	}
}

