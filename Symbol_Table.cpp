#include "head.h"
#include "Symbol_Table.h"

vector<word> Lex_Table;

/*
1.push_back �������������һ������

2.pop_back ȥ����������һ������

3.at �õ����λ�õ�����

4.begin �õ�����ͷ��ָ��

5.end �õ���������һ����Ԫ+1��ָ��

6��front �õ�����ͷ������

7.back �õ���������һ����Ԫ������

8.max_size �õ�vector�������Ƕ��

9.capacity ��ǰvector����Ĵ�С

10.size ��ǰʹ�����ݵĴ�С

11.resize �ı䵱ǰʹ�����ݵĴ�С��������ȵ�ǰʹ�õĴ������Ĭ��ֵ

12.reserve �ı䵱ǰvecotr������ռ�Ĵ�С

13.erase ɾ��ָ��ָ���������

14.clear ��յ�ǰ��vector

15.rbegin ��vector��ת��Ŀ�ʼָ�뷵��(��ʵ����ԭ����end-1)

16.rend ��vector��ת���Ľ���ָ�뷵��(��ʵ����ԭ����begin-1)

17.empty �ж�vector�Ƿ�Ϊ��

18.swap ����һ��vector��������

*/