/* ��������������������������������������������������������������������� */
/* Visual Basic Disassembled String Reference Analyser        (c) DEATH  */
/* ��������������������������������������������������������������������� */
/* import.h                                                              */
/*                                                                       */
/* Import references functions include file                              */
/* ��������������������������������������������������������������������� */
#ifndef _IMPORT_H
#define _IMPORT_H


#include "vb.h"

extern char szImportRef[MAX_IMPREF][256];

DWORD GetImports(char *Filename);


#endif /* _IMPORT_H */
