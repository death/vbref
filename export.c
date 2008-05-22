/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
/* Visual Basic Disassembled String Reference Analyser        (c) DEATH  */
/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
/* export.c                                                              */
/*                                                                       */
/* Export references handling functions                                  */
/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
#include <windows.h>
#include <imagehlp.h>
#include <stdio.h>

#include "include/vb.h"
#include "include/pe.h"
#include "include/export.h"

/* Export references */
char szExportRef[MAX_EXPREF][256];

/* Get all exports */
DWORD GetExports(char *Filename)
{
    IMAGE_DOS_HEADER        imgDos;
    IMAGE_NT_HEADERS        imgNt;
    IMAGE_EXPORT_DIRECTORY  imgExport;
    FILE *filep;
    short iCount;
    DWORD dwSave;
    DWORD dwOffset;

    iCount = 0;

    filep = fopen(Filename, "rb");

    if (filep) {

        fread(&imgDos, sizeof(imgDos), 1, filep);

        if (imgDos.e_magic == IMAGE_DOS_SIGNATURE) {

            fseek(filep, imgDos.e_lfanew, SEEK_SET);

            fread(&imgNt, IMAGE_SIZEOF_NT_OPTIONAL_HEADER, 1, filep);

            if (imgNt.Signature == IMAGE_NT_SIGNATURE) {

                fseek(filep, VA2Phys(Filename, imgNt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress), SEEK_SET);

                fread(&imgExport, sizeof(imgExport), 1, filep);

                if (imgExport.Name == 0) {
                    fclose(filep);
                    return(0);
                }

                fseek(filep, VA2Phys(Filename, (DWORD )imgExport.AddressOfNames), SEEK_SET);

                for (iCount = 0; iCount < imgExport.NumberOfNames; iCount++) {

                    fread(&dwOffset, sizeof(DWORD), 1, filep);
                    dwSave = ftell(filep);

                    fseek(filep, VA2Phys(Filename, dwOffset), SEEK_SET);

                    GetASCIIZString(&szExportRef[iCount], filep);

                    fseek(filep, dwSave, SEEK_SET);

                }

            }

        }

        fclose(filep);
    }

    return(iCount);
}

/* Get name of export by ordinal */
DWORD GetExport(char *Filename, char *szStr, DWORD dwExport)
{
    IMAGE_DOS_HEADER        imgDos;
    IMAGE_NT_HEADERS        imgNt;
    IMAGE_EXPORT_DIRECTORY  imgExport;
    FILE *filep;
    WORD iOrdinal;
    DWORD dwOffset;
    WORD iCount;

    filep = fopen(Filename, "rb");

    if (filep) {

        fread(&imgDos, sizeof(imgDos), 1, filep);

        if (imgDos.e_magic == IMAGE_DOS_SIGNATURE) {

            fseek(filep, imgDos.e_lfanew, SEEK_SET);

            fread(&imgNt, IMAGE_SIZEOF_NT_OPTIONAL_HEADER, 1, filep);

            if (imgNt.Signature == IMAGE_NT_SIGNATURE) {

                fseek(filep, VA2Phys(Filename, imgNt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress), SEEK_SET);

                fread(&imgExport, sizeof(imgExport), 1, filep);

                if (imgExport.Name == 0) {
                    fclose(filep);
                    return(0);
                }

                fseek(filep, VA2Phys(Filename, (DWORD )imgExport.AddressOfNameOrdinals), SEEK_SET);

                if (dwExport < imgExport.NumberOfNames) {
                    for (iCount = 0; iCount < imgExport.NumberOfNames; iCount++) {
                        fread(&iOrdinal, sizeof(WORD), 1, filep);
                        if (iOrdinal == (dwExport - 100)) {
                            break;
                        }
                    }
                    fseek(filep, VA2Phys(Filename, (DWORD )imgExport.AddressOfNames + iCount * sizeof(DWORD)), SEEK_SET);
                    fread(&dwOffset, sizeof(DWORD), 1, filep);
                    fseek(filep, VA2Phys(Filename, dwOffset), SEEK_SET);
                    GetASCIIZString(szStr, filep);

                    fclose(filep);
                    return(dwExport);
                }

            }

        }

        fclose(filep);
    }

    return(0xFFFFFFFF);
}
