/**********************************************************/
/*    filename:bbi_pars.cpp �����м�                      */
/**********************************************************/
#include "bbi.h"
#include "bbi_prot.h"

#define NO_FIX_ADRS 0                                 /* ���� �ּ� ���� ǥ�� */
Token token;                                          /* ���� ó������ ��ū  */
SymTbl tmpTb;                                       /* �ӽ� ���� �ɺ� ���̺� */
int blkNest;                                                /* ����� ����   */
int localAdrs;                                        /* ���� ���� �ּ� ���� */
int mainTblNbr;                     /* main �Լ��� ������ �� �ɺ� ���̺� ��ġ*/
int loopNest;                                                 /* ���� �׽�Ʈ */
bool fncDecl_F;                                  /* �Լ� ���� ó�� ���̸� �� */
bool explicit_F;                                    /* ���̸� ���� ���� ���� */
char codebuf[LIN_SIZ+1], *codebuf_p;                /* ���� �ڵ� ���� �۾��� */
extern vector<char*> intercode;                /* ��ȯ�� ��ģ ���� �ڵ� ���� */

void init() /* �ʱ갪 ���� */
{
	initChTyp();                                                    /* ���� ����ǥ */
	mainTblNbr = -1;
	blkNest = loopNest = 0;
	fncDecl_F = explicit_F = false;
	codebuf_p = codebuf;
}

void convert_to_internalCode(char *fname) /* �ڵ� ��ȯ */
{
	init();                                               /* ���� ����ǥ �� �ʱ�ȭ  */

	// �Լ� ���� �̸��� ���� ��� 
	fileOpen(fname);
	while (token=nextLine_tkn(), token.kind != EofProg) {
		if (token.kind == Func) {
			token = nextTkn(); set_name(); enter(tmpTb, fncId);
		}
	}

	// ���� �ڵ�� ��ȯ 
	push_intercode();                                /* 0��°�� �ʿ� �����Ƿ� �ſ��  */
	fileOpen(fname);
	token = nextLine_tkn();
	while (token.kind != EofProg) {
		convert();                                            /* ���� �ڵ�� ��ȯ */
	}

	// main �Լ��� ������ ���� �Լ� ȣ�� �ڵ带 ���� 
	set_startPc(1);                                        /* 1����� ���� ����  */
	if (mainTblNbr != -1) {
		set_startPc(intercode.size());                        /* main�������� ���� ���� */
		setCode(Fcall, mainTblNbr); setCode('('); setCode(')');
		push_intercode();
	}
}

// �� �տ��� �����ϴ� �ڵ带 ó��. ������ �κ��� convert_rest()�� ó��
void convert()
{
	switch (token.kind) {
	case Option: optionSet(); break;                               /* �ɼ� ����  */
	case Var:    varDecl();   break;                                /* ���� ���� */
	case Func:   fncDecl();   break;                                /* �Լ� ���� */
	case While: case For:
		++loopNest;
		convert_block_set(); setCode_End();
		--loopNest;
		break;
	case If:
		convert_block_set();                                // if
		while (token.kind == Elif) { convert_block_set(); } // elif
		if (token.kind == Else)    { convert_block_set(); } // else
		setCode_End();                                      // end
		break;
	case Break:
		if (loopNest <= 0) err_exit("�߸��� break�Դϴ�.");
		setCode(token.kind); token = nextTkn(); convert_rest();
		break;
	case Return:
		if (!fncDecl_F) err_exit("�߸��� return�Դϴ�.");
		setCode(token.kind); token = nextTkn(); convert_rest();
		break;
	case Exit:
		setCode(token.kind); token = nextTkn(); convert_rest();
		break;
	case Print: case Println:
		setCode(token.kind); token = nextTkn(); convert_rest();
		break;
	case End:
		err_exit("�߸��� end�Դϴ�.");       /* end�� �ܵ����� ���Ǵ� ���� ���� */
		break;
	default: convert_rest(); break;
	}
}

void convert_block_set() /* ��� ó�� ���� */
{
	int patch_line;
	patch_line = setCode(token.kind, NO_FIX_ADRS); token = nextTkn();
	convert_rest();
	convert_block();                                            /* ��� ó�� */
	backPatch(patch_line, get_lineNo());        /* NO_FIX_ADRS�� ����(end�� ��ȣ) */
}

void convert_block() /* ��� ó�� */
{
	TknKind k;
	++blkNest;                                      /* ��� ������ ���� �м� */
	while(k=token.kind, k!=Elif && k!=Else && k!=End && k!=EofProg) {
		convert();
	}
	--blkNest;
}

