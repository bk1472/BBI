#include	"bbi_type.h"
#include	"bbi_fio.h"
#include	"bbi_token.h"

#define LINE_SIZE		(500)
#define LAST_LINE		(-1)

static FILE 	*InputFile = NULL;
static char		lineBuf[LINE_SIZE];

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
			return LAST_LINE;
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
		}
	}
	*(tmpBuf++) = '\0';

	lineNum++;


	return (lineNum);
}

FILE_INFO_T getLine(void)
{
	FILE_INFO_T finfo;

	finfo.line = -1;
	finfo.pBuf = NULL;

	if (InputFile == NULL)
		return finfo;

	memset(&lineBuf[0], 0x00, LINE_SIZE);
	finfo.line = readline(&lineBuf[0], LINE_SIZE, InputFile);
	if (finfo.line == LAST_LINE)
	{
		finfo.line = -1;
		srcFileclose();
	}
	finfo.pBuf = &lineBuf[0];
	return finfo;
}



FILE * srcFileOpen(char* fileName)
{

	InputFile = fopen(fileName, "rd");

	if(InputFile == NULL)
	{
		printf("Can not open %s file!\n", fileName);
		exit(1);
	}

	return InputFile;
}

void srcFileclose(void)
{
	if (InputFile != NULL)
	{
		fclose(InputFile);
		InputFile = NULL;
	}
}
