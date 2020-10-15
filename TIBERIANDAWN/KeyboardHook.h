#ifndef KEYBOARDHOOK_H
#define KEYBOARDHOOK_H

class KeyboardHook
{
private:
    HINSTANCE _hHookInstance;
    InstallHookProc _installHookProc;
    UninstallHookProc _uninstallHookProc;
    GetKeyDataProc _getKeyDataProc;

    HANDLE _hookThreadHandle;
    DWORD _hookThreadId;

    HANDLE _exitEvent;

public:
    KeyboardHook(void);
    ~KeyboardHook();

    BOOL InstallHook(void);
    BOOL UninstallHook(void);

private:
    BOOL LoadHookDll(LPCHOOKCONFIGURATION lpHookConfiguration);
    BOOL UnloadHookDll(void);

    void StopHookMessageThread(void);

    static DWORD WINAPI HookMessageThread(LPVOID lpParameter);
};

#endif
