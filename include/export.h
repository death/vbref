/* 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴� */
/* Visual Basic Disassembled String Reference Analyser        (c) DEATH  */
/* 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴� */
/* export.h                                                              */
/*                                                                       */
/* Export references functions include file                              */
/* 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴� */
#ifndef _EXPORT_H
#define _EXPORT_H


#include "vb.h"

extern char szExportRef[MAX_EXPREF][256];

DWORD GetExports(char *Filename);
DWORD GetExport(char *Filename, char *szStr, DWORD dwExport);


#endif /* _EXPORT_H */
