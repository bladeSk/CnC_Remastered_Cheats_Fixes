#pragma once

constexpr auto KEYSTATE_SHIFT = 0x00010000;
constexpr auto KEYSTATE_CTRL = 0x00020000;
constexpr auto KEYSTATE_ALT = 0x00040000;

constexpr auto KEYHOOK_MAXCHORDLEN = 16;
constexpr auto KEYHOOK_MAXHOOKKEYS = 32;

struct KeyConfiguration
{
    DWORD dwPrimaryKey;
    BOOL bIsChorded;
    DWORD dwMaxKeys;
    DWORD dwEndKey;
    UINT uMessage;
};

struct HookConfiguration
{
    DWORD dwSize;
    DWORD dwMessageThreadId;
    DWORD dwEntries;
    KeyConfiguration kcEntries[KEYHOOK_MAXHOOKKEYS];
};

typedef HookConfiguration HOOKCONFIGURATION, *LPHOOKCONFIGURATION;
typedef const HookConfiguration *LPCHOOKCONFIGURATION;

struct HookKeyData
{
    DWORD dwEntries;
    DWORD dwKeys[KEYHOOK_MAXCHORDLEN];
};

typedef HookKeyData HOOKKEYDATA, *LPHOOKKEYDATA;
typedef const HookKeyData *LPCHOOKKEYDATA;

union KeyHookLParam
{
    LPARAM lParam;
    struct
    {
        UINT uRepeat : 16;
        UINT uScan : 8;
        UINT uExt : 1;
        UINT uReserved : 4;
        UINT uContext : 1;
        UINT uPrevious : 1;
        UINT uTransition : 1;
    };
};

extern "C"
{
    typedef BOOL(__cdecl *InstallHookProc)(LPCHOOKCONFIGURATION);
    typedef BOOL(__cdecl *UninstallHookProc)(void);
    typedef BOOL(__cdecl *GetKeyDataProc)(WPARAM, LPHOOKKEYDATA);
}
