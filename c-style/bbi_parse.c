#include	"bbi_type.h"
#include	"bbi_fio.h"
#include	"bbi_parse.h"

#define LINE_SIZE		(500)

#define CR				(0x0d)
#define LF				(0x0a)
#define TAB				(0x09)
#define SPACE			(0x20)

static char lineBuf[LINE_SIZE];

static int readline (char *pBuf, int maxLineSize, FILE *fp)
{
	static int	lineNum  = 0;
	int			lineByte = 0;
	char*		tmpBuf   = pBuf;


	while (1)
	{
		char c = (char)fgetc(fp);

		if (c == EOF)
		{
			lineNum = 0;
			return -1;
		}

		if (c == CR)
		{
			c = (char)fgetc(fp);
			break;
		}

		*(tmpBuf++) = c;
		lineByte++;
		if(lineByte > maxLineSize)
		{
			fprintf(stderr, "Line Buffer Max Size %d !\n", maxLineSize);
			exit(-1); //TODO : 나중에 통합 에러 핸들링을 해야 함.
			//return -2;
		}
	}
	*(tmpBuf++) = '\0';

	lineNum++;


	return (lineNum);
}

void parse_code (FILE *fp)
{
	int line = 0;

	while (1)
	{
		line = readline(&lineBuf[0], LINE_SIZE, fp);

		fprintf(stdout, "%s\n", &lineBuf[0]);
		if (line == -1)
			break;
	}
}
