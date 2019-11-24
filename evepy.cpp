#include <windows.h>
#include <stdio.h>

#pragma comment (lib, "advapi32")

//typedef int (__stdcall *PYRUN_SIMPLEFILE)(FILE *fp, const char *str);
typedef int (__stdcall *PYRUN_SIMPLESTRING)(const char *str);
typedef void (__stdcall *PY_INITIALIZE)();

//PYRUN_SIMPLEFILE PyRun_SimpleFile;
PYRUN_SIMPLESTRING PyRun_SimpleString;
PY_INITIALIZE Py_Initialize;
wchar_t **Py_GetPathWData;

HMODULE hModule;

BOOL getEvePath(char *path, DWORD len)
{
      HKEY hKey;
      LONG res;
      DWORD type = REG_SZ;
      
      // for steamed versoin on win64
      res=RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Steam App 8500",0,KEY_READ | KEY_WOW64_64KEY,&hKey);
      if(res!=ERROR_SUCCESS) return FALSE;
      
      res=RegQueryValueEx(hKey,"InstallLocation", NULL, &type, (LPBYTE)path, &len);
      RegCloseKey(hKey);
      
      if(res!=ERROR_SUCCESS) return FALSE;
      return TRUE;
}

int main(int argc, char **argv)
{
    char buf[100000];
    wchar_t buf2[10000];
    char path[MAX_PATH];
    
    // get EVE path
    if(getEvePath(path, MAX_PATH))
    {
        strcat(path, "\\SharedCache\\tq");
        printf("EVE dir: %s\n", path);
    }
    else
    {
        printf("Failed to find eve path :(\n");
        return 1;
    }
    
    // get python
    sprintf(buf, "%s\\bin64\\python27.dll", path);
    hModule = LoadLibrary(buf);
    //PyRun_SimpleFile    = (PYRUN_SIMPLEFILE)    GetProcAddress(hModule, "PyRun_SimpleFile");
    PyRun_SimpleString  = (PYRUN_SIMPLESTRING)  GetProcAddress(hModule, "PyRun_SimpleString");
    Py_Initialize       = (PY_INITIALIZE)       GetProcAddress(hModule, "Py_Initialize");
    Py_GetPathWData     = (wchar_t**)           GetProcAddress(hModule, "Py_GetPathWData");
    
    // MUST be done before init
    swprintf(buf2, 10000, L"%hs\\code.ccp;%hs\\bin64", path, path);
    wprintf(L"%s\n", buf2);
    *Py_GetPathWData = buf2;
    
    // display debug info
    //printf("PyRun_SimpleFile:   %X\n", (unsigned int)PyRun_SimpleFile);
    printf("Py_GetPathWData:         %p\n", (void *)Py_GetPathWData);
    wprintf(L"Py_GetPathWData:         %s\n", *Py_GetPathWData);
    printf("PyRun_SimpleString: %p\n", (void *)PyRun_SimpleString);
    printf("Py_Initialize:      %p\n", (void *)Py_Initialize);
    printf("\n");
    
    
    Py_Initialize();
    
    if (argc > 1)
    {
        FILE *fp = fopen(argv[1], "r");
        
        // SimpleFile emulator
        int len = fread(buf, 1, 100000, fp);
        buf[len] = 0;
        fclose(fp);
        
        PyRun_SimpleString(buf);
    }
    else
    {
        PyRun_SimpleString("import sys, code");
        PyRun_SimpleString("code.interact()");
    }
    
    return 0;
}
