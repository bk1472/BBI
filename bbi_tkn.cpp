/**********************************************************/
/*    filename:bbi_tkn.cpp ��ū ó��                      */
/**********************************************************/
#include "bbi.h"
#include "bbi_prot.h"

struct KeyWord {                               /* ���ֿ� ������ ������ ���� */
	const char *keyName;                                       /* if�� for �� */
	TknKind keyKind;                 /* �����ϴ� ��. TknKind�� bbi.h���� ���� */
};

KeyWord KeyWdTbl[] = {                          /* �����, �ɺ�, ���� ���� ���̺� */
	{"func"   , Func  }, {"var"    , Var    },
	{"if"     , If    }, {"elif"   , Elif   },
	{"else"   , Else  }, {"for"    , For    },
	{"to"     , To    }, {"step"   , Step   },
	{"while"  , While }, {"end"    , End    },
	{"break"  , Break }, {"return" , Return },
	{"print"  , Print }, {"println", Println},
	{"option" , Option}, {"input"  , Input  },
	{"toint"  , Toint }, {"exit"   , Exit   },
	{"("  , Lparen    }, {")"  , Rparen   },
	{"["  , Lbracket  }, {"]"  , Rbracket },
	{"+"  , Plus      }, {"-"  , Minus    },
	{"*"  , Multi     }, {"/"  , Divi     },
	{"==" , Equal     }, {"!=" , NotEq    },
	{"<"  , Less      }, {"<=" , LessEq   },
	{">"  , Great     }, {">=" , GreatEq  },
	{"&&" , And       }, {"||" , Or       },
	{"!"  , Not       }, {"%"  , Mod      },
	{"?"  , Ifsub     }, {"="  , Assign   },
	{"\\" , IntDivi   }, {","  , Comma    },
	{"\"" , DblQ      },
	{"@dummy", END_KeyList},
};

int srcLineno;                                          /* �ҽ��� �� ��ȣ */
TknKind ctyp[256];                                    /* ���� ����ǥ �迭 */
char *token_p;                                   /* 1���� ȹ��� ������ġ */
bool endOfFile_F;                                     /* ���� ���� �÷��� */
char buf[LIN_SIZ+5];                                  /* �ҽ� �о���� �� */
ifstream fin;                                              /* �Է� ��Ʈ�� */
#define MAX_LINE 2000                               /* �ִ� ���α׷� ��� */

void initChTyp() /* ���� ����ǥ ���� */
{                  /* ��: ��� ��Ҹ� ������� ������ Ȯ���� ����� �ְ� �ִ� */
	int i;
	for (i=0; i<256; i++)    { ctyp[i] = Others; }
	for (i='0'; i<='9'; i++) { ctyp[i] = Digit;  }
	for (i='A'; i<='Z'; i++) { ctyp[i] = Letter; }
	for (i='a'; i<='z'; i++) { ctyp[i] = Letter; }
	ctyp['_']  = Letter;    ctyp['$']  = Doll;
	ctyp['(']  = Lparen;    ctyp[')']  = Rparen;
	ctyp['[']  = Lbracket;  ctyp[']']  = Rbracket;
	ctyp['<']  = Less;      ctyp['>']  = Great;
	ctyp['+']  = Plus;      ctyp['-']  = Minus;
	ctyp['*']  = Multi;     ctyp['/']  = Divi;
	ctyp['!']  = Not;       ctyp['%']  = Mod;
	ctyp['?']  = Ifsub;     ctyp['=']  = Assign;
	ctyp['\\'] = IntDivi;   ctyp[',']  = Comma;
	ctyp['\"'] = DblQ;
}

void fileOpen(char *fname) /* ���� ���� */
{
	srcLineno = 0;
	endOfFile_F = false;
	fin.open(fname);
	if (!fin) { cout << fname << " ��(��) �� �� �����ϴ�\n"; exit(1); }
}

void nextLine() /* ���� �� ���� �����´� */
{
	string s;

	if (endOfFile_F) return;
	fin.getline(buf, LIN_SIZ+5);                                 /* 1�� �б� */
	if (fin.eof()) {                                            /* ���� ���� */
		fin.clear(); fin.close();                /* clear�� �� ���¿� ����Ѵ� */
		endOfFile_F = true; return;
	}

	if (strlen(buf) > LIN_SIZ)
		err_exit("���α׷��� 1�� ", LIN_SIZ, " �����̳��� ����� �ּ���");
	if (++srcLineno > MAX_LINE)
		err_exit("���α׷��� ", MAX_LINE, " ��(��) �Ѿ����ϴ�.");
	token_p = buf;                 /* ��ū �м��� �����͸� buf �� �տ� ��ġ��Ų�� */
}

