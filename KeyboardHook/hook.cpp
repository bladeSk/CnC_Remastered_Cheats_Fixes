#include "pch.h"

#include "hook.h"
#include "hookdata.h"

#pragma data_seg("shared")

HHOOK _hookHandle = NULL;

BYTE _rawHookData[sizeof(HookData)] = { 0 };

#pragma data_seg()
#pragma comment(linker, "/section:shared,RWS")

extern HINSTANCE _dllInstance;

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
HookData* GetHookData(void);

bool _isKeyDown[256];

extern "C" __declspec(dllexport) BOOL __cdecl InstallHook(LPHOOKCONFIGURATION lpHookConfiguration)
{
    if (lpHookConfiguration == NULL)
    {
        return FALSE;
    }

    if (_hookHandle != NULL)
    {
        return FALSE;
    }

    HookData* pHookData = GetHookData();

    pHookData->Initialize();
    pHookData->SetConfiguration(lpHookConfiguration);

    memset(_isKeyDown, 0, sizeof(_isKeyDown));

    HWND gameHwnd = ::FindWindow("PGCLIENTWINCLASS", NULL);
    if (gameHwnd == NULL)
    {
        return FALSE;
    }

    DWORD gameThreadId = ::GetWindowThreadProcessId(gameHwnd, NULL);

    _hookHandle = ::SetWindowsHookEx(WH_KEYBOARD, KeyboardHookProc, _dllInstance, gameThreadId);
    if (_hookHandle == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

extern "C" __declspec(dllexport) BOOL __cdecl UninstallHook(void)
{
    if ((_hookHandle != NULL) && (::UnhookWindowsHookEx(_hookHandle) != FALSE))
    {
        _hookHandle = NULL;
        return TRUE;
    }

    return FALSE;
}

extern "C" __declspec(dllexport) BOOL __cdecl GetKeyData(WPARAM index, LPHOOKKEYDATA lpKeyData)
{
    if (GetHookData()->CopyTo((int)index, lpKeyData))
    {
        return TRUE;
    }

    return FALSE;
}


#pragma optimize("", off)

HookData* GetHookData(void)
{
    return (HookData*)(_rawHookData);
}

DWORD GetKeyCodeWithCurrentState(WPARAM vkKey)
{
    bool isAlt = GetAsyncKeyState(VK_MENU) & 0x8000;
    bool isCtrl = GetAsyncKeyState(VK_CONTROL) & 0x8000;
    bool isShift = GetAsyncKeyState(VK_SHIFT) & 0x8000;

    return vkKey | (isAlt ? KEYSTATE_ALT : 0) | (isCtrl ? KEYSTATE_CTRL : 0) | (isShift ? KEYSTATE_SHIFT : 0);
}

bool IsKeyActive(DWORD dwKey, UINT action)
{
    if (action == 0)
    {
        if (!_isKeyDown[dwKey])
        {
            _isKeyDown[dwKey] = true;
            return true;
        }

        return false;
    }
    else
    {
        _isKeyDown[dwKey] = false;
        return false;
    }
}

void FinalizeAndSend(void)
{
    HookData* pHookData = GetHookData();

    const KeyConfiguration* pConfiguration = pHookData->GetActiveConfiguration();
    int dataIndex = pHookData->FinalizeCurrentData();

    BOOL bResult = ::PostThreadMessage(pHookData->GetMessageThreadId(), pConfiguration->uMessage, dataIndex, NULL);
    DWORD dwResult = ::GetLastError();

    pHookData->ClearActiveConfiguration();
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
    {
        return ::CallNextHookEx(_hookHandle, nCode, wParam, lParam);
    }

    HookData* pHookData = GetHookData();

    KeyHookLParam hookParam;
    hookParam.lParam = lParam;

    bool isStateKey = true;
    switch (wParam)
    {
    case VK_LCONTROL:
    case VK_RCONTROL:
    case VK_CONTROL:
    case VK_LSHIFT:
    case VK_RSHIFT:
    case VK_SHIFT:
    case VK_LMENU:
    case VK_RMENU:
    case VK_MENU:
        break;

    default:
        isStateKey = false;
        break;
    }

    if (!isStateKey)
    {
        bool bContinueProcessing = true;
        bool bSuppressKey = false;

        if (IsKeyActive(wParam, hookParam.uTransition))
        {
            DWORD dwCurrentKey = GetKeyCodeWithCurrentState(wParam);

            if (pHookData->GetActiveConfigurationIndex() >= 0)
            {
                int iResult = pHookData->AddKey(dwCurrentKey);

                if (iResult > 0)
                {
                    bContinueProcessing = false;
                    bSuppressKey = true;

                    if ((iResult == ADD_SUCCESS_ENDKEY) || (iResult == ADD_SUCCESS_LIMIT))
                    {
                        FinalizeAndSend();
                    }
                }
                else
                {
                    FinalizeAndSend();
                }
            }

            if (bContinueProcessing)
            {
                int iConfigurationIndex = pHookData->FindConfiguration(dwCurrentKey);

                if (iConfigurationIndex >= 0)
                {
                    pHookData->SetActiveConfiguration(iConfigurationIndex);

                    bContinueProcessing = false;
                    bSuppressKey = true;

                    int iResult = pHookData->AddKey(dwCurrentKey);
                    if ((iResult == ADD_SUCCESS_ENDKEY) || (iResult == ADD_SUCCESS_LIMIT))
                    {
                        int dataIndex = pHookData->FinalizeCurrentData();

                        FinalizeAndSend();
                    }
                    else if (iResult < 0)
                    {
                        bSuppressKey = false;

                        pHookData->FinalizeCurrentData();
                        pHookData->ClearActiveConfiguration();
                    }
                }
            }
        }

        if (bSuppressKey)
        {
            return -1;
        }
    }

    return ::CallNextHookEx(_hookHandle, nCode, wParam, lParam);
}

#pragma optimize("", on)