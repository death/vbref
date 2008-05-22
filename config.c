/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
/* Visual Basic Disassembled String Reference Analyser        (c) DEATH  */
/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
/* config.c                                                              */
/*                                                                       */
/* Configuration handling functions                                      */
/* ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ */
#include <windows.h>

#include "include/config.h"

static const char cszRegKey[] = "Software\\Execution\\VBRef";

/* Set a setting */
void SetSetting(char *szName, DWORD dwValue)
{
    HKEY hkey;
    LONG lRet;

    lRet = RegCreateKey(HKEY_CURRENT_USER, cszRegKey, &hkey);

    if (lRet == ERROR_SUCCESS) {
        RegSetValueEx(hkey, szName, 0, REG_DWORD, (LPBYTE )&dwValue, sizeof(DWORD));
        RegCloseKey(hkey);
    }
}

/* Get a setting */
BOOL GetSetting(char *szName, DWORD *dwValue)
{
    HKEY hkey;
    LONG lRet;
    DWORD dwType;
    DWORD dwSize;

    lRet = RegOpenKeyEx(HKEY_CURRENT_USER, cszRegKey, 0, KEY_ALL_ACCESS, &hkey);

    if (lRet == ERROR_SUCCESS) {
        dwSize = sizeof(DWORD);
        lRet = RegQueryValueEx(hkey, szName, NULL, &dwType, (LPBYTE )dwValue, &dwSize);

        RegCloseKey(hkey);

        if (lRet == ERROR_SUCCESS)
            return(TRUE);
    }

    return(FALSE);
}
