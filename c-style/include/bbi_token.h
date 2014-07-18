#ifndef __BBI_TOKEN_H__
#define __BBI_TOKEN_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CR				(0x0d)
#define LF				(0x0a)
#define TAB				(0x09)
#define SPACE			(0x20)

typedef enum {
	LPAREN   = '(', RPAREN   = ')', LBRACKET = '[', RBRACKET = ']',
	PLUS     = '+', MINUS    = '-', MULTI    = '*', DIVI     = '/',
	MOD      = '%', INTDIVI  = '\\',
	NOT      = '!', IFSUB    = '?', ASSIGN   = '=', COMMA    = ',',
	DBLQ     = '"',

	FUNC     = 150,
	VAR           , IF            , ELIF          , ELSE          ,
	FOR           , TO            , STEP          , WHILE         ,
	END           , BREAK         , RETURN        , OPTION        ,
	PRINT         , PRINTLN       , INPUT         , TOINT         ,
	EXIT          , EQUAL         , NOTEQ         , LESS          ,
	LESSEQ        , GREAT         , GREATEQ       , AND           ,
	OR            , KEYLIST_END   ,

	IDENT         , INTNUM        , DBLNUM        , STRING        ,
	LETTER        , DOLL          , DIGIT         , GVAR          ,
	LVAR          , FCALL         , E_OF_PROG     , E_OF_LINE     ,
	OTHERS
} TOKEN_KIND_T;

typedef struct _token_
{
	TOKEN_KIND_T	tokKind;
	char			*pText;
	double			dblVal;
} TOKEN_T;

extern void			initChtype			(void);
extern TOKEN_T*		getLine_token		(void);
extern TOKEN_T*		get_token			(void);
extern void			release_token		(TOKEN_T*);
extern int			checkToken			(FILE_INFO_T);

#ifdef __cplusplus
}
#endif

#endif/*__BBI_TOKEN_H__*/
