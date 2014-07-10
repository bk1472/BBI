/**********************************************************/
/*    filename:bbi_tbl.cpp �ɺ� ���̺�                    */
/**********************************************************/
#include "bbi.h"
#include "bbi_prot.h"

/* ---------------------------- ���̺� */
vector<SymTbl> Gtable;            /* ���� �ɺ� ���̺� */
vector<SymTbl> Ltable;            /* ���� �ɺ� ���̺� */
int startLtable;           		  /* ���ÿ� ���� ��ġ */

int enter(SymTbl& tb, SymKind kind) /* �ɺ� ���̺� ��� */
{
	int n, mem_size;
	bool isLocal = is_localName(tb.name, kind);
	extern int localAdrs;                               /* ���� ���� �ּ� ���� */
	extern Mymemory Dmem;                                            /* �޸� */

	// Ȯ��
	mem_size = tb.aryLen;
	if (mem_size == 0) mem_size = 1;                          /* �ܼ� ������ �� */
	if (kind!=varId && tb.name[0]=='$')                          /* $ ��� Ȯ�� */
		err_exit("������ �̿ܿ��� $�� ����� �� �����ϴ�: ", tb.name);
	tb.nmKind = kind;
	n = -1;                                                         /* �ߺ�Ȯ�� */
	if (kind == fncId)  n = searchName(tb.name, 'G');
	if (kind == paraId) n = searchName(tb.name, 'L');
	if (n != -1) err_exit("�̸��� �ߺ��Ǿ����ϴ�: ", tb.name);

	// �ּ� ����
	if (kind == fncId) tb.adrs = get_lineNo();                    /* �Լ� ���� �� */
	else {
		if (isLocal) { tb.adrs = localAdrs; localAdrs += mem_size; }      /* ���� */
		else {
			tb.adrs = Dmem.size();                                          /* �۷ι� */
			Dmem.resize(Dmem.size() + mem_size);                  /* �۷ι� ���� Ȯ�� */
		}
	}

	// ���
	if (isLocal) { n = Ltable.size(); Ltable.push_back(tb); }            /* ���� */
	else         { n = Gtable.size(); Gtable.push_back(tb); }          /* �۷ι� */
	return n;                                                       /* ��� ��ġ */
}

void set_startLtable() /* ���� �ɺ� ���̺� ���� ��ġ */
{
	startLtable = Ltable.size();
}

bool is_localName(const string& name, SymKind kind) /* ���� �̸��̸� ��*/
{
	if (kind == paraId) return true;
	if (kind == varId) {
		if (is_localScope() && name[0]!='$') return true; else return false;
	}
	return false;                                                      /* fncId */
}

int searchName(const string& s, int mode) /* �̸� �˻� */
{
	int n;
	switch (mode) {
	case 'G': 										 /* �۷ι� �ɺ� ���̺� �˻� */
		for (n=0; n<(int)Gtable.size(); n++) {
			if (Gtable[n].name == s) return n;
		}
		break;
	case 'L':  											/* ���� �ɺ� ���̺� �˻� */
		for (n=startLtable; n<(int)Ltable.size(); n++) {
			if (Ltable[n].name == s) return n;
		}
		break;
	case 'F':  													  /* �Լ��� �˻� */
		n = searchName(s, 'G');
		if (n != -1 && Gtable[n].nmKind==fncId) return n;
		break;
	case 'V':  													  /* ������ �˻� */
		if (searchName(s, 'F') != -1) err_exit("�Լ���� �ߺ��Ǿ����ϴ�: ", s);
		if (s[0] == '$')     return searchName(s, 'G');
		if (is_localScope()) return searchName(s, 'L');      /* ���� ���� ó���� */
		else                 return searchName(s, 'G');      /* �۷ι� ���� ó���� */
	}
	return -1; // �߰ߵ��� ����
}

vector<SymTbl>::iterator tableP(const CodeSet& cd) /* �ݺ��� ȹ�� */
{
	if (cd.kind == Lvar) return Ltable.begin() + cd.symNbr;             /* Lvar */
	return Gtable.begin() + cd.symNbr;                            /* Gvar Fcall */
}

