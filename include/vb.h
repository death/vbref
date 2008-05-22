/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
/* Visual Basic Disassembled String Reference Analyser        (c) DEATH  */
/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
/* vb.h                                                                  */
/*                                                                       */
/* Visual Basic string references functions include file                 */
/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
#ifndef _VB_H
#define _VB_H


#define MAX_STRREF 500
#define MAX_IMPREF 256
#define MAX_EXPREF 256

/* String reference structure */
typedef struct {
    long rva;
    char strrva[9];
    char wcString[512];
} vstrref_struct;

extern long strref[MAX_STRREF];
extern short strref_counter;

void add_strref(long pos);
void GetWCString(char *str, FILE *filep, long pos);
void search_strref(char *filename);
void GetASCIIZString(char *szStr, FILE *filep);


#endif /* _VB_H */