void convert_rest() /* ���� ������ ó�� */
{
	int tblNbr;

	for (;;) {
		if (token.kind == EofLine) break;
		switch (token.kind) {      // �� �� Ű���尡 ���߿� ��Ÿ���� ���� ���� */
		case If: case Elif: case Else: case For: case While: case Break:
		case Func:  case Return:  case Exit:  case Print:  case Println:
		case Option: case Var: case End:
			err_exit("�߸��� ����Դϴ�: ", token.text);
			break;
		case Ident:                                              /* �Լ� ȣ��, ���� */
			set_name();
			if ((tblNbr=searchName(tmpTb.name, 'F')) != -1) {    /* �Լ� ��� ����*/
				if (tmpTb.name == "main") err_exit("main�Լ��� ȣ���� �� �����ϴ�.");
				setCode(Fcall, tblNbr); continue;
			}
			if ((tblNbr=searchName(tmpTb.name, 'V')) == -1) {    /* ���� ��� ����  */
				if (explicit_F) err_exit("���� ������ �ʿ��մϴ�: ", tmpTb.name);
				tblNbr = enter(tmpTb, varId);                      /* �ڵ� ���� ��� */
			}
			if (is_localName(tmpTb.name, varId)) setCode(Lvar, tblNbr);
			else                                 setCode(Gvar, tblNbr);
			continue;
		case IntNum: case DblNum:                         /* ������ double������ ���� */
			setCode(token.kind, set_LITERAL(token.dblVal));
			break;
		case String:
			setCode(token.kind, set_LITERAL(token.text));
			break;
		default:                                                   /* + - <= ��  */
			setCode(token.kind);
			break;
		}
		token = nextTkn();
	}
	push_intercode();
	token = nextLine_tkn();
}

void optionSet() /* �ɼ� ���� */
{
	setCode(Option);                 /* �� ���� ������̹Ƿ� �ڵ� ��ȯ�� Option�� */
	setCode_rest();                                       /* ������ ������� ���� */
	token = nextTkn();                                  /* �� ���� ������ �����Ѵ� */
	if (token.kind==String && token.text=="var") explicit_F = true;
	else err_exit("option������ �ٸ��� �ʽ��ϴ�.");
	token = nextTkn();
	setCode_EofLine();
}

void varDecl() /* var�� ����ϴ� ���� ���� */
{
	setCode(Var);                  /* �� ���� ������̹Ƿ� �ڵ� ��ȯ��  Var�� */
	setCode_rest();                                 /* �������� ������� ���� */
	for (;;) {
		token = nextTkn();
		var_namechk(token);                                         /* �̸� �˻� */
		set_name(); set_aryLen();                          /* �迭�̸� ���� ���� */
		enter(tmpTb, varId);                            /* �������(�ּҵ� ���) */
		if (token.kind != ',') break;                               /* ���� ���� */
	}
	setCode_EofLine();
}

void var_namechk(const Token& tk) /* �̸� Ȯ�� */
{
	if (tk.kind != Ident) err_exit(err_msg(tk.text, "�ĺ���"));
	if (is_localScope() && tk.text[0] == '$')
		err_exit("�Լ� �� var���𿡼��� $�� ���� �̸��� ������ �� �����ϴ�: ", tk.text);
	if (searchName(tk.text, 'V') != -1)
		err_exit("�ĺ��ڰ� �ߺ��Ǿ����ϴ�: ", tk.text);
}

void set_name() /* �̸� ���� */
{
	if (token.kind != Ident) err_exit("�ĺ��ڰ� �ʿ��մϴ�: ", token.text);
	tmpTb.clear(); tmpTb.name = token.text;                         /* �̸� ���� */
	token = nextTkn();
}

void set_aryLen() /* �迭 ũ�� ���� */
{
	tmpTb.aryLen = 0;
	if (token.kind != '[') return;                                /* �迭�� �ƴϴ� */

	token = nextTkn();
	if (token.kind != IntNum)
		err_exit("�迭 ���̴� ��(+)�� ������ ������ �ּ���: ", token.text);
	tmpTb.aryLen = (int)token.dblVal + 1;   /* var a[5]�� ÷��0~5�� ��ȿ�̹Ƿ� +1 */
	token = chk_nextTkn(nextTkn(), ']');
	if (token.kind == '[') err_exit("������ �迭�� ������ �� �����ϴ�.");
}

