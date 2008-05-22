/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
/* Visual Basic Disassembled String Reference Analyser        (c) DEATH  */
/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
/* vb.c                                                                  */
/*                                                                       */
/* Visual Basic string references handling functions                     */
/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
#include <windows.h>
#include <imagehlp.h>
#include <stdio.h>

#include "include/vb.h"

/* String reference offsets */
long strref[MAX_STRREF];
short strref_counter;

/* Search for wide char string references */
void search_strref(char *filename)
{
    HANDLE hFile;
    HANDLE hMap;
    char *pMap;
    char *pFile;
    short data;
    long curpos;
    long filesize;

    hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return;

    filesize = GetFileSize(hFile, NULL);
    hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, filesize, NULL);

    pMap = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);

    pFile = pMap;

    /* -------------------------- */

    curpos = 0;

    while (1) {
        if (pFile >= (pMap + filesize)) break;

        data = *pFile++;
        curpos = pFile - pMap;

        if (data < 128 && data > 31) {
            if ((*pFile++) == 0) {
                data = *pFile++;
                if (data < 128 && data > 31) {
                    if (*pFile++ == 0) {
                        data = *pFile++;
                        if (data < 128 && data > 31) {
                            if (*pFile++ == 0) {

                                while (1) {
                                    data = *pFile++;
                                    pFile++;
                                    if (pFile >= (pMap + filesize)) break;
                                    if (data == 0) {
                                        add_strref(curpos - 1);
                                        curpos = (pFile - pMap) - 1;
                                        break;
                                    }
                                    if (data < 32 || data > 127)
                                        break;
                                }
                            }
                        }
                    }
                }
            }
        }
        pFile = pMap + curpos;
    }

    UnmapViewOfFile(pMap);

    CloseHandle(hMap);
    CloseHandle(hFile);
}

/* Add a string reference */
void add_strref(long pos)
{
    if (strref_counter < MAX_STRREF) {
        strref[strref_counter] = pos;
        strref_counter++;
    }
}

/* Get wide char string */
void GetWCString(char *str, FILE *filep, long pos)
{
    short data;
    short count;

    fseek(filep, pos, SEEK_SET);

    count = 0;

    while (!feof(filep) && !ferror(filep)) {
        data = fgetc(filep);
        fgetc(filep);
        str[count] = data;
        if (data == 0) break;
        count++;
    }
}

/* Get ASCIIZ (NULL terminated) string */
void GetASCIIZString(char *szStr, FILE *filep)
{
    DWORD dwCount;
    short ch;

    ch = fgetc(filep);
    dwCount = 0;
    if (ch == EOF)
        szStr[dwCount] = '\0';
    else
        szStr[dwCount] = ch;

    while (ch != EOF && ch != '\0') {
        dwCount++;
        ch = fgetc(filep);
        if (ch == EOF)
            szStr[dwCount] = '\0';
        else
            szStr[dwCount] = ch;
    }
}
