/**************************************************************/
/*    filename:bbi_code.cpp �޸� ����, ���� �˻�, ����    */
/**************************************************************/
#include "bbi.h"
#include "bbi_prot.h"

CodeSet code;                                                    /* �ڵ� ��Ʈ */
int startPc;                                                   /* ���� ������ */
int Pc = -1;                                /* ���α׷� ī���� -1: ������ �ƴ�*/
int baseReg;                                               /* ���̽� �������� */
int spReg;                                                     /* ���� ������ */
int maxLine;                                                 /* ���α׷� ���� */
vector<char*> intercode;                        /* ��ȯ�� ���� ���� �ڵ� ���� */
char *code_ptr;                                    /* ���� �ڵ� �м��� ������ */
double returnValue;                                           /* �Լ� ��ȯ �� */
bool break_Flg, return_Flg, exit_Flg;                        /* ����� �÷��� */
Mymemory Dmem;                                                      /* �޸� */
vector<string> strLITERAL;                              /* ���ڿ� ���ͷ� ���� */
vector<double> nbrLITERAL;                                /* ��ġ ���ͷ� ���� */
bool syntaxChk_mode = false;                             /* ���� �˻��� �� �� */
extern vector<SymTbl> Gtable;                           /* �۷ι� �ɺ� ���̺� */

class Mystack {                                     /* stack<double> �� ���� */
private:
	stack<double> st;
public:
	void push(double n) { st.push(n); }                                 /* �ֱ� */
	int size() { return (int)st.size(); }                               /* ũ�� */
	bool empty() { return st.empty(); }                        /* ������� ���� */
	double pop() {                        /* �б� & ����(���� pop�� �ٸ��� ���� */
		if (st.empty()) err_exit("stack underflow");
		double d = st.top();                                     /* ���� ž�� �� */
		st.pop(); return d;                                          /* �ϳ� ���� */
	}
};
Mystack stk;                                            /* ���۷��� ���� */

void syntaxChk() /* ���� �˻� */
{
	syntaxChk_mode = true;
	for (Pc=1; Pc<(int)intercode.size(); Pc++) {
		code = firstCode(Pc);
		switch (code.kind) {
		case Func: case Option: case Var:                         /* �˻� �Ϸ� */
			break;
		case Else: case End: case Exit:
			code = nextCode(); chk_EofLine();
			break;
		case If: case Elif: case While:
			code = nextCode(); (void)get_expression(0, EofLine);            /* �İ� */
			break;
		case For:
			code = nextCode();
			(void)get_memAdrs(code);                              /* ���� ���� �ּ� */
			(void)get_expression('=', 0);                                 /* �ʱ갪 */
			(void)get_expression(To, 0);                                  /* ������ */
			if (code.kind == Step) (void)get_expression(Step,0);          /* ���а� */
			chk_EofLine();
			break;
		case Fcall:                                         /* ���� ���� �Լ� ȣ�� */
			fncCall_syntax(code.symNbr);
			chk_EofLine();
			(void)stk.pop();                                          /* ��ȯ �� ���ʿ� */
			break;
		case Print: case Println:
			sysFncExec_syntax(code.kind);
			break;
		case Gvar: case Lvar:                                           /* ���Թ� */
			(void)get_memAdrs(code);                                   /* �º� �ּ� */
			(void)get_expression('=', EofLine);                        /* �캯�� �� */
			break;
		case Return:
			code = nextCode();                                              /* ��ȯ �� */
			if (code.kind!='?' && code.kind!=EofLine) (void)get_expression();
			if (code.kind == '?') (void)get_expression('?', 0);
			chk_EofLine();
			break;
		case Break:
			code = nextCode();
			if (code.kind == '?') (void)get_expression('?', 0);
			chk_EofLine();
			break;
		case EofLine:
			break;
		default:
			err_exit("�߸��� ����Դϴ�: ", kind_to_s(code.kind));
		}
	}
	syntaxChk_mode = false;
}

void set_startPc(int n) /* ������ ���� */
{
	startPc = n;
}

void execute() /* ���� */
{
	baseReg = 0;                                         /* ���̽� �������� �ʱ갪 */
	spReg = Dmem.size();                                     /* ���� ������ �ʱ갪 */
	Dmem.resize(spReg+1000);                              /* �޸� ���� �ʱ� Ȯ�� */
	break_Flg = return_Flg = exit_Flg = false;

	Pc = startPc;
	maxLine = intercode.size() - 1;
	while (Pc<=maxLine && !exit_Flg) {
		statement();
	}
	Pc = -1;                                                      /* ����� ���� */
}