void fncDecl() /* �Լ����� */
{
	extern vector<SymTbl> Gtable;                                 /* �۷ι� �ɺ� ���̺� */
	int tblNbr, patch_line, fncTblNbr;

	if(blkNest > 0) err_exit("�Լ� ���� ��ġ�� �ٸ��� �ʽ��ϴ�.");
	fncDecl_F = true;                                  /* �Լ� ó�� ���� �÷��� */
	localAdrs = 0;                              /* ���� ���� �Ҵ� ī���� �ʱ�ȭ */
	set_startLtable();                            /* ���� �ɺ� ���̺� ���� ��ġ */
	patch_line = setCode(Func, NO_FIX_ADRS);       /* ���߿� end�� ��ȣ�� �ִ´�*/
	token = nextTkn();

	fncTblNbr = searchName(token.text, 'F');      /* �Լ����� �� ó���� ����Ѵ� */
	Gtable[fncTblNbr].dtTyp = DBL_T;                     /* �Լ� ���� double���� */

	// ���μ� �м�
	token = nextTkn();
	token = chk_nextTkn(token, '(');                               /* '(' �� �� */
	setCode('(');
	if (token.kind != ')') {                                       /* �μ� ���� */
		for (;; token=nextTkn()) {
			set_name();
			tblNbr = enter(tmpTb, paraId);                             /* �μ� ��� */
			setCode(Lvar, tblNbr);                           /*�μ��� Lvar�μ� ó�� */
			++Gtable[fncTblNbr].args;                               /* �μ�������+1 */
			if (token.kind != ',') break;                              /* ���� ���� */
			setCode(',');
		}
	}
	token = chk_nextTkn(token, ')');                                 /* ')'�� �� */
	setCode(')'); setCode_EofLine();
	convert_block();                                           /* �Լ� ��ü ó�� */

	backPatch(patch_line, get_lineNo());                     /* NO_FIX_ADRS ���� */
	setCode_End();
	Gtable[fncTblNbr].frame = localAdrs;                          /* ������ ũ�� */

	if (Gtable[fncTblNbr].name == "main") {                    /* main �Լ� ó�� */
		mainTblNbr = fncTblNbr;
		if (Gtable[mainTblNbr].args != 0)
			err_exit("main�Լ������� ���μ��� ������ �� �����ϴ�.");
	}
	fncDecl_F = false;                                          /* �Լ� ó�� ���� */
}

void backPatch(int line, int n) /* line�࿡ n�� ���� */
{
	*SHORT_P(intercode[line] + 1) = (short)n;
}

void setCode(int cd) /* �ڵ� ���� */
{
	*codebuf_p++ = (char)cd;
}

int setCode(int cd, int nbr) /* �ڵ�� short�� ���� */
{
	*codebuf_p++ = (char)cd;
	*SHORT_P(codebuf_p) = (short)nbr; codebuf_p += SHORT_SIZ;
	return get_lineNo();                           /* backPatch������ �������� ��ȯ�Ѵ� */
}

void setCode_rest() /* ������ �ؽ�Ʈ�� �״�� ���� */
{
	extern char *token_p;
	strcpy(codebuf_p, token_p);
	codebuf_p += strlen(token_p) + 1;
}

void setCode_End() /* end�� ����ó�� */
{
	if (token.kind != End) err_exit(err_msg(token.text, "end"));
	setCode(End); token = nextTkn(); setCode_EofLine();
}

void setCode_EofLine() /* ���� ���� ó�� */
{
	if (token.kind != EofLine) err_exit("�߸��� ����Դϴ�: ", token.text);
	push_intercode();
	token = nextLine_tkn();                                     /* ���� ������ �����Ѵ� */
}

void push_intercode() /* ��ȯ�� ���� �ڵ带 ���� */
{
	int len;
	char *p;

	*codebuf_p++ = '\0';
	if ((len = codebuf_p-codebuf) >= LIN_SIZ)
		err_exit("��ȯ ���� ���� �ڵ尡 �ʹ� ��ϴ�. ���� �ٿ��ּ���.");

	try {
		p = new char[len];                                          /* �޸� Ȯ�� */
		memcpy(p, codebuf, len);
		intercode.push_back(p);
	}
	catch (bad_alloc) { err_exit("�޸𸮸� Ȯ���� �� �����ϴ�"); }
	codebuf_p = codebuf;                /* ���� ó���� ����� ������ �� �� ������ ��ġ���� */
}

bool is_localScope() /* �Լ��� ó�� ���̸� ��*/
{
	return fncDecl_F;
}

