/**********************************************************/
/*    filename:bbi_misc.cpp ��Ÿ �Լ�                     */
/**********************************************************/
#include "bbi.h"
#include "bbi_prot.h"

string dbl_to_s(double d) /* ��ġ �� ���ڿ� */
{
	ostringstream ostr;                             /* ��¿� ��Ʈ�� Ȯ��  */
	ostr << d;                                      /* ��� ��Ʈ���� ����  */
	return ostr.str();                              /* ���� ������ ��ȯ�Ѵ� */
}

string err_msg(const string& a, const string& b) /* ���� ���� ���� */
{
	if (a == "") return b + " �� �ʿ��մϴ�.";
	if (b == "") return a + " �� �ٸ��� �ʽ��ϴ�.";
	return b + " �� " + a + " �տ� �ʿ��մϴ�.";
}

// �Լ� ���𿡼� ���� ����Ʈ �μ��� ���� 
//void err_exit(Tobj a="\1", Tobj b="\1", Tobj c="\1", Tobj d="\1")
void err_exit(Tobj a, Tobj b, Tobj c, Tobj d) /* ���� ǥ�� */
{
	Tobj ob[5];
	ob[1] = a; ob[2] = b; ob[3] = c; ob[4] = d;
	cerr << "line:" << get_lineNo() << " ERROR ";

	for (int i=1; i<=4 && ob[i].s!="\1"; i++) {
		if (ob[i].type == 'd') cout << ob[i].d;  // ��ġ ����
		if (ob[i].type == 's') cout << ob[i].s;  // ���ڿ� ����
	}
	cout << endl;
	exit(1);
}
