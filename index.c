/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
/* Visual Basic Disassembled String Reference Analyser        (c) DEATH  */
/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
/* index.c                                                               */
/*                                                                       */
/* Index file handling functions                                         */
/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
#include <windows.h>
#include <stdio.h>

#include "include/vb.h"
#include "include/import.h"
#include "include/export.h"
#include "include/extern.h"
#include "include/index.h"

/* Write VBRef index file */
BOOL WriteIndex(char *szFilename)
{
    FILE *filep;
    DWORD dwCount;

    /* Index file */
    filep = fopen(szFilename, "wb");

    if (filep == NULL) return(FALSE);

    /* Number of lines */
    fwrite(&dwNumLines, sizeof(DWORD), 1, filep);

    /* Offset index */
    fwrite(&dwIndexOffset[0], sizeof(DWORD), dwNumLines, filep);

    /* Entry point */
    fwrite(&dwEntryPoint, sizeof(DWORD), 1, filep);

    /* Number of String references */
    fwrite(&dwNumRefs, sizeof(DWORD), 1, filep);

    /* Write references */
    for (dwCount = 0; dwCount < dwNumRefs; dwCount++)
        fwrite(&szStrRef[dwCount], 1, strlen(szStrRef[dwCount]) + 1, filep);

    /* Number of Import references */
    fwrite(&dwNumImpRefs, sizeof(DWORD), 1, filep);

    /* Write import references */
    for (dwCount = 0; dwCount < dwNumImpRefs; dwCount++)
        fwrite(&szImportRef[dwCount], 1, strlen(szImportRef[dwCount]) + 1, filep);

    /* Number of Export references */
    fwrite(&dwNumExpRefs, sizeof(DWORD), 1, filep);

    /* Write export references */
    for (dwCount = 0; dwCount < dwNumExpRefs; dwCount++)
        fwrite(&szExportRef[dwCount], 1, strlen(szExportRef[dwCount]) + 1, filep);

    /* Close index file */
    fclose(filep);

    return(TRUE);
}

/* Read VBRef index file */
BOOL ReadIndex(char *szFilename)
{
    FILE *filep;
    DWORD dwCount;

    /* Open index file */
    filep = fopen(szFilename, "rb");

    if (filep == NULL) return(FALSE);

    /* Get number of lines in disassembly */
    fread(&dwNumLines, sizeof(DWORD), 1, filep);

    /* Allocate space */
    dwIndexOffset = malloc(dwNumLines * 4);

    /* Read offsets indexing */
    fread(&dwIndexOffset[0], sizeof(DWORD), dwNumLines, filep);

    /* Read entry point */
    fread(&dwEntryPoint, sizeof(DWORD), 1, filep);

    /* Read number of String References */
    fread(&dwNumRefs, sizeof(DWORD), 1, filep);

    /* Get String References */
    for (dwCount = 0; dwCount < dwNumRefs; dwCount++)
        GetASCIIZString(&szStrRef[dwCount], filep);

    /* Read number of Import References */
    fread(&dwNumImpRefs, sizeof(DWORD), 1, filep);

    /* Get Import References */
    for (dwCount = 0; dwCount < dwNumImpRefs; dwCount++)
        GetASCIIZString(&szImportRef[dwCount], filep);

    /* Read number of Export References */
    fread(&dwNumExpRefs, sizeof(DWORD), 1, filep);

    /* Get Import References */
    for (dwCount = 0; dwCount < dwNumExpRefs; dwCount++)
        GetASCIIZString(&szExportRef[dwCount], filep);

    /* Close index file */
    fclose(filep);

    return(TRUE);
}

/* Index disassembly lines */
DWORD *IndexLines(void)
{
    FILE *filep;
    DWORD *dwIndexOffset;
    char strDasm[512];
    DWORD dwCurrRef;
    DWORD dwCount;
    DWORD dwTemp;
    BOOL flDupe;
    short iLastPercent;

    dwIndexOffset = malloc(sizeof(DWORD) * dwNumLines);

    if (dwIndexOffset == NULL)
        return(dwIndexOffset);

    /* Open created disassembly */
    filep = fopen(szNameVBR, "rb");

    if (filep == NULL) {
        free(dwIndexOffset);
        dwIndexOffset = NULL;
        return(dwIndexOffset);
    }

    dwCurrRef = 0;
    iLastPercent = 0;

    for (dwCount = 0; dwCount < dwNumLines; dwCount++) {
        dwIndexOffset[dwCount] = ftell(filep);
        fgets(strDasm, sizeof(strDasm), filep);

        if (strncmp(strDasm, cszSDRID, strlen(cszSDRID)) == 0) {
            strcpy(szStrRef[dwCurrRef], &strDasm[43]);

            if (strchr(szStrRef[dwCurrRef], '\r')) *strchr(szStrRef[dwCurrRef], '\r') = '\0';
            if (strchr(szStrRef[dwCurrRef], '\n')) *strchr(szStrRef[dwCurrRef], '\n') = '\0';

            for (dwTemp = 0; szStrRef[dwCurrRef][dwTemp]; dwTemp++);

            dwTemp--;
            szStrRef[dwCurrRef][dwTemp] = '\0';

            flDupe = FALSE;

            /* Check String Reference dupes */
            for (dwTemp = 0; dwTemp < dwCurrRef; dwTemp++) {
                if (strcmp(szStrRef[dwTemp], szStrRef[dwCurrRef]) == 0) {
                    flDupe = TRUE;
                    break;
                }
            }

            if (flDupe == FALSE)
                dwCurrRef++;
        }

        if (((dwCount * 100) / dwNumLines) != iLastPercent) {
            iLastPercent = (dwCount * 100) / dwNumLines;
            sprintf(strDasm, "Indexing lines (%%%d)...", iLastPercent);
            SetWindowText(hwndStatus, strDasm);
        }
    }

    dwNumRefs = dwCurrRef;

    memset(strDasm, 0, sizeof(strDasm));

    /* Close new disassembly file */
    fclose(filep);

    return(dwIndexOffset);
}
