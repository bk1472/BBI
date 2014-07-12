#include	"bbi_type.h"
#include	"bbi_fio.h"
#include	"bbi_parse.h"

int main (int argc, char *argv[])
{
	if (argc != 2) { fprintf(stderr, "Usage: bbi <file name>\n"); exit(1); }

	parse_code(srcFileOpen(argv[1]));

	return 0;
}
