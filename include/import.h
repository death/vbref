/* 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴� */
/* Visual Basic Disassembled String Reference Analyser        (c) DEATH  */
/* 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴� */
/* import.h                                                              */
/*                                                                       */
/* Import references functions include file                              */
/* 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴� */
#ifndef _IMPORT_H
#define _IMPORT_H


#include "vb.h"

extern char szImportRef[MAX_IMPREF][256];

DWORD GetImports(char *Filename);


#endif /* _IMPORT_H */
