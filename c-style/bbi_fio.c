#include	"bbi_type.h"

static FILE *InputFile = NULL;

FILE * srcFileOpen(char* fileName)
{

	if (InputFile == NULL)
		InputFile = fopen(fileName, "rd");
	else
		fseek(InputFile, 0, SEEK_SET);

	return InputFile;
}