void statement() /* �� */
{
	CodeSet save;
	int top_line, end_line, varAdrs;
	double wkVal, endDt, stepDt;

	if (Pc>maxLine || exit_Flg) return;                     /* ���α׷� ���� */
	code = save = firstCode(Pc);
	top_line = Pc; end_line = code.jmpAdrs;          /* ���� ������ ���۰� �� */
	if (code.kind == If ) end_line = endline_of_If(Pc);     /* if���� ���� �� */

	switch (code.kind) {
	case If:
		// if
		if (get_expression(If, 0)) {                          /*   ��(TRUE)�̸�    */
			++Pc; block(); Pc = end_line + 1;                   /*   �����ϰ�     */
			return;                                             /*   ����        */
		}
		Pc = save.jmpAdrs;                                    /*   ��������         */
		// elif
		while (lookCode(Pc) == Elif) {
			save = firstCode(Pc); code = nextCode();
			if (get_expression()) {                             /*   ��(TRUE)�̸�     */
				++Pc; block(); Pc = end_line + 1;                 /*   �����ϰ�    */
				return;                                           /*   ����         */
			}
			Pc = save.jmpAdrs;                                  /*   ��������         */
		}
		// else
		if (lookCode(Pc) == Else) {                           /* else��       */
			++Pc; block(); Pc = end_line + 1;                   /*   �����ϰ�      */
			return;                                             /* ����         */
		}
		// end
		++Pc;
		break;
	case While:
		for (;;) {                                           /*                   */
			if (!get_expression(While, 0)) break;              /*   false ����   */
			++Pc; block();                                     /*        [����]      */
			if (break_Flg || return_Flg || exit_Flg) {         /*                  */
				break_Flg = false; break;                        /*        �ߴ�        */
			}                                                  /*                   */
			Pc = top_line; code = firstCode(Pc);               /*   �� ������        */
		}                                                    /*                   */
		Pc = end_line + 1;                                   /*                   */
		break;
	case For:										/* for �����, �ʱ갪, ������, ���н� */
		save = nextCode();
		varAdrs = get_memAdrs(save);                    /* ���̺��� �ּ� ���ϱ�  */

		expression('=', 0);                                             /* �ʱ갪   */
		set_dtTyp(save, DBL_T);                                         /* �� Ȯ��  */
		Dmem.set(varAdrs, stk.pop());                             /*   �ʱ갪�� ����  */

		endDt = get_expression(To, 0);                            /* �������� ����   */
		/* ���а��� ����   */
		if (code.kind == Step) stepDt = get_expression(Step, 0); else stepDt = 1.0;
		for (;; Pc=top_line) {                               /*                   */
			if (stepDt >= 0) {                                 /*   ���� ����         */
				if (Dmem.get(varAdrs) > endDt) break;            /* �����̸� ����        */
			} else {                                           /*   ���� ����         */
				if (Dmem.get(varAdrs) < endDt) break;            /* �����̸� ����        */
			}                                                  /*                   */
			++Pc; block();                                     /*   [ ���� ]         */
			if (break_Flg || return_Flg || exit_Flg) {         /*                   */
				break_Flg = false; break;                        /*    �ߴ�            */
			}                                                  /*                   */
			Dmem.add(varAdrs, stepDt);                         /* �� ����             */
		}                                                    /*                   */
		Pc = end_line + 1;                                   /*                    */
		break;
	case Fcall:                                           /* ������ ���� �Լ� ȣ�� */
		fncCall(code.symNbr);
		(void)stk.pop();                                            /* ��ȯ �� ���ʿ� */
		++Pc;
		break;
	case Func:                                            /* �Լ� ���Ǵ� �ǳʲ� */
		Pc = end_line + 1;
		break;
	case Print: case Println:
		sysFncExec(code.kind);
		++Pc;
		break;
	case Gvar: case Lvar:                                             /* ���Թ� */
		varAdrs = get_memAdrs(code);
		expression('=', 0);
		set_dtTyp(save, DBL_T);                                 /* ������ �� �� Ȯ��*/
		Dmem.set(varAdrs, stk.pop());
		++Pc;
		break;
	case Return:
		wkVal = returnValue;
		code = nextCode();
		if (code.kind!='?' && code.kind!=EofLine)   /* '��'�� ������ ��ȯ ���� ��� */
			wkVal = get_expression();
		post_if_set(return_Flg);                                /* ?�� ������ ó�� */
		if (return_Flg) returnValue = wkVal;
		if (!return_Flg) ++Pc;
		break;
	case Break:
		code = nextCode(); post_if_set(break_Flg);              /* ? �� ������ ó�� */
		if (!break_Flg) ++Pc;
		break;
	case Exit:
		code = nextCode(); exit_Flg = true;
		break;
	case Option: case Var: case EofLine:                        /* ���� �ô� ���� */
		++Pc;
		break;
	default:
		err_exit("�߸��� ����Դϴ�: ", kind_to_s(code.kind));
	}
}

