#include	"bbi_type.h"
#include	"bbi_fio.h"
#include	"bbi_token.h"

/******************************************************************************
 * Local Type Definitions
 */

typedef struct _key_word_
{
	const char 		*name;
	TOKEN_KIND_T	keyKind;
} KEYWORD_T;

/******************************************************************************
 * Variables
 */
KEYWORD_T	keyTbl[] = {
	{"func"   , FUNC        }, {"var"    , VAR         },
	{"if"     , IF          }, {"elif"   , ELIF        },
	{"else"   , ELSE        }, {"for"    , FOR         },
	{"to"     , TO          }, {"step"   , STEP        },
	{"while"  , WHILE       }, {"end"    , END         },
	{"break"  , BREAK       }, {"return" , RETURN      },
	{"print"  , PRINT       }, {"println", PRINTLN     },
	{"option" , OPTION      }, {"input"  , INPUT       },
	{"toint"  , TOINT       }, {"exit"   , EXIT        },
	{"("      , LPAREN      }, {")"      , RPAREN      },
	{"["      , LBRACKET    }, {"]"      , RBRACKET    },
	{"+"      , PLUS        }, {"-"      , MINUS       },
	{"*"      , MULTI       }, {"/"      , DIVI        },
	{"=="     , EQUAL       }, {"!="     , NOTEQ       },
	{"<"      , LESS        }, {"<="     , LESSEQ      },
	{">"      , GREAT       }, {">="     , GREATEQ     },
	{"&&"     , AND         }, {"||"     , OR          },
	{"!"      , NOT         }, {"%"      , MOD         },
	{"?"      , IFSUB       }, {"="      , ASSIGN      },
	{"\\"     , INTDIVI     }, {","      , COMMA       },
	{"\""     , DBLQ        },
	{"@dummy" , KEYLIST_END },
};

static TOKEN_KIND_T	ctyp[256];		/* ASCII numbers*/
static char			*pCurrTok = NULL;
static BOOLEAN		endOfFile = FALSE;


void initChtype(void)
{
	int		index;

	for (index = 0; index < 256; index++)
	{
		if      (index >= '0' && index <= '9')
			ctyp[index] = DIGIT;
		else if (index >= 'a' && index <= 'z')
			ctyp[index] = LETTER;
		else if (index >= 'A' && index <= 'Z')
			ctyp[index] = LETTER;
		else
			ctyp[index] = OTHERS;
	}
	ctyp['_']  = LETTER;    ctyp['$']  = DOLL;
	ctyp['(']  = LPAREN;    ctyp[')']  = RPAREN;
	ctyp['[']  = LBRACKET;  ctyp[']']  = RBRACKET;
	ctyp['<']  = LESS;      ctyp['>']  = GREAT;
	ctyp['+']  = PLUS;      ctyp['-']  = MINUS;
	ctyp['*']  = MULTI;     ctyp['/']  = DIVI;
	ctyp['!']  = NOT;       ctyp['%']  = MOD;
	ctyp['?']  = IFSUB;     ctyp['=']  = ASSIGN;
	ctyp['\\'] = INTDIVI;   ctyp[',']  = COMMA;
	ctyp['\"'] = DBLQ;
}

static BOOLEAN is_operator_2(char c1, char c2)
{
	char	s[] = "    ";
	if (c1=='\0' || c2=='\0')
		return FALSE;

	s[1] = c1; s[2] = c2;
	if(strstr(" ++ -- <= >= == != && || ", s) != NULL)
		return TRUE;

	return FALSE;
}

static TOKEN_KIND_T get_kind(char *pStr)
{
	int		i;

	for (i = 0; keyTbl[i].keyKind != KEYLIST_END; i++)
	{
		if (strcmp(pStr, keyTbl[i].name) == 0)
			return keyTbl[i].keyKind;
	}

	if (ctyp[pStr[0]] == LETTER || ctyp[pStr[0] == DOLL])
		return IDENT;
	if (ctyp[pStr[0]] == DIGIT)
		return DBLNUM;

	return OTHERS;
}

