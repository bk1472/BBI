/***********************************************************************/
/*    filename:bbi.cpp ���� ó��                                      */
/*    BBI ���������� (Rev 1.01) Copyright (c) 2011 Haruhiko Hayashi   */
/***********************************************************************/
#include "bbi.h"
#include "bbi_prot.h"

int main(int argc, char *argv[])
{
	if (argc == 1) { cout << "���: bbi filename\n"; exit(1); }
	convert_to_internalCode(argv[1]);
	syntaxChk();
	execute();
	return 0;
}

