/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
/* Visual Basic Disassembled String Reference Analyser        (c) DEATH  */
/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
/* import.c                                                              */
/*                                                                       */
/* Import references handling function                                   */
/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
#include <windows.h>
#include <imagehlp.h>
#include <stdio.h>

#include "include/vb.h"
#include "include/pe.h"
#include "include/export.h"
#include "include/import.h"

/* Import references */
char szImportRef[MAX_IMPREF][256];

/* Get all imports */
DWORD GetImports(char *Filename)
{
    IMAGE_DOS_HEADER        imgDos;
    IMAGE_NT_HEADERS        imgNt;
    IMAGE_IMPORT_DESCRIPTOR imgImport;
    IMAGE_THUNK_DATA        imgTData;
    FILE *filep;
    DWORD dwSave1;
    DWORD dwSave2;
    char szBuffer[128];
    char szDll[128];
    char szDllPath[256];
    short iCount;

    iCount = 0;

    filep = fopen(Filename, "rb");

    if (filep) {

        fread(&imgDos, sizeof(imgDos), 1, filep);

        if (imgDos.e_magic == IMAGE_DOS_SIGNATURE) {

            fseek(filep, imgDos.e_lfanew, SEEK_SET);

            fread(&imgNt, IMAGE_SIZEOF_NT_OPTIONAL_HEADER, 1, filep);

            if (imgNt.Signature == IMAGE_NT_SIGNATURE) {

                fseek(filep, VA2Phys(Filename, imgNt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress), SEEK_SET);

                do {

                    fread(&imgImport, sizeof(imgImport), 1, filep);

                    dwSave1 = ftell(filep);

                    if (imgImport.Name == 0) break;

                    fseek(filep, VA2Phys(Filename, imgImport.Name), SEEK_SET);

                    GetASCIIZString(&szDll, filep);

                    if (strchr(szDll, '.')) *strchr(szDll, '.') = '\0';

                    if (imgImport.OriginalFirstThunk != 0)
                        fseek(filep, VA2Phys(Filename, (DWORD )imgImport.OriginalFirstThunk), SEEK_SET);
                    else
                        fseek(filep, VA2Phys(Filename, (DWORD )imgImport.FirstThunk), SEEK_SET);

                    do {

                        fread(&imgTData, sizeof(imgTData), 1, filep);

                        dwSave2 = ftell(filep);

                        if (imgTData.u1.Function == NULL) break;

                        if (iCount < MAX_IMPREF) {

                            if (imgTData.u1.Ordinal & IMAGE_ORDINAL_FLAG) {

                                /* Convert to name (from ordinal) */
                                GetSystemDirectory(szDllPath, sizeof(szDllPath));
                                strcat(szDllPath, "\\");
                                strcat(szDllPath, szDll);
                                strcat(szDllPath, ".DLL");

                                GetExport(szDllPath, &szBuffer, IMAGE_ORDINAL(imgTData.u1.Ordinal));
                                sprintf(szImportRef[iCount], "%s.%s", szDll, szBuffer);

                                iCount++;

                            } else {

                                fseek(filep, VA2Phys(Filename, (DWORD )imgTData.u1.Function) + 2, SEEK_SET);
                                GetASCIIZString(&szBuffer, filep);
                                sprintf(szImportRef[iCount], "%s.%s", szDll, szBuffer);
                                iCount++;

                            }

                        }

                        fseek(filep, dwSave2, SEEK_SET);

                    } while (1);

                    fseek(filep, dwSave1, SEEK_SET);

                } while (1);

            }

        }

        fclose(filep);
    }

    return(iCount);
}