static TOKEN_T *getToken(char *pSrc, char **ppDest)
{
	TOKEN_T			*pTkn = NULL;
	char			*tmp  = pSrc;
	int				idx   = 0;
	char			arry[50];
	TOKEN_KIND_T	tk;

	pTkn = (TOKEN_T*)malloc(sizeof(TOKEN_T));

	pTkn->pText   = NULL;
	pTkn->dblVal  = 0.0;
	pTkn->tokKind = OTHERS;


	if (endOfFile == TRUE)
	{
		pTkn->tokKind = E_OF_PROG;
		pTkn->dblVal  = 0.0;
		*ppDest       = NULL;

		return pTkn;
	}
	memset(&arry[0], 0x00, 50);
	while(1)
	{
		if (*tmp == '\0')
			break;
		if (tmp[0] == '/'&&tmp[1] == '/')
		{
			tmp[0] = '\0';
			break;
		}
		else
			tmp++;
	}
	tmp = pSrc;

	while (*tmp == TAB || *tmp == SPACE)
		tmp++;
	if(*tmp == '\0')
	{
		pTkn->tokKind = E_OF_LINE;
		pTkn->dblVal  = 0.0;
		*ppDest       = NULL;

		return pTkn;
	}

	switch(ctyp[*tmp])
	{
		case DOLL: case LETTER:
			arry[idx++] = *(tmp++);
			while(ctyp[*tmp] == LETTER || ctyp[*tmp] == DIGIT)
				arry[idx++] = *(tmp++);
			arry[idx] = '\0';
			break;
		case DIGIT:
			tk = INTNUM;
			while(ctyp[*tmp] == DIGIT)
				arry[idx++] = *(tmp++);
			if(ctyp[*tmp] == '.')
			{
				tk = DBLNUM;
				arry[idx++] = *(tmp++);
			}
			while(ctyp[*tmp] == DIGIT)
				arry[idx++] = *(tmp++);

			arry[idx] = '\0';
			pTkn->pText = (char*)malloc(strlen(&arry[0])+1);
			pTkn->tokKind = tk;
			strcpy(pTkn->pText, &arry[0]);
			pTkn->dblVal = atof(&arry[0]);

			*ppDest = tmp;
			return pTkn;
		case DBLQ:
			tmp++;
			while(*tmp != '\0' && *tmp != '"')
				arry[idx++] = *(tmp++);
			if (*tmp == '"')
				tmp++;
			else
			{
				printf("문자열 리터럴이 닫혀있지 않다.\n");
				exit(1);
			}
			arry[idx] = '\0';
			pTkn->pText = (char*)malloc(strlen(&arry[0])+1);
			pTkn->tokKind = STRING;
			strcpy(pTkn->pText, &arry[0]);

			*ppDest = tmp;
			return pTkn;
		default:
			if (is_operator_2(*tmp, *(tmp+1)) == TRUE)
			{
				arry[idx++] = *(tmp++);
				arry[idx++] = *(tmp++);
			}
			else
			{
				arry[idx++] = *(tmp++);
			}
			arry[idx] = '\0';
			break;
	}
	if((tk = get_kind(&arry[0])) == OTHERS)
	{
		printf("잘못된 토큰입니다: %s\n", &arry[0]);
		exit(1);
	}

	pTkn->tokKind = tk;
	pTkn->pText = (char*)malloc(strlen(&arry[0])+1);
	strcpy(pTkn->pText, &arry[0]);

	*ppDest = tmp;
	return pTkn;
}


TOKEN_T *getLine_token(void)
{
	FILE_INFO_T	fInfo   = getLine();
	TOKEN_T		*pTok   = NULL;
	char		*curr   = fInfo.pBuf;
	char		*next   = NULL;

	if (fInfo.line < 0) endOfFile = TRUE;
	else                endOfFile = FALSE;

	pTok = getToken(curr, &next);

	pCurrTok = next;
	return pTok;
}

TOKEN_T *get_token(void)
{
	TOKEN_T		*pTok   = NULL;
	char		*curr   = pCurrTok;
	char		*next   = NULL;

	pTok = getToken(curr, &next);

	pCurrTok = next;
	return pTok;
}

void release_token(TOKEN_T *pToken)
{
	if(pToken && pToken->pText)
		free(pToken->pText);

	free(pToken);
}

int checkToken(FILE_INFO_T fInfo)
{
	char			*pBuf    = fInfo.pBuf;
	TOKEN_T *pToken = NULL;
	char	*curr   = pBuf;
	char	*next   = NULL;

	do {
		pToken = getToken(curr, &next);

		if (pToken->tokKind == E_OF_LINE)
		{
			free(pToken);
			break;
		}

		printf("%-10s : [%03d] [%03.8f]\n", pToken->pText, pToken->tokKind, pToken->dblVal);

		free(pToken->pText);
		free(pToken);
		pToken = NULL;

		curr = next;
		next = NULL;
	} while(1);

	return 0;
}
