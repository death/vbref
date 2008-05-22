/* 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴� */
/* Visual Basic Disassembled String Reference Analyser        (c) DEATH  */
/* 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴� */
/* vbref.c                                                               */
/*                                                                       */
/* Main programme                                                        */
/* 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴� */

/* Includes */
#include <stdio.h>
#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>

#include "include/export.h"             /* Export handling functions */
#include "include/import.h"             /* Import handling functions */
#include "include/vb.h"                 /* VB EXE's related functions */
#include "include/pe.h"                 /* Portable Executable functions */
#include "include/config.h"             /* Configuration functions */
#include "include/index.h"              /* Index file handling functions */
#include "include/vbref.h"              /* Resource definitions */

/* Buttons in Toolbar */
TBBUTTON tbb[] = {
    {0, IDM_OPENALF, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0},
    {1, IDM_SAVEVBR, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0},
    {2, IDM_OPENVBR, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0},
    {0, 0,           TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, 0},
    {3, IDM_STRREF,  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0},
    {4, IDM_TEXT,    TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0},
    {5, IDM_IMPORT,  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0},
    {6, IDM_EXPORT,  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0},
    {0, 0,           TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0, 0},
    {7, IDM_GOTOEP,  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0},
    {8, IDM_GOTOLOC, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0}
};

/* Constants */
const char cszFilterALF[] = "W32Dasm disassembly text\0*.ALF\0\0";
const char cszFilterEXE[] = "Executables\0*.EXE;*.DLL;*.OCX\0\0";
const char cszFilterVBR[] = "VBRef disassembly\0*.VBR\0\0";
const char cszChooseOpen[] = "Choose a file to open";
const char cszChooseSave[] = "Choose a file to save";
const char cszDefExtALF[] = "ALF";
const char cszDefExtEXE[] = "EXE";
const char cszDefExtVBR[] = "VBR";
const char cszSDRID[] = "* Possible StringData Ref";
const char cszENDID[] = "                                  |";
const char cszIMPID[] = "* Reference To: ";
const char cszEXPID[] = "Exported fn(): ";
const char cszViewerClass[] = "ViewerClass";

/* Global variables */
char szNameTMP[256];
char szNameALF[256];
char szNameEXE[256];
char szNameVBR[256];
char szNameIDX[256];
char szStrRef[MAX_STRREF][512];
vstrref_struct vstrref[MAX_STRREF];
DWORD *dwIndexOffset;
BOOL flOverwrite;
HINSTANCE hInst;
HFONT hFont;
DWORD dwNumRefs;
DWORD dwNumImpRefs;
DWORD dwNumExpRefs;
DWORD dwEntryPoint;
HWND hwndMain;
HWND hwndToolbar;
HWND hwndStatus;
HWND hwndViewer;

BOOL flViewing;
short cxClient;
short cyClient;
short cxChar;
short cxCaps;
long cyChar;
long dwVscrollPos;
long dwNumLines;
long dwCurrBar;

