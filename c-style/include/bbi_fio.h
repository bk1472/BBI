#ifndef __BBI_FIO_H__
#define __BBI_FIO_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _f_info_
{
	int		line;
	char	*pBuf;
} FILE_INFO_T;

extern FILE_INFO_T	getLine				(void);
extern FILE*		srcFileOpen			(char* fileName);
extern void			srcFileclose		(void);

#ifdef __cplusplus
}
#endif

#endif/*__BBI_FIO_H__*/
