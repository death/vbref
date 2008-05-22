/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
/* Visual Basic Disassembled String Reference Analyser        (c) DEATH  */
/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
/* extern.h                                                              */
/*                                                                       */
/* External global variables (from vbref.c) include file                 */
/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
#ifndef _EXTERN_H
#define _EXTERN_H


extern const char cszFilterALF[];
extern const char cszFilterEXE[];
extern const char cszFilterVBR[];
extern const char cszChooseOpen[];
extern const char cszChooseSave[];
extern const char cszDefExtALF[];
extern const char cszDefExtEXE[];
extern const char cszDefExtVBR[];
extern const char cszSDRID[];
extern const char cszENDID[];
extern const char cszIMPID[];
extern const char cszEXPID[];
extern const char cszViewerClass[];

extern char szNameTMP[256];
extern char szNameALF[256];
extern char szNameEXE[256];
extern char szNameVBR[256];
extern char szNameIDX[256];
extern char szStrRef[MAX_STRREF][512];
extern vstrref_struct vstrref[MAX_STRREF];
extern DWORD *dwIndexOffset;
extern BOOL flOverwrite;
extern HINSTANCE hInst;
extern HFONT hFont;
extern DWORD dwNumRefs;
extern DWORD dwNumImpRefs;
extern DWORD dwNumExpRefs;
extern DWORD dwEntryPoint;
extern HWND hwndMain;
extern HWND hwndToolbar;
extern HWND hwndStatus;
extern HWND hwndViewer;

extern BOOL flViewing;
extern short cxClient;
extern short cyClient;
extern short cxChar;
extern short cxCaps;
extern long cyChar;
extern long dwVscrollPos;
extern long dwNumLines;
extern long dwCurrBar;


#endif /* _EXTERN_H */