void block()  // ��� ������ ���� ����
{
	TknKind k;
	while (!break_Flg && !return_Flg && !exit_Flg) {  // break, return, exit���� ���ᡡ
		k = lookCode(Pc);                               // ���� ���� �ڵ�
		if (k==Elif || k==Else || k==End) break;        // ��� ���� ����
		statement();
	}
}

// �Լ� ���𿡼� ������ �⺻ �μ��� ����
// double get_expression(int kind1=0, int kind2=0)
double get_expression(int kind1, int kind2) /* ����� ��ȯ�ϴ� expression */
{
	expression(kind1, kind2); return stk.pop();
}

void expression(int kind1, int kind2) /* ���Ǻ� ��ó�� */
{
	if (kind1 != 0) code = chk_nextCode(code, kind1);
	expression();
	if (kind2 != 0) code = chk_nextCode(code, kind2);
}

void expression() /* �Ϲ� �� ó�� */
{
	term(1);
}

void term(int n) /* n�� �켱 ���� */
{
	TknKind op;
	if (n == 7) { factor(); return; }
	term(n+1);
	while (n == opOrder(code.kind)) {                /* �켱 ������ ���� �����ڰ� ���ӵȴ� */
		op = code.kind;
		code = nextCode(); term(n+1);
		if (syntaxChk_mode) { stk.pop(); stk.pop(); stk.push(1.0); }   /* ���� chk �� */
		else binaryExpr(op);
	}
}

void factor() /* ���� */
{
	TknKind kd = code.kind;

	if (syntaxChk_mode) {                                          /* ���� chk �� */
		switch (kd) {
		case Not: case Minus: case Plus:
			code = nextCode(); factor(); stk.pop(); stk.push(1.0);
			break;
		case Lparen:
			expression('(', ')');
			break;
		case IntNum: case DblNum:
			stk.push(1.0); code = nextCode();
			break;
		case Gvar: case Lvar:
			(void)get_memAdrs(code); stk.push(1.0);
			break;
		case Toint: case Input:
			sysFncExec_syntax(kd);
			break;
		case Fcall:
			fncCall_syntax(code.symNbr);
			break;
		case EofLine:
			err_exit("���� �ٸ��� �ʽ��ϴ�.");
		default:
			err_exit("�� ����:", kind_to_s(code));            /* a + = ��� �߻� */
		}
		return;
	}

	switch (kd) {                                                     /* ����� */
	case Not: case Minus: case Plus:
		code = nextCode(); factor();                         /* ���� ���� ȹ�� */
		if (kd == Not) stk.push(!stk.pop());                      /* !ó���Ѵ� */
		if (kd == Minus) stk.push(-stk.pop());                    /* -ó���Ѵ� */
		break;                                /* ���� +�� �ƹ��͵� ���� �ʴ´� */
	case Lparen:
		expression('(', ')');
		break;
	case IntNum: case DblNum:
		stk.push(code.dblVal); code = nextCode();
		break;
	case Gvar: case Lvar:
		chk_dtTyp(code);                                 /* �� ������ ��ģ �����ΰ� */
		stk.push(Dmem.get(get_memAdrs(code)));
		break;
	case Toint: case Input:
		sysFncExec(kd);
		break;
	case Fcall:
		fncCall(code.symNbr);
		break;
	}
}

int opOrder(TknKind kd) /* ���� ������ �켱 ����? */
{
	switch (kd) {
	case Multi: case Divi: case Mod:
	case IntDivi:                    return 6; /* *  /  % \  */
	case Plus:  case Minus:          return 5; /* +  -       */
	case Less:  case LessEq:
	case Great: case GreatEq:        return 4; /* <  <= > >= */
	case Equal: case NotEq:          return 3; /* == !=      */
	case And:                        return 2; /* &&         */
	case Or:                         return 1; /* ||         */
	default:                         return 0; /* �ش� ����    */
	}
}

void binaryExpr(TknKind op) /* ���� ���� */
{
	double d = 0, d2 = stk.pop(), d1 = stk.pop();

	if ((op==Divi || op==Mod || op==IntDivi) && d2==0)
		err_exit("0���� ���������ϴ�.");

	switch (op) {
	case Plus:    d = d1 + d2;  break;
	case Minus:   d = d1 - d2;  break;
	case Multi:   d = d1 * d2;  break;
	case Divi:    d = d1 / d2;  break;
	case Mod:     d = (int)d1 % (int)d2; break;
	case IntDivi: d = (int)d1 / (int)d2; break;
	case Less:    d = d1 <  d2; break;
	case LessEq:  d = d1 <= d2; break;
	case Great:   d = d1 >  d2; break;
	case GreatEq: d = d1 >= d2; break;
	case Equal:   d = d1 == d2; break;
	case NotEq:   d = d1 != d2; break;
	case And:     d = d1 && d2; break;
	case Or:      d = d1 || d2; break;
	}
	stk.push(d);
}

void post_if_set(bool& flg) /* ? �� */
{
	if (code.kind == EofLine) { flg = true; return; }       /* ?�� ������ flg�� ������  */
	if (get_expression('?', 0)) flg = true;                     /* ���ǽ����� ó�� */
}

void fncCall_syntax(int fncNbr) /* �Լ� ȣ�� �˻� */
{
	int argCt = 0;

	code = nextCode(); code = chk_nextCode(code, '(');
	if (code.kind != ')') {                                       /* �μ��� �ִ� */
		for (;; code=nextCode()) {
			(void)get_expression(); ++argCt;                /* �μ��� ó���� �μ� ���� */
			if (code.kind != ',') break;                     /* , �̸� �μ��� ��ӵȴ� */
		}
	}
	code = chk_nextCode(code, ')');                                /* ) �� �� */
	if (argCt != Gtable[fncNbr].args)                       /* �μ� ���� �˻� */
		err_exit(Gtable[fncNbr].name, " �Լ��� �μ� ������ �߸��Ǿ����ϴ�.");
	stk.push(1.0);                                          /* ������ ��ȯ �� */
}

//void fncCall(int fncNbr) /* a��eia?eo */
void fncCall(int fncNbr) /* �Լ�ȣ�� Ȯ�� */
{
	int  n, argCt = 0;
	vector<double> vc;

	// ���μ� ����
	nextCode(); code = nextCode();                         /* �Լ��� ( �ǳʶ� */
	if (code.kind != ')') {                                    /* �μ��� �ִ� */
		for (;; code=nextCode()) {
			expression(); ++argCt;                     /* �μ��� ó���� �μ� ���� */
			if (code.kind != ',') break;                 /* ,��� �μ��� ��ӵȴ� */
		}
	}
	code = nextCode();                                            /* ) �ǳʶ� */

	// �μ� ���� ���� ����
	for (n=0; n<argCt; n++) vc.push_back(stk.pop());  /* �ڿ������� �μ� �������� ����*/
	for (n=0; n<argCt; n++) { stk.push(vc[n]); }

	fncExec(fncNbr);                                                /* �Լ� ���� */
}

void fncExec(int fncNbr) /* �Լ� ���� */
{
	// �Լ��Ա�ó��1
	int save_Pc = Pc;                                     /* ���� �������� ���� */
	int save_baseReg = baseReg;                          /* ���� baseReg�� ���� */
	int save_spReg = spReg;                                /* ���� spReg�� ���� */
	char *save_code_ptr = code_ptr;         /* ���� ������ �м��� �����͸� ���� */
	CodeSet save_code = code;                               /* ���� code�� ���� */

	// �Լ��Ա�ó��2
	Pc = Gtable[fncNbr].adrs;                                 /* ���ο� Pc ���� */
	baseReg = spReg;                             /* ���ο� ���̽� �������� ���� */
	spReg += Gtable[fncNbr].frame;                               /* ������ Ȯ�� */
	Dmem.auto_resize(spReg);                           /* �޸� ��ȿ ���� Ȯ�� */
	returnValue = 1.0;                                     /* ��ȯ ���� �⺻��  */
	code = firstCode(Pc);                                     /* ���� �ڵ� ȹ�� */

	// �μ� ���� ó��
	nextCode(); code = nextCode();                           /* Func ( �ǳʶ�   */
	if (code.kind != ')') {                                        /* �μ� ���� */
		for (;; code=nextCode()) {
			set_dtTyp(code, DBL_T);                               /* ���� �� �� Ȯ�� */
			Dmem.set(get_memAdrs(code), stk.pop());                /* ���μ� �� ���� */
			if (code.kind != ',') break;                                /* �μ� ���� */
		}
	}
	code = nextCode();                                            /* ) �ǳʶ� */

	// �Լ� ��ü ó��
	++Pc; block(); return_Flg = false;                          /* �Լ� ��ü ó�� */

	// �Լ� �ⱸ ó��
	stk.push(returnValue);                                        /* ��ȯ �� ���� */
	Pc       = save_Pc;                                    /* ȣ�� �� ȯ���� ���� */
	baseReg  = save_baseReg;
	spReg    = save_spReg;
	code_ptr = save_code_ptr;
	code     = save_code;
}

void sysFncExec_syntax(TknKind kd) /* ���� �Լ� �˻� */
{
	switch (kd) {
	case Toint:
		code = nextCode(); (void)get_expression('(', ')');
		stk.push(1.0);
		break;
	case Input:
		code = nextCode();
		code = chk_nextCode(code, '('); code = chk_nextCode(code, ')');
		stk.push(1.0); /* ������ �� */
		break;
	case Print: case Println:
		do{
			code = nextCode();
			if(code.kind == String) code = nextCode();
			else (void)get_expression();
		} while(code.kind == ',');
		chk_EofLine();
		break;
	}
}


void sysFncExec(TknKind kd) /* �����Լ����� */
{
	double d;
	string s;

	switch (kd) {
	case Toint:
		code = nextCode();
		stk.push((int)get_expression('(', ')'));               /* ���� ���� */
		break;
	case Input:
		nextCode(); nextCode(); code = nextCode();       /* input ( ) �ǳʶ� */
		getline(cin, s);                                        /* 1�� �б�  */
		stk.push(atof(s.c_str()));                   /* ���ڷ� ��ȯ�ؼ� ���� */
		break;
	case Print: case Println:
		do {
			code = nextCode();
			if (code.kind == String) {                             /* ���ڿ� ��� */
				cout << code.text; code = nextCode();
			} else {
				d = get_expression();             /* �Լ� ������ exit ���ɼ��� �ִ� */
				if (!exit_Flg) cout << d;                              /* ��ġ ��� */
			}
		} while (code.kind == ',');               /* , ��� �Ķ���Ͱ� ��ӵȴ� */
		if (kd == Println) cout << endl;                  /* println�̸� �ٹٲ� */
		break;
	}
}

// �ܼ� ���� �Ǵ� �迭 ����� �ּҸ� ��ȯ�Ѵ�
int get_memAdrs(const CodeSet& cd)
{
	int adr=0, index, len;
	double d;

	adr = get_topAdrs(cd);
	len = tableP(cd)->aryLen;
	code = nextCode();
	if (len == 0) return adr;                                     /* ��迭 ���� */

	d = get_expression('[', ']');
	if ((int)d != d) err_exit("÷�ڴ� ������ ���� ��ġ�� ������ �ּ���.");
	if (syntaxChk_mode) return adr;                           /* ���� �˻� �� */

	/* Edit by Sigmadream */
	index = (int) d;
	if (index <0 || len <= index)
		err_exit(index,"÷�ڴ� ������ ������ϴ�(���ι��� : 0-",len-1,")" );
	return adr + index;		/* ÷�� ���� */
}

// ������ ���� �ּ�(�迭�� ���� �� ���� �ּ�)�� ��ȯ�Ѵ�
int get_topAdrs(const CodeSet& cd)
{
	switch (cd.kind) {
	case Gvar: return tableP(cd)->adrs;			        // �۷ι� ����
	case Lvar: return tableP(cd)->adrs + baseReg;         // ���� ����
	default: err_exit("�������� �ʿ��մϴ�: ", kind_to_s(cd));
	}
	return 0; // �̰����� ���� �ʴ´�
}

int endline_of_If(int line) /* if ���� �����ϴ� end ��ġ */
{
	CodeSet cd;
	char *save = code_ptr;

	cd = firstCode(line);
	for (;;) {
		line = cd.jmpAdrs;
		cd = firstCode(line);
		if (cd.kind==Elif || cd.kind==Else) continue;
		if (cd.kind == End) break;
	}
	code_ptr = save;
	return line;
}