/* Function prototypes */
BOOL CALLBACK DlgMain(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgStrRef(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgText(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgImportRef(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgExportRef(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgGotoLocation(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgAbout(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ViewerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void GotoEntryPoint(void);
void ChangeExtension(char *szFilename, char *szExtension);
DWORD GetFileLen(FILE *filep);
void Reconstruct_Disassembly(void);
char *GetLine(char *pFile, char *pMap, char *szStr, DWORD dwFileSize);

/* Main function */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    WNDCLASSEX wc;

    hInst = hInstance;
    InitCommonControls();

    /* Register viewer window class */
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = ViewerWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hIconSm = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH )(GetStockObject(LTGRAY_BRUSH));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = cszViewerClass;

    RegisterClassEx(&wc);

    /* Main dialog box */
    DialogBox(hInstance, MAKEINTRESOURCE(DLG_MAIN), NULL, DlgMain);

    return(msg.wParam);
}

/* Main dialog box function */
BOOL CALLBACK DlgMain(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HMENU hMenu;
    OPENFILENAME ofn;
    long dwCount;
    FILE *filep;
    static BOOL flSaved;
    HICON hIcon;
    LPNMHDR pnmh;
    LPTOOLTIPTEXT lpttt;
    RECT rectToolbar;
    RECT rectStatus;
    RECT rectMain;
    short cyMain;
    short cxMain;

    switch (uMsg) {
        case WM_INITDIALOG:
            /* Initialize dialog */
            hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_EXEC));
            SetClassLong(hDlg, GCL_HICON, (LONG )hIcon);

            /* Set menu */
            hMenu = LoadMenu(hInst, "MAIN");
            SetMenu(hDlg, hMenu);

            /* Get font */
            hFont = CreateFont(10, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, OEM_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE , "Terminal");

            /* Create Toolbar */
            hwndToolbar = CreateToolbarEx(hDlg,
                WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_TOP | TBSTYLE_TOOLTIPS,
                1, 9, hInst, IDB_TOOLBAR, tbb,
                sizeof(tbb) / sizeof(TBBUTTON), 0, 0, 16, 16, sizeof(TBBUTTON));

            /* Create Status bar */
            hwndStatus = CreateStatusWindow(
                WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_BOTTOM | SBARS_SIZEGRIP,
                "Ready", hDlg, 2);

            /* Create viewer window */
            hwndViewer = CreateWindow(cszViewerClass, NULL, WS_CHILDWINDOW | WS_VISIBLE, 0, 0, 0, 0, hDlg, (HMENU )3, hInst, NULL);

            /* Set variables */
            flViewing = FALSE;
            flSaved = TRUE;

            hwndMain = hDlg;

            /* Get settings */
            if (GetSetting("bottom", (LPDWORD )&rectMain.bottom) == TRUE) {
                if (GetSetting("top", (LPDWORD )&rectMain.top) == TRUE) {
                    if (GetSetting("left", (LPDWORD )&rectMain.left) == TRUE) {
                        if (GetSetting("right", (LPDWORD )&rectMain.right) == TRUE) {
                            /* Resize window */
                            MoveWindow(hwndMain, rectMain.left, rectMain.top, rectMain.right - rectMain.left, rectMain.bottom - rectMain.top, TRUE);
                        }
                    }
                }
            }
            break;
        case WM_COMMAND:
            switch (wParam) {
                case IDM_OPENALF:
                    /* Open ALF file */
                    if (flSaved == FALSE) {
                        /* Not saved previous disassembly */
                        switch (MessageBox(hDlg, "You haven\'t saved the new disassembly, would you like to save it?", "Save?", MB_YESNOCANCEL)) {
                            case IDNO:
                                /* Don't want to save */
                                break;
                            case IDYES:
                                /* Save before load */
                                SendMessage(hDlg, WM_COMMAND, IDM_SAVEVBR, 0);
                                break;
                            case IDCANCEL:
                                /* Cancel loading */
                                return(FALSE);
                                break;
                            default:
                                break;
                        }
                    }

                    /* Get ALF file name */
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = hDlg;
                    ofn.hInstance = hInst;
                    ofn.lpstrFilter = &cszFilterALF;
                    ofn.lpstrCustomFilter = NULL;
                    ofn.nFilterIndex = 0;
                    ofn.lpstrFile = &szNameALF;
                    ofn.nMaxFile = sizeof(szNameALF);
                    ofn.lpstrFileTitle = NULL;
                    ofn.lpstrInitialDir = NULL;
                    ofn.lpstrTitle = &cszChooseOpen;
                    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_NOCHANGEDIR | OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST;
                    ofn.lpstrDefExt = &cszDefExtALF;
                    if (GetOpenFileName(&ofn) == FALSE)
                        break;

                    /* Get EXE file name */
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = hDlg;
                    ofn.hInstance = hInst;
                    ofn.lpstrFilter = &cszFilterEXE;
                    ofn.lpstrCustomFilter = NULL;
                    ofn.nFilterIndex = 0;
                    ofn.lpstrFile = &szNameEXE;
                    ofn.nMaxFile = sizeof(szNameEXE);
                    ofn.lpstrFileTitle = NULL;
                    ofn.lpstrInitialDir = NULL;
                    ofn.lpstrTitle = &cszChooseOpen;
                    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_NOCHANGEDIR | OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST;
                    ofn.lpstrDefExt = &cszDefExtEXE;
                    if (GetOpenFileName(&ofn) == FALSE)
                        break;

                    /* Remove any temporary files */
                    if (flSaved == FALSE) {
                        remove(szNameVBR);
                        remove(szNameIDX);
                    }

                    /* Initialize */
                    if (dwIndexOffset) {
                        free(dwIndexOffset);
                        dwIndexOffset = NULL;
                    }

                    dwNumLines = 0;
                    flSaved = FALSE;

                    /* Get imports */
                    SetWindowText(hwndStatus, "Getting Imports...");
                    dwNumImpRefs = GetImports(szNameEXE);

                    /* Get exports */
                    SetWindowText(hwndStatus, "Getting exports...");
                    dwNumExpRefs = GetExports(szNameEXE);

                    /* Get entry point */
                    dwEntryPoint = GetEP(szNameEXE);

                    /* Find references */
                    SetWindowText(hwndStatus, "Finding string references...");
                    search_strref(szNameEXE);

                    /* Convert positions to Relative Virtual Addresses */
                    SetWindowText(hwndStatus, "Converting positions...");

                    for (dwCount = 0; dwCount < strref_counter; dwCount++) {
                        vstrref[dwCount].rva = Phys2RVA(szNameEXE, strref[dwCount]);
                        sprintf(vstrref[dwCount].strrva, "%08lX", vstrref[dwCount].rva);

                        filep = fopen(szNameEXE, "rb");
                        GetWCString(vstrref[dwCount].wcString, filep, strref[dwCount]);
                        fclose(filep);
                    }

                    /* Reconstruct disassembly text file */
                    SetWindowText(hwndStatus, "Reconstructing disassembly text file (%0)...");

                    Reconstruct_Disassembly();

                    /* Index lines */
                    SetWindowText(hwndStatus, "Indexing lines (%0)...");

                    dwIndexOffset = IndexLines();

                    if (dwIndexOffset == NULL) {
                        MessageBox(hDlg, "Not enough memory, programme will be terminated", "Error", MB_OK);
                        SendMessage(hDlg, WM_CLOSE, 0, 0);
                        return(FALSE);
                    }

                    /* Write index file */
                    tmpnam(szNameIDX);
                    WriteIndex(szNameIDX);

                    /* Initialize vertical scroll bar */
                    dwVscrollPos = 0;
                    dwCurrBar = 0;
                    flViewing = TRUE;

                    /* Update status */
                    SetWindowText(hwndStatus, "ALF file loaded successfuly");

                    /* Update viewing window */
                    SetScrollPos(hwndViewer, SB_VERT, dwVscrollPos, TRUE);
                    SetScrollRange(hwndViewer, SB_VERT, 0, dwNumLines, FALSE);

                    InvalidateRect(hwndViewer, NULL, TRUE);
                    break;
                case IDM_SAVEVBR:
                    /* Save to VBR */
                    if (flSaved == TRUE) break;

                    /* Get VBR filename */
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = hDlg;
                    ofn.hInstance = hInst;
                    ofn.lpstrFilter = &cszFilterVBR;
                    ofn.lpstrCustomFilter = NULL;
                    ofn.nFilterIndex = 0;
                    ofn.lpstrFile = &szNameTMP;
                    ofn.nMaxFile = sizeof(szNameTMP);
                    ofn.lpstrFileTitle = NULL;
                    ofn.lpstrInitialDir = NULL;
                    ofn.lpstrTitle = &cszChooseSave;
                    ofn.Flags = OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_NOCHANGEDIR | OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST;
                    ofn.lpstrDefExt = &cszDefExtVBR;
                    if (GetSaveFileName(&ofn) == FALSE) {
                        InvalidateRect(hDlg, NULL, TRUE);
                        break;
                    }

                    /* Save */
                    rename(szNameVBR, szNameTMP);
                    strcpy(szNameVBR, szNameTMP);
                    ChangeExtension(szNameTMP, "IDX");
                    rename(szNameIDX, szNameTMP);
                    strcpy(szNameIDX, szNameTMP);

                    flSaved = TRUE;

                    InvalidateRect(hwndViewer, NULL, TRUE);

                    /* Update status */
                    SetWindowText(hwndStatus, "VBR file saved successfuly");
                    break;
                case IDM_OPENVBR:
                    /* Open VBR file */
                    if (flSaved == FALSE) {
                        /* User didn't save */
                        switch (MessageBox(hDlg, "You haven\'t saved the new disassembly, would you like to save it?", "Save?", MB_YESNOCANCEL)) {
                            case IDNO:
                                /* Don't want to save */
                                break;
                            case IDYES:
                                /* Save before loading */
                                SendMessage(hDlg, WM_COMMAND, IDM_SAVEVBR, 0);
                                break;
                            case IDCANCEL:
                                /* User doesn't want to open VBR file */
                                return(FALSE);
                                break;
                            default:
                                break;
                        }
                    }

                    /* Get VBR filename */
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = hDlg;
                    ofn.hInstance = hInst;
                    ofn.lpstrFilter = &cszFilterVBR;
                    ofn.lpstrCustomFilter = NULL;
                    ofn.nFilterIndex = 0;
                    ofn.lpstrFile = &szNameTMP;
                    ofn.nMaxFile = sizeof(szNameTMP);
                    ofn.lpstrFileTitle = NULL;
                    ofn.lpstrInitialDir = NULL;
                    ofn.lpstrTitle = &cszChooseOpen;
                    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_NOCHANGEDIR | OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST;
                    ofn.lpstrDefExt = &cszDefExtVBR;
                    if (GetOpenFileName(&ofn) == FALSE)
                        break;

                    /* Erase temporary files */
                    if (flSaved == FALSE) {
                        remove(szNameVBR);
                        remove(szNameIDX);
                    }

                    strcpy(szNameVBR, szNameTMP);
                    strcpy(szNameIDX, szNameTMP);
                    ChangeExtension(szNameIDX, "IDX");

                    /* Read index file */
                    if (ReadIndex(szNameIDX) == FALSE) {
                        MessageBox(hDlg, "Cannot open index file!", "Error", MB_OK);
                        break;
                    }

                    if (dwIndexOffset) {
                        free(dwIndexOffset);
                        dwIndexOffset = FALSE;
                    }

                    dwVscrollPos = 0;
                    dwCurrBar = 0;
                    flViewing = TRUE;
                    flSaved = TRUE;

                    /* Update viewer */
                    SetScrollPos(hwndViewer, SB_VERT, dwVscrollPos, TRUE);
                    SetScrollRange(hwndViewer, SB_VERT, 0, dwNumLines, FALSE);

                    InvalidateRect(hwndViewer, NULL, TRUE);

                    SetWindowText(hwndStatus, "VBR file loaded successfuly");
                    break;
                case IDM_STRREF:
                    /* String references search */
                    if (flViewing == FALSE) break;
                    DialogBox(hInst, MAKEINTRESOURCE(DLG_SEARCH), hDlg, DlgStrRef);
                    break;
                case IDM_TEXT:
                    /* Text search */
                    if (flViewing == FALSE) break;
                    DialogBox(hInst, MAKEINTRESOURCE(DLG_TEXT), hDlg, DlgText);
                    break;
                case IDM_IMPORT:
                    /* Import references search */
                    if (flViewing == FALSE) break;
                    DialogBox(hInst, MAKEINTRESOURCE(DLG_SEARCH), hDlg, DlgImportRef);
                    break;
                case IDM_EXPORT:
                    /* Export references search */
                    if (flViewing == FALSE) break;
                    DialogBox(hInst, MAKEINTRESOURCE(DLG_SEARCH), hDlg, DlgExportRef);
                    break;
                case IDM_GOTOEP:
                    /* Go to entry point */
                    if (flViewing == FALSE) break;
                    GotoEntryPoint();
                    break;
                case IDM_GOTOLOC:
                    /* Go to location */
                    if (flViewing == FALSE) break;
                    DialogBox(hInst, MAKEINTRESOURCE(DLG_GOTO), hDlg, DlgGotoLocation);
                    break;
                case IDM_ABOUT:
                    /* About VBRef */
                    DialogBox(hInst, MAKEINTRESOURCE(DLG_ABOUT), hDlg, DlgAbout);
                    break;
                case IDM_EXIT:
                    /* Exit VBRef */
                    PostMessage(hDlg, WM_CLOSE, 0, 0);
                    break;
                default:
                    break;
            }
            break;
        case WM_SIZE:
            /* Window resized */
            cyMain = HIWORD(lParam);
            cxMain = LOWORD(lParam);

            /* Resize Toolbar */
            GetWindowRect(hwndToolbar, &rectToolbar);
            MoveWindow(hwndToolbar, 0, 0, cxMain, rectToolbar.bottom - rectToolbar.top, TRUE);
            GetWindowRect(hwndToolbar, &rectToolbar);

            /* Resize Status bar */
            GetWindowRect(hwndStatus, &rectStatus);
            MoveWindow(hwndStatus, 0, cyMain - (rectStatus.bottom - rectStatus.top), cxMain, (rectStatus.bottom - rectStatus.top), TRUE);
            GetWindowRect(hwndStatus, &rectStatus);

            /* Resize viewer window */
            MoveWindow(hwndViewer, 0, rectToolbar.bottom - rectToolbar.top, cxMain, cyMain - ((rectToolbar.bottom - rectToolbar.top) + (rectStatus.bottom - rectStatus.top)), TRUE);

            /* Repaint */
            InvalidateRect(hDlg, NULL, TRUE);
            break;
        case WM_NOTIFY:
            /* Window notified by child windows */
            pnmh = (LPNMHDR )lParam;

            if (pnmh->code == TTN_NEEDTEXT) {
                /* Tooltip */
                lpttt = (LPTOOLTIPTEXT )lParam;
                switch (lpttt->hdr.idFrom) {
                    case IDM_OPENALF:
                        strcpy(lpttt->szText, "Open ALF");
                        break;
                    case IDM_SAVEVBR:
                        strcpy(lpttt->szText, "Save as VBR");
                        break;
                    case IDM_OPENVBR:
                        strcpy(lpttt->szText, "Open VBR");
                        break;
                    case IDM_STRREF:
                        strcpy(lpttt->szText, "String references search");
                        break;
                    case IDM_TEXT:
                        strcpy(lpttt->szText, "Text search");
                        break;
                    case IDM_IMPORT:
                        strcpy(lpttt->szText, "Import references search");
                        break;
                    case IDM_EXPORT:
                        strcpy(lpttt->szText, "Export references search");
                        break;
                    case IDM_GOTOEP:
                        strcpy(lpttt->szText, "Go to entry point");
                        break;
                    case IDM_GOTOLOC:
                        strcpy(lpttt->szText, "Go to location");
                        break;
                    default:
                        break;
                }
            }
            break;
        case WM_CLOSE:
            /* Close window */
            if (flSaved == FALSE) {
                /* User didn't save */
                switch (MessageBox(hDlg, "You haven\'t saved the new disassembly, would you like to save it?", "Save?", MB_YESNOCANCEL)) {
                    case IDNO:
                        /* Don't save */
                        remove(szNameVBR);
                        remove(szNameIDX);
                        break;
                    case IDYES:
                        /* Save then exit */
                        SendMessage(hDlg, WM_COMMAND, IDM_SAVEVBR, 0);
                        break;
                    case IDCANCEL:
                        /* Cancel exit */
                        return(FALSE);
                        break;
                    default:
                        break;
                }
            }

            /* Free previously allocated space */
            if (dwIndexOffset) {
                free(dwIndexOffset);
                dwIndexOffset = NULL;
            }

            /* Save settings */
            GetWindowRect(hwndMain, &rectMain);
            SetSetting("bottom", rectMain.bottom);
            SetSetting("top", rectMain.top);
            SetSetting("left", rectMain.left);
            SetSetting("right", rectMain.right);

            /* Deinitialize */
            DeleteObject(hFont);
            DestroyMenu(hMenu);
            EndDialog(hDlg, TRUE);
            break;
        default:
            break;
    }

    return(FALSE);
}

/* Change extension in a filename/path */
void ChangeExtension(char *szFilename, char *szExtension)
{
    if (strchr(szFilename,'.')) {
        strcpy(strchr(szFilename,'.') + 1, szExtension);
    } else {
        strcat(szFilename, ".");
        strcat(szFilename, szExtension);
    }
}

/* Get file length */
DWORD GetFileLen(FILE *filep)
{
    DWORD dwSave;
    DWORD dwLen;

    dwSave = ftell(filep);
    fseek(filep, 0L, SEEK_END);
    dwLen = ftell(filep);
    fseek(filep, dwSave, SEEK_SET);

    return(dwLen);
}

/* String reference search dialog */
BOOL CALLBACK DlgStrRef(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    long dwCount;
    static long dwLastIndex;
    static BOOL flContinue;
    long dwIndex;
    FILE *filep;
    char strDasm[512];

    switch (uMsg) {
        case WM_INITDIALOG:
            /* Dialog initialization */
            /* Add all StrRef's to list */
            for (dwCount = 0; dwCount < dwNumRefs; dwCount++)
                SendDlgItemMessage(hDlg, IDC_LB, LB_ADDSTRING, 0, (LPARAM )(LPCSTR )&szStrRef[dwCount]);

            flContinue = FALSE;
            dwLastIndex = dwIndex = 0;
            break;
        case WM_COMMAND:
            switch (wParam) {
                case IDC_CLOSE:
                    /* Close dialog */
                    PostMessage(hDlg, WM_CLOSE, 0, 0);
                    break;
                case IDC_FIND:
                    /* Find string reference */
                    dwIndex = SendDlgItemMessage(hDlg, IDC_LB, LB_GETCURSEL, 0, 0);

                    if (dwIndex != dwLastIndex)
                        flContinue = FALSE;

                    dwLastIndex = dwIndex;

                    /* Open disassembly */
                    filep = fopen(szNameVBR, "rb");

                    if (filep) {
                        if (flContinue == FALSE)
                            dwCurrBar = 0;

                        fseek(filep, dwIndexOffset[dwCurrBar + 1], SEEK_SET);

                        for (dwCount = dwCurrBar + 1; dwCount < dwNumLines; dwCount++) {
                            memset(strDasm, 0, sizeof(strDasm));
                            fgets(strDasm, sizeof(strDasm), filep);

                            if (strDasm[43] == 0) continue;

                            if (strncmp(&strDasm[43], szStrRef[dwIndex], strlen(szStrRef[dwIndex])) == 0) {
                                if (strDasm[43 + strlen(szStrRef[dwIndex])] == '"') {
                                    /* Found Reference */
                                    flContinue = TRUE;
                                    dwCurrBar = dwCount;
                                    dwVscrollPos = max(dwCurrBar - ((cyClient / cyChar) / 2), 0);
                                    SetScrollPos(hwndViewer, SB_VERT, dwVscrollPos, TRUE);
                                    InvalidateRect(hwndViewer, NULL, TRUE);
                                    break;
                                }
                            }
                        }

                        /* If end of disassembly, restart */
                        if (dwCount == dwNumLines)
                            flContinue = FALSE;

                        /* Close disassembly */
                        fclose(filep);
                    }
                    break;
                default:
                    break;
            }

            switch (HIWORD(wParam)) {
                case LBN_DBLCLK:
                    /* List box double clicked */
                    PostMessage(hDlg, WM_COMMAND, IDC_FIND, 0);
                    break;
                default:
                    break;
            }
            break;
        case WM_CLOSE:
            /* Close window */
            EndDialog(hDlg, TRUE);
            break;
        default:
            break;
    }

    return(FALSE);
}

/* Text search dialog */
BOOL CALLBACK DlgText(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    long dwCount;
    static long dwLastIndex;
    static BOOL flContinue;
    long dwIndex;
    FILE *filep;
    char strDasm[512];
    static char szText[512];
    static char szLastText[512];

    switch (uMsg) {
        case WM_INITDIALOG:
            /* Dialog initialization */
            flContinue = FALSE;
            dwLastIndex = dwIndex = 0;
            szLastText[0] = szText[0] = '\0';
            break;
        case WM_COMMAND:
            switch (wParam) {
                case IDC_CLOSE:
                    /* Close window */
                    PostMessage(hDlg, WM_CLOSE, 0, 0);
                    break;
                case IDC_FIND:
                    /* Find text in disassembly */
                    /* Open disassembly */
                    filep = fopen(szNameVBR, "rb");

                    if (filep) {
                        /* Get text to search */
                        GetDlgItemText(hDlg, IDT_TEXT, szText, sizeof(szText));

                        /* If new text, restart */
                        if (strcmp(szText, szLastText) != 0)
                            flContinue = FALSE;

                        strcpy(szLastText, szText);

                        if (flContinue == FALSE)
                            dwCurrBar = 0;

                        /* Search from last position */
                        fseek(filep, dwIndexOffset[dwCurrBar + 1], SEEK_SET);

                        /* Search text */
                        for (dwCount = dwCurrBar + 1; dwCount < dwNumLines; dwCount++) {
                            memset(strDasm, 0, sizeof(strDasm));
                            fgets(strDasm, sizeof(strDasm), filep);

                            if (strstr(strDasm, szText)) {
                                /* Found text */
                                flContinue = TRUE;
                                dwCurrBar = dwCount;
                                dwVscrollPos = max(dwCurrBar - ((cyClient / cyChar) / 2), 0);
                                SetScrollPos(hwndViewer, SB_VERT, dwVscrollPos, TRUE);
                                InvalidateRect(hwndViewer, NULL, TRUE);
                                break;
                            }
                        }

                        /* If end of disassembly, restart */
                        if (dwCount == dwNumLines)
                            flContinue = FALSE;

                        /* Close disassembly */
                        fclose(filep);
                    }
                    break;
                default:
                    break;
            }
            break;
        case WM_CLOSE:
            /* Close window */
            EndDialog(hDlg, TRUE);
            break;
        default:
            break;
    }

    return(FALSE);
}

/* Import reference search dialog */
BOOL CALLBACK DlgImportRef(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    long dwCount;
    static long dwLastIndex;
    static BOOL flContinue;
    long dwIndex;
    FILE *filep;
    char strDasm[512];

    switch (uMsg) {
        case WM_INITDIALOG:
            /* Dialog initialization */
            /* Add all Imports to list */
            for (dwCount = 0; dwCount < dwNumImpRefs; dwCount++)
                SendDlgItemMessage(hDlg, IDC_LB, LB_ADDSTRING, 0, (LPARAM )(LPCSTR )&szImportRef[dwCount]);

            flContinue = FALSE;
            dwLastIndex = dwIndex = 0;
            break;
        case WM_COMMAND:
            switch (wParam) {
                case IDC_CLOSE:
                    /* Close window */
                    PostMessage(hDlg, WM_CLOSE, 0, 0);
                    break;
                case IDC_FIND:
                    /* Find import reference */
                    dwIndex = SendDlgItemMessage(hDlg, IDC_LB, LB_GETCURSEL, 0, 0);

                    if (dwIndex != dwLastIndex)
                        flContinue = FALSE;

                    dwLastIndex = dwIndex;

                    /* Open disassembly */
                    filep = fopen(szNameVBR, "rb");

                    if (filep) {
                        if (flContinue == FALSE)
                            dwCurrBar = 0;

                        fseek(filep, dwIndexOffset[dwCurrBar + 1], SEEK_SET);

                        for (dwCount = dwCurrBar + 1; dwCount < dwNumLines; dwCount++) {
                            memset(strDasm, 0, sizeof(strDasm));
                            fgets(strDasm, sizeof(strDasm), filep);

                            if (strDasm[16] == 0) continue;

                            if (strncmp(&strDasm[16], szImportRef[dwIndex], strlen(szImportRef[dwIndex])) == 0) {
                                if (strDasm[16 + strlen(szImportRef[dwIndex])] == ',') {
                                    /* Found Import Reference */
                                    flContinue = TRUE;
                                    dwCurrBar = dwCount;
                                    dwVscrollPos = max(dwCurrBar - ((cyClient / cyChar) / 2), 0);
                                    SetScrollPos(hwndViewer, SB_VERT, dwVscrollPos, TRUE);
                                    InvalidateRect(hwndViewer, NULL, TRUE);
                                    break;
                                }
                            }
                        }

                        /* If end of disassembly, restart */
                        if (dwCount == dwNumLines)
                            flContinue = FALSE;

                        /* Close disassembly */
                        fclose(filep);
                    }
                    break;
                default:
                    break;
            }

            switch (HIWORD(wParam)) {
                case LBN_DBLCLK:
                    /* List box double clicked */
                    PostMessage(hDlg, WM_COMMAND, IDC_FIND, 0);
                    break;
                default:
                    break;
            }
            break;
        case WM_CLOSE:
            /* Close window */
            EndDialog(hDlg, TRUE);
            break;
        default:
            break;
    }

    return(FALSE);
}

/* Export reference search */
BOOL CALLBACK DlgExportRef(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    long dwCount;
    static long dwLastIndex;
    static BOOL flContinue;
    long dwIndex;
    FILE *filep;
    char strDasm[512];

    switch (uMsg) {
        case WM_INITDIALOG:
            /* Dialog initialization */
            /* Add all Imports to list */
            for (dwCount = 0; dwCount < dwNumExpRefs; dwCount++)
                SendDlgItemMessage(hDlg, IDC_LB, LB_ADDSTRING, 0, (LPARAM )(LPCSTR )&szExportRef[dwCount]);

            flContinue = FALSE;
            dwLastIndex = dwIndex = 0;
            break;
        case WM_COMMAND:
            switch (wParam) {
                case IDC_CLOSE:
                    /* Close window */
                    PostMessage(hDlg, WM_CLOSE, 0, 0);
                    break;
                case IDC_FIND:
                    /* Find export reference */
                    dwIndex = SendDlgItemMessage(hDlg, IDC_LB, LB_GETCURSEL, 0, 0);

                    if (dwIndex != dwLastIndex)
                        flContinue = FALSE;

                    dwLastIndex = dwIndex;

                    /* Open disassembly */
                    filep = fopen(szNameVBR, "rb");

                    if (filep) {
                        if (flContinue == FALSE)
                            dwCurrBar = 0;

                        fseek(filep, dwIndexOffset[dwCurrBar + 1], SEEK_SET);

                        /* Search export reference */
                        for (dwCount = dwCurrBar + 1; dwCount < dwNumLines; dwCount++) {
                            memset(strDasm, 0, sizeof(strDasm));
                            fgets(strDasm, sizeof(strDasm), filep);

                            if (strDasm[15] == 0) continue;

                            if (strncmp(&strDasm[15], szExportRef[dwIndex], strlen(szExportRef[dwIndex])) == 0) {
                                if (strDasm[15 + strlen(szExportRef[dwIndex])] == ' ') {
                                    /* Found Export Reference */
                                    flContinue = TRUE;
                                    dwCurrBar = dwCount;
                                    dwVscrollPos = max(dwCurrBar - ((cyClient / cyChar) / 2), 0);
                                    SetScrollPos(hwndViewer, SB_VERT, dwVscrollPos, TRUE);
                                    InvalidateRect(hwndViewer, NULL, TRUE);
                                    break;
                                }
                            }
                        }

                        /* If end of disassembly, restart */
                        if (dwCount == dwNumLines)
                            flContinue = FALSE;

                        /* Close disassembly */
                        fclose(filep);
                    }
                    break;
                default:
                    break;
            }

            switch (HIWORD(wParam)) {
                case LBN_DBLCLK:
                    /* List box double clicked */
                    PostMessage(hDlg, WM_COMMAND, IDC_FIND, 0);
                    break;
                default:
                    break;
            }
            break;
        case WM_CLOSE:
            /* Close dialog */
            EndDialog(hDlg, TRUE);
            break;
        default:
            break;
    }

    return(FALSE);
}

/* Go to location dialog */
BOOL CALLBACK DlgGotoLocation(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    char szLoc[128];
    BOOL flInvalid;
    char strDasm[512];
    FILE *filep;
    DWORD dwCount;

    switch (uMsg) {
        case WM_INITDIALOG:
            /* Dialog initialization */
            break;
        case WM_CLOSE:
            /* Close dialog */
            EndDialog(hDlg, TRUE);
            break;
        case WM_COMMAND:
            switch (wParam) {
                case IDC_GOTO:
                    /* Go to position */
                    GetDlgItemText(hDlg, IDT_OFFSET, szLoc, sizeof(szLoc));

                    flInvalid = FALSE;

                    /* Check if valid offset */
                    for (dwCount = 0; szLoc[dwCount]; dwCount++) {
                        if (szLoc[dwCount] < '0') flInvalid = TRUE;
                        if (szLoc[dwCount] > 'F') flInvalid = TRUE;
                        if (szLoc[dwCount] < 'A' && szLoc[dwCount] > '9') flInvalid = TRUE;
                    }

                    if (strlen(szLoc) > 8 || strlen(szLoc) == 0) flInvalid = TRUE;

                    if (flInvalid == TRUE) {
                        /* Offset invalid */
                        MessageBox(hDlg, "Please enter a valid hexadecimal number", "Error", MB_OK);
                        break;
                    }

                    dwCount = strtol(szLoc, NULL, 16);
                    sprintf(szLoc, "%08lX", dwCount);

                    /* Open disassembly */
                    filep = fopen(szNameVBR, "rb");

                    if (filep) {
                        /* Search offset */
                        for (dwCount = 0; dwCount < dwNumLines; dwCount++) {
                            memset(strDasm, 0, sizeof(strDasm));
                            fgets(strDasm, sizeof(strDasm), filep);

                            if (strDasm[0] == ':') {
                                if (strncmp(&strDasm[1], szLoc, strlen(szLoc)) == 0) {
                                    /* Found offset */
                                    dwCurrBar = dwCount;
                                    dwVscrollPos = max(dwCurrBar - ((cyClient / cyChar) / 2), 0);
                                    SetScrollPos(hwndViewer, SB_VERT, dwVscrollPos, TRUE);
                                    InvalidateRect(hwndViewer, NULL, TRUE);
                                    break;
                                }
                            }
                        }

                        /* Close disassembly */
                        fclose(filep);
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return(FALSE);
}

/* Go to entry point */
void GotoEntryPoint(void)
{
    char szLoc[128];
    char strDasm[512];
    FILE *filep;
    DWORD dwCount;

    sprintf(szLoc, "%08lX", dwEntryPoint);

    /* Open disassembly */
    filep = fopen(szNameVBR, "rb");

    if (filep) {
        /* Search entry point */
        for (dwCount = 0; dwCount < dwNumLines; dwCount++) {
            memset(strDasm, 0, sizeof(strDasm));
            fgets(strDasm, sizeof(strDasm), filep);

            if (strDasm[0] == ':') {
                if (strncmp(&strDasm[1], szLoc, strlen(szLoc)) == 0) {
                    /* Found entry point */
                    dwCurrBar = dwCount;
                    dwVscrollPos = max(dwCurrBar - ((cyClient / cyChar) / 2), 0);
                    SetScrollPos(hwndViewer, SB_VERT, dwVscrollPos, TRUE);
                    InvalidateRect(hwndViewer, NULL, TRUE);
                    break;
                }
            }
        }

        /* Close disassembly */
        fclose(filep);
    }
}

/* About dialog */
BOOL CALLBACK DlgAbout(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HICON hIcon;

    switch (uMsg) {
        case WM_INITDIALOG:
            /* Dialog initialization */
            hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_EXEC));
            SendDlgItemMessage(hDlg, IDC_ABOUT, STM_SETICON, (WPARAM )hIcon, 0);
            break;
        case WM_CLOSE:
            /* Close dialog */
            EndDialog(hDlg, TRUE);
            break;
        case WM_COMMAND:
            switch (wParam) {
                case IDOK:
                    /* Close dialog */
                    PostMessage(hDlg, WM_CLOSE, 0, 0);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return(FALSE);
}

/* Viewer window procedure */
LRESULT CALLBACK ViewerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    long y;
    FILE *filep;
    DWORD dwCount;
    char strDasm[512];
    DWORD dwTemp;
    TEXTMETRIC tm;

    switch (uMsg) {
        case WM_CREATE:
            /* Initialize viewer variables */
            hdc = GetDC(hwnd);

            SelectObject(hdc, hFont);

            GetTextMetrics(hdc, &tm);
            cxChar = tm.tmAveCharWidth;
            cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
            cyChar = tm.tmHeight + tm.tmExternalLeading;

            ReleaseDC(hwnd, hdc);
            break;
        case WM_VSCROLL:
            /* Scrollbar operation */
            if (flViewing == FALSE) break;

            switch (LOWORD(wParam)) {
                case SB_LINEUP:
                    /* One line up */
                    dwVscrollPos--;
                    break;
                case SB_LINEDOWN:
                    /* One line down */
                    dwVscrollPos++;
                    break;
                case SB_PAGEUP:
                    /* One page up */
                    dwVscrollPos -= cyClient / cyChar + 2;
                    break;
                case SB_PAGEDOWN:
                    /* One page down */
                    dwVscrollPos += cyClient / cyChar - 2;
                    break;
                case SB_THUMBPOSITION:
                    /* Thumb moved */
                    dwVscrollPos = HIWORD(wParam);
                    break;
                default:
                    break;
            }

            /* Get real scroller position */
            dwVscrollPos = max(0, min(dwVscrollPos, dwNumLines));

            if (dwVscrollPos != GetScrollPos(hwndViewer, SB_VERT)) {
                /* Valid scroll position */
                SetScrollPos(hwndViewer, SB_VERT, dwVscrollPos, TRUE);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        case WM_SIZE:
            /* Window resized */
            cyClient = HIWORD(lParam);
            cxClient = LOWORD(lParam);

            InvalidateRect(hwnd, NULL, TRUE);
            break;
        case WM_LBUTTONDBLCLK:
            /* Left button double clicked */
            if (flViewing == FALSE) break;

            if ((HIWORD(lParam) / cyChar - 1 + dwVscrollPos) >= dwVscrollPos) {
                if ((HIWORD(lParam) / cyChar - 1 + dwVscrollPos) <= min(dwVscrollPos + (cyClient / cyChar) - 1, dwNumLines)) {
                    /* Change hilighted text */
                    dwCurrBar = HIWORD(lParam) / cyChar - 1 + dwVscrollPos;
                    InvalidateRect(hwndViewer, NULL, TRUE);
                }
            }
            break;
        case WM_PAINT:
            /* Window repaint */
            if (flViewing == FALSE) break;

            hdc = BeginPaint(hwnd, &ps);

            /* Font selection */
            SelectObject(hdc, hFont);

            /* Transparent background mode (light blue) */
            SetBkColor(hdc, RGB(128, 128, 255));
            SetBkMode(hdc, TRANSPARENT);

            /* Open disassembly */
            filep = fopen(szNameVBR, "rb");

            if (filep != NULL) {
                fseek(filep, dwIndexOffset[dwVscrollPos], SEEK_SET);

                /* Get the lines that need to be viewed */
                for (dwCount = dwVscrollPos; dwCount < min(dwVscrollPos + (cyClient / cyChar) - 1, dwNumLines); dwCount++) {
                    y = cyChar * (1 - dwVscrollPos + dwCount);

                    fgets(strDasm, sizeof(strDasm), filep);

                    if (strchr(strDasm, '\r')) *strchr(strDasm, '\r') = '\0';
                    if (strchr(strDasm, '\n')) *strchr(strDasm, '\n') = '\0';

                    if (dwCount == dwCurrBar) {
                        /* Hilighted line */
                        for (dwTemp = strlen(strDasm); dwTemp < (cxClient / cxChar); dwTemp++)
                            strDasm[dwTemp] = ' ';
                        strDasm[dwTemp] = '\0';

                        SetBkMode(hdc, OPAQUE);
                        TextOut(hdc, cxChar, y, strDasm, strlen(strDasm));
                        SetBkMode(hdc, TRANSPARENT);
                    } else {
                        /* Regular line */
                        TextOut(hdc, cxChar, y, strDasm, strlen(strDasm));
                    }
                }

                memset(strDasm, 0, sizeof(strDasm));

                /* Close disassembly */
                fclose(filep);
            }

            EndPaint(hwnd, &ps);
            break;
        default:
            break;
    }

    return(DefWindowProc(hwnd, uMsg, wParam, lParam));
}

/* Reconstruct disassembly text */
void Reconstruct_Disassembly(void)
{
    HANDLE hDasmIn;
    HANDLE hDasmOut;
    HANDLE hInMap;
    HANDLE hOutMap;
    char *pInMap;
    char *pOutMap;
    char *pInFile;
    char *pOutFile;
    DWORD dwFileSize;
    short iLastPercent;
    long dwCount;
    char strDasm[512];

    /* Open input file */
    hDasmIn = CreateFile(szNameALF, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hDasmIn == INVALID_HANDLE_VALUE)
        return;

    /* Create temporary output file */
    tmpnam(szNameVBR);
    hDasmOut = CreateFile(szNameVBR, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

    /* Get length */
    dwFileSize = GetFileSize(hDasmIn, NULL);
    iLastPercent = 0;

    /* Set file mapping */
    hInMap = CreateFileMapping(hDasmIn, NULL, PAGE_READONLY, 0, dwFileSize, NULL);
    pInMap = MapViewOfFile(hInMap, FILE_MAP_READ, 0, 0, 0);
    pInFile = pInMap;

    hOutMap = CreateFileMapping(hDasmOut, NULL, PAGE_READWRITE, 0, dwFileSize * 2, NULL);
    pOutMap = MapViewOfFile(hOutMap, FILE_MAP_WRITE, 0, 0, 0);
    pOutFile = pOutMap;

    /* Clear string buffer */
    memset(strDasm, '\0', sizeof(strDasm));

    /* Reconstructing loop */
    while (1) {
        if (pInFile >= (pInMap + dwFileSize)) break;

        /* Get line */
        pInFile = GetLine(pInFile, pInMap, strDasm, dwFileSize);

        for (dwCount = 0; dwCount < strref_counter; dwCount++) {
            if (strstr(&strDasm[9], vstrref[dwCount].strrva)) {
                pOutFile += sprintf(pOutFile, "\n"
                    "* Possible StringData Ref from Code Obj ->\"%s\"\n"
                    "                                  |\n", vstrref[dwCount].wcString);
                dwNumLines += 2;
            }
        }

        pOutFile += sprintf(pOutFile, "%s\n", strDasm);
        dwNumLines++;

        /* String Data Reference found? Make sure we don't */
        /* Add out SDR.. */
        if (strncmp(strDasm, cszSDRID, strlen(cszSDRID)) == 0) {
            BOOL bExitFlag;

            bExitFlag = FALSE;

            memset(strDasm, 0, sizeof(strDasm));

            while (bExitFlag == FALSE) {
                pInFile = GetLine(pInFile, pInMap, strDasm, dwFileSize);

                pOutFile += sprintf(pOutFile, "%s\n", strDasm);
                dwNumLines++;

                if (strcmp(strDasm, cszENDID) == 0)
                    bExitFlag = TRUE;

                memset(strDasm, 0, sizeof(strDasm));
            }

            pInFile = GetLine(pInFile, pInMap, strDasm, dwFileSize);

            pOutFile += sprintf(pOutFile, "%s\n", strDasm);
            dwNumLines++;
        }

        /* Update status */
        if ((((pInFile - pInMap) << 5) / dwFileSize) != iLastPercent) {
            iLastPercent = ((pInFile - pInMap) << 5) / dwFileSize;
            sprintf(strDasm, "Reconstructing disassembly text file (%%%d)...", iLastPercent * 100 / 32);
            SetWindowText(hwndStatus, strDasm);
        }

        memset(strDasm, 0, sizeof(strDasm));
    }

    /* Get output file length */
    dwFileSize = pOutFile - pOutMap;

    /* Unmap files */
    UnmapViewOfFile(pInMap);
    UnmapViewOfFile(pOutMap);
    CloseHandle(hInMap);
    CloseHandle(hOutMap);

    /* Close input file */
    CloseHandle(hDasmIn);

    /* Trim and close output file */
    SetFilePointer(hDasmOut, dwFileSize, NULL, FILE_BEGIN);
    SetEndOfFile(hDasmOut);
    CloseHandle(hDasmOut);
}

/* Get a line from a mapped file */
char *GetLine(char *pFile, char *pMap, char *szStr, DWORD dwFileSize)
{
    DWORD dwCount;

    dwCount = 0L;

    if (pFile >= (pMap + dwFileSize)) return(pFile);

    while (*pFile != '\n' && (pFile < (pMap + dwFileSize))) {
        if (*pFile != '\r')
            szStr[dwCount] = *pFile;
        pFile++;
        dwCount++;
    }

    if (pFile < (pMap + dwFileSize)) {

        if (*pFile == '\n') pFile++;

    }

    return(pFile);
}
