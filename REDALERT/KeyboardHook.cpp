#include <Shlwapi.h>
#include "function.h"

//
// KeyboardHook
//

KeyboardHook::KeyboardHook(void)
{
    _hHookInstance = NULL;
    
    _installHookProc = NULL;
    _uninstallHookProc = NULL;
    _getKeyDataProc = NULL;

    _hookThreadHandle = NULL;
    _hookThreadId = 0;

    _exitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

KeyboardHook::~KeyboardHook()
{
    UninstallHook();

    CloseHandle(_exitEvent);
}

BOOL KeyboardHook::InstallHook(void)
{
    if (!_hookThreadHandle)
    {
        ResetEvent(_exitEvent);

        _hookThreadHandle = CreateThread(NULL, 0, HookMessageThread, this, 0, &_hookThreadId);
        if (_hookThreadHandle)
        {
            ModState::SetHookMessageThreadId(_hookThreadId);

            if (!_hHookInstance && (LoadHookDll(ModState::GetHookConfiguration()) == FALSE))
            {
                StopHookMessageThread();
            }
        }

        return !!_hookThreadHandle;
    }

    return FALSE;
}

BOOL KeyboardHook::UninstallHook(void)
{
    if (_hookThreadHandle)
    {
        StopHookMessageThread();
        UnloadHookDll();

        return TRUE;
    }

    return FALSE;
}

BOOL KeyboardHook::LoadHookDll(LPCHOOKCONFIGURATION lpHookConfiguration)
{
    char szModulePath[MAX_PATH];
    char szHookPath[MAX_PATH];

    HMODULE hm = NULL;

    if (GetModuleHandleEx(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR)&HookMessageThread,
        &hm) == 0)
    {
        return FALSE;
    }

    if (GetModuleFileName(hm, szModulePath, sizeof(szModulePath)) == 0)
    {
        return FALSE;
    }

    PathRemoveFileSpec(szModulePath);
    PathCombine(szHookPath, szModulePath, "KeyboardHook.dll");

    _hHookInstance = ::LoadLibrary(szHookPath);

    if (_hHookInstance == NULL)
    {
        return FALSE;
    }

    _installHookProc = (InstallHookProc) ::GetProcAddress(_hHookInstance, "InstallHook");
    _uninstallHookProc = (UninstallHookProc) ::GetProcAddress(_hHookInstance, "UninstallHook");
    _getKeyDataProc = (GetKeyDataProc) ::GetProcAddress(_hHookInstance, "GetKeyData");

    if ((_installHookProc == NULL) || (_uninstallHookProc == NULL) || (_getKeyDataProc == NULL))
    {
        ::FreeLibrary(_hHookInstance);
        _hHookInstance = NULL;

        return FALSE;
    }

    return _installHookProc(lpHookConfiguration);
}

BOOL KeyboardHook::UnloadHookDll(void)
{
    if (_hHookInstance != NULL)
    {
        if (_uninstallHookProc)
        {
            _uninstallHookProc();
        }

        ::FreeLibrary(_hHookInstance);
        _hHookInstance = NULL;

        return TRUE;
    }

    return FALSE;
}

void KeyboardHook::StopHookMessageThread(void)
{
    if (_hookThreadHandle)
    {
        PostThreadMessage(_hookThreadId, WM_USER, 0, NULL);
        WaitForSingleObject(_exitEvent, INFINITE);

        CloseHandle(_hookThreadHandle);
        _hookThreadHandle = NULL;
    }
}