void chk_EofLine() /* �ڵ� Ȯ�� */
{
	if (code.kind != EofLine) err_exit("�߸��� ����Դϴ�: ", kind_to_s(code));
}

TknKind lookCode(int line) /* line���� ���� �ڵ� */
{
	return (TknKind)(unsigned char)intercode[line][0];
}

CodeSet chk_nextCode(const CodeSet& cd, int kind2) /* Ȯ�κ� �ڵ� ȹ�� */
{
	if (cd.kind != kind2) {
		if (kind2   == EofLine) err_exit("�߸��� ����Դϴ�: ", kind_to_s(cd));
		if (cd.kind == EofLine) err_exit(kind_to_s(kind2), " �� �ʿ��մϴ�.");
		err_exit(kind_to_s(kind2) + " ��(��) " + kind_to_s(cd) + " �տ� �ʿ��մϴ�.");
	}
	return nextCode();
}

CodeSet firstCode(int line)   			/* ���� �ڵ� ȹ�� */
{
	code_ptr = intercode[line];          /* �м��� �����͸� ���� �������� ���� */
	return nextCode();
}

CodeSet nextCode() 						/* �ڵ� ȹ�� */
{
	TknKind kd;
	short int jmpAdrs, tblNbr;

	if (*code_ptr == '\0') return CodeSet(EofLine);
	kd = (TknKind)*UCHAR_P(code_ptr++);
	switch (kd) {
	case Func:
	case While: case For: case If: case Elif: case Else:
		jmpAdrs = *SHORT_P(code_ptr); code_ptr += SHORT_SIZ;
		return CodeSet(kd, -1, jmpAdrs);          // ������ �ּ�
	case String:
		tblNbr = *SHORT_P(code_ptr); code_ptr += SHORT_SIZ;
		return CodeSet(kd, strLITERAL[tblNbr].c_str());
	case IntNum: case DblNum:						// ���ڿ� ���ͷ� ��ġ
		tblNbr = *SHORT_P(code_ptr); code_ptr += SHORT_SIZ;
		return CodeSet(kd, nbrLITERAL[tblNbr]);  // ��ġ ���ͷ�
	case Fcall: case Gvar: case Lvar:
		tblNbr = *SHORT_P(code_ptr); code_ptr += SHORT_SIZ;
		return CodeSet(kd, tblNbr, -1);
	default:              						// �μ� ������ ���� �ڵ�
		return CodeSet(kd);
	}
}

void chk_dtTyp(const CodeSet& cd) /* �� Ȯ�� */
{
	if (tableP(cd)->dtTyp == NON_T)
		err_exit("�ʱ�ȭ���� ���� ������ ���Ǿ����ϴ�: ", kind_to_s(cd));
}

void set_dtTyp(const CodeSet& cd, char typ) /* �� ���� */
{
	int memAdrs = get_topAdrs(cd);
	vector<SymTbl>::iterator p = tableP(cd);

	if (p->dtTyp != NON_T) return;                  /* �̹� ���� �����Ǿ� �ִ� */
	p->dtTyp = typ;
	if (p->aryLen != 0) {                           /* �迭�̸� ������ 0���� �ʱ�ȭ */
		for (int n=0; n < p->aryLen; n++) { Dmem.set(memAdrs+n, 0); }
	}
}

int set_LITERAL(double d) /* ��ġ ���ͷ� */
{
	for (int n=0; n<(int)nbrLITERAL.size(); n++) {
		if (nbrLITERAL[n] == d) return n;            /* ���� ÷�� ��ġ�� ��ȯ�Ѵ� */
	}
	nbrLITERAL.push_back(d);                                /* ��ġ ���ͷ� ���� */
	return nbrLITERAL.size() - 1;                /*���� ��ġ ���ͷ��� ÷�� ��ġ */
}

int set_LITERAL(const string& s) /* ���ڿ� ���ͷ� */
{
	for (int n=0; n<(int)strLITERAL.size(); n++) {
		if (strLITERAL[n] == s) return n;            /* ���� ÷�� ��ġ�� ��ȯ�Ѵ� */
	}
	strLITERAL.push_back(s);                              /* ���ڿ� ���ͷ� ���� */
	return strLITERAL.size() - 1;             /* ���� ���ڿ� ���ͷ��� ÷�� ��ġ */
}