Token nextLine_tkn() /* ���� ���� �а� ���� ��ū�� ��ȯ�Ѵ� */
{
	nextLine();
	return nextTkn();
}

#define CH (*token_p)
#define C2 (*(token_p+1))
#define NEXT_CH()  ++token_p
Token nextTkn() /* ���� ��ū */
{
	TknKind kd;
	string txt = "";

	if (endOfFile_F) return Token(EofProg);                     /* ���� ���� */
	while (isspace(CH)) NEXT_CH();                              /* ���� ��ŵ */
	if (CH == '\0')  return Token(EofLine);                          /* �ೡ */

	switch (ctyp[CH]) {
	case Doll: case Letter:
		txt += CH; NEXT_CH();
		while (ctyp[CH]==Letter || ctyp[CH]==Digit) { txt += CH; NEXT_CH(); }
		break;
	case Digit:                                                     /* ��ġ��� */
		kd = IntNum;
		while (ctyp[CH] == Digit)   { txt += CH; NEXT_CH(); }
		if (CH == '.') { kd = DblNum; txt += CH; NEXT_CH(); }
		while (ctyp[CH] == Digit)   { txt += CH; NEXT_CH(); }
		return Token(kd, txt, atof(txt.c_str()));       /* IntNum�� double������ ���� */
	case DblQ:                                                    /* ���ڿ� ��� */
		NEXT_CH();
		while (CH!='\0' && CH!='"') { txt += CH; NEXT_CH(); }
		if (CH == '"') NEXT_CH(); else err_exit("���ڿ� ���ͷ��� �������� �ʴ�. ");
		return Token(String, txt);
	default:
		if (CH=='/' && C2=='/') return Token(EofLine);                /* �ּ� */
		if (is_ope2(CH, C2)) { txt += CH; txt += C2; NEXT_CH(); NEXT_CH(); }
		else                 { txt += CH; NEXT_CH(); }
	}
	kd = get_kind(txt);                                             /* ������ ���� */

	if (kd == Others) err_exit("�߸��� ��ū�Դϴ�: ", txt);
	return Token(kd, txt);
}

bool is_ope2(char c1, char c2) /* 2���� �����ڸ� �� */
{
	char s[] = "    ";
	if (c1=='\0' || c2=='\0') return false;
	s[1] = c1; s[2] = c2;
	return strstr(" ++ -- <= >= == != && || ", s) != NULL;
}

TknKind get_kind(const string& s) /* ��ū ������ ���� */
{
	for (int i=0; KeyWdTbl[i].keyKind != END_KeyList; i++) {
		if (s == KeyWdTbl[i].keyName) return KeyWdTbl[i].keyKind;
	}
	if (ctyp[s[0]]==Letter || ctyp[s[0]]==Doll) return Ident;
	if (ctyp[s[0]] == Digit)  return DblNum;
	return Others;   // ����
}

Token chk_nextTkn(const Token& tk, int kind2) /* Ȯ�κ� ��ū ȹ�� */
{
	if (tk.kind != kind2) err_exit(err_msg(tk.text, kind_to_s(kind2)));
	return nextTkn();
}

void set_token_p(char *p) /* ��ū ó�� ������ ���� */
{
	token_p = p;
}

string kind_to_s(int kd) /* ������ �� ���ڿ� */
{
	for (int i=0; ; i++) {
		if (KeyWdTbl[i].keyKind == END_KeyList) break;
		if (KeyWdTbl[i].keyKind == kd) return KeyWdTbl[i].keyName;
	}
	return "";
}

string kind_to_s(const CodeSet& cd) /*  ������ �� ���ڿ� */
{
	switch (cd.kind) {
	case Lvar: case Gvar: case Fcall: return tableP(cd)->name;
	case IntNum: case DblNum: return dbl_to_s(cd.dblVal);
	case String: return string("\"") + cd.text + "\"";
	case EofLine: return "";
	}
	return kind_to_s(cd.kind);
}

int get_lineNo() /* �б� or ���� ��  */
{
	extern int Pc;
	return (Pc == -1) ? srcLineno : Pc;          /* �м� �� : ���� �� */
}