DWORD WINAPI KeyboardHook::HookMessageThread(LPVOID lpParameter)
{
    KeyboardHook* pThis = (KeyboardHook*)lpParameter;

    static char buffer[256] = { 0 };

    bool bContinue = true;

    MSG msg;
    BOOL bRet;

    while (bContinue && ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0))
    {
        if (bRet == -1)
        {
            break;
        }
        else
        {
            bool mode = false;
            HookKeyData hkdData = { 0 };

            switch (msg.message)
            {
            case WM_USER:
                bContinue = false;
                break;

            case WM_USER + 1:
                ModState::TriggerNeedShowHelp();
                break;

            case WM_USER + 2:
                mode = ModState::ToggleNoDamage();
                sprintf_s(buffer, "No damage mode: %s", mode ? "enabled" : "disabled");
                ModState::AddModMessage(buffer);
                break;

            case WM_USER + 3:
                mode = ModState::ToggleUnlockBuildOptions();
                sprintf_s(buffer, "Unlock build mode: %s", mode ? "enabled" : "disabled");
                ModState::AddModMessage(buffer);
                break;

            case WM_USER + 4:
                mode = ModState::ToggleInstantBuild();
                sprintf_s(buffer, "Instant build mode: %s", mode ? "enabled" : "disabled");
                ModState::AddModMessage(buffer);
                break;

            case WM_USER + 5:
                mode = ModState::ToggleInstantSuperweapons();
                sprintf_s(buffer, "Instant superweapons mode: %s", mode ? "enabled" : "disabled");
                ModState::AddModMessage(buffer);
                break;

            case WM_USER + 6:
                mode = ModState::ToggleDismissShroud();
                sprintf_s(buffer, "Dismiss shroud mode: %s", mode ? "enabled" : "disabled");
                ModState::AddModMessage(buffer);
                break;

            case WM_USER + 7:
                mode = ModState::ToggleUnlimitedAmmo();
                sprintf_s(buffer, "Unlimited ammo for aircrafts: %s", mode ? "enabled" : "disabled");
                ModState::AddModMessage(buffer);
                break;

            case WM_USER + 8:
                ModState::IncreaseCreditBoost();
                sprintf_s(buffer, "Credits boosted");
                ModState::AddModMessage(buffer);
                break;

            case WM_USER + 9:
                ModState::IncreasePowerBoost();
                sprintf_s(buffer, "Power boosted");
                ModState::AddModMessage(buffer);
                break;

            case WM_USER + 10:
                ModState::IncreaseMovementBoost();
                sprintf_s(buffer, "Movement boost: %.0f%%", ModState::GetMovementBoost() * 100.0f);
                ModState::AddModMessage(buffer);
                break;

            case WM_USER + 11:
                ModState::DecreaseMovementBoost();
                sprintf_s(buffer, "Movement boost: %.0f%%", ModState::GetMovementBoost() * 100.0f);
                ModState::AddModMessage(buffer);
                break;

            case WM_USER + 12:
                ModState::IncreaseResourceGrowthMultiplier();
                sprintf_s(buffer, "Tiberium growth multiplier: %d", ModState::GetResourceGrowthMultiplier());
                ModState::AddModMessage(buffer);
                break;

            case WM_USER + 13:
                ModState::DecreaseResourceGrowthMultiplier();
                sprintf_s(buffer, "Tiberium growth multiplier: %d", ModState::GetResourceGrowthMultiplier());
                ModState::AddModMessage(buffer);
                break;

            case WM_USER + 14:
                ModState::IncreaseHarvesterBoost();
                sprintf_s(buffer, "Harvester load: %.0f%% of normal", ModState::GetHarvesterBoost() * 100.0f);
                ModState::AddModMessage(buffer);
                break;

            case WM_USER + 15:
                ModState::DecreaseHarvesterBoost();
                sprintf_s(buffer, "Harvester load: %.0f%% of normal", ModState::GetHarvesterBoost() * 100.0f);
                ModState::AddModMessage(buffer);
                break;

            case WM_USER + 16:
                ModState::TriggerNeedSaveSettings();
                sprintf_s(buffer, "Saving current settings");
                ModState::AddModMessage(buffer);
                break;

            case WM_USER + 17:
                if ((pThis->_getKeyDataProc != NULL) && (pThis->_getKeyDataProc(msg.wParam, &hkdData) == TRUE))
                {
                    ModState::SetSpawnInfantryFromKeyData(hkdData);
                }
                break;

            case WM_USER + 18:
                if ((pThis->_getKeyDataProc != NULL) && (pThis->_getKeyDataProc(msg.wParam, &hkdData) == TRUE))
                {
                    ModState::SetSpawnUnitFromKeyData(hkdData);
                }
                break;

            case WM_USER + 19:
                if ((pThis->_getKeyDataProc != NULL) && (pThis->_getKeyDataProc(msg.wParam, &hkdData) == TRUE))
                {
                    ModState::SetSpawnAircraftFromKeyData(hkdData);
                }
                break;

            case WM_USER + 20:
                if ((pThis->_getKeyDataProc != NULL) && (pThis->_getKeyDataProc(msg.wParam, &hkdData) == TRUE))
                {
                    ModState::SetCaptureHouseFromKeyData(hkdData);
                }
                break;

            case WM_USER + 21:
                ModState::TriggerNeedResetSettingsToDefault();
                break;

            case WM_USER + 22:
                if ((pThis->_getKeyDataProc != NULL) && (pThis->_getKeyDataProc(msg.wParam, &hkdData) == TRUE))
                {
                    ModState::SetSpawnVesselFromKeyData(hkdData);
                }
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!bContinue)
        {
            break;
        }
    }

    // Flush the message queue.
    while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
    {
        if ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
        {
            if (bRet == -1)
            {
                break;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    SetEvent(pThis->_exitEvent);

    return 0;
}