/* ��������������������������������������������������������������������� */
/* Visual Basic Disassembled String Reference Analyser        (c) DEATH  */
/* ��������������������������������������������������������������������� */
/* index.h                                                               */
/*                                                                       */
/* Index file functions include file                                     */
/* ��������������������������������������������������������������������� */
#ifndef _INDEX_H
#define _INDEX_H


BOOL WriteIndex(char *szFilename);
BOOL ReadIndex(char *szFilename);
DWORD *IndexLines(void);


#endif /* _INDEX_H */
