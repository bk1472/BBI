#include	"bbi_type.h"
#include	"bbi_fio.h"
#include	"bbi_parse.h"
#include	"bbi_token.h"

void parse_code (char *fname)
{
	BOOLEAN		break_cond = FALSE;
	TOKEN_T		*pToken    = NULL;

	initChtype();

	srcFileOpen(fname);
	do {
		pToken = getLine_token();
		break_cond = (pToken->tokKind == E_OF_PROG)?TRUE:FALSE;

		if (pToken->tokKind == FUNC)
		{
			/*TODO: Function�� ��츸 �켱 symbol Table�� ��� �Ѵ�.*/
			printf("[Func]%-10s : [%03d] [%03.8f]\n", pToken->pText, pToken->tokKind, pToken->dblVal);
			release_token(pToken);
			pToken = get_token();
			printf("      %-10s : [%03d] [%03.8f]\n", pToken->pText, pToken->tokKind, pToken->dblVal);
		}
		release_token(pToken);
	} while (break_cond == FALSE);
}
