/* 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴� */
/* Visual Basic Disassembled String Reference Analyser        (c) DEATH  */
/* 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴� */
/* pe.h                                                                  */
/*                                                                       */
/* Portable Executable functions include file                            */
/* 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴� */
#ifndef _PE_H
#define _PE_H


long Phys2VA(char *filename, long phys);
long Phys2RVA(char *filename, long phys);
DWORD VA2Phys(char *Filename, DWORD va);
DWORD RVA2Phys(char *Filename, DWORD rva);
DWORD GetEP(char *Filename);


#endif /* _PE_H */
