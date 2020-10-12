//
// Copyright 2020 Electronic Arts Inc.
//
// TiberianDawn.DLL and RedAlert.dll and corresponding source code is free 
// software: you can redistribute it and/or modify it under the terms of 
// the GNU General Public License as published by the Free Software Foundation, 
// either version 3 of the License, or (at your option) any later version.

// TiberianDawn.DLL and RedAlert.dll and corresponding source code is distributed 
// in the hope that it will be useful, but with permitted additional restrictions 
// under Section 7 of the GPL. See the GNU General Public License in LICENSE.TXT 
// distributed with this program. You should have received a copy of the 
// GNU General Public License along with permitted additional restrictions 
// with this program. If not, see https://github.com/electronicarts/CnC_Remastered_Collection

#include "function.h"

static const char* StartupMessage = "Mod started.\n";

bool ModState::s_isKeyboardHook = true;
bool ModState::s_isNoDamage = false;
bool ModState::s_needHealing = false;
bool ModState::s_isUnlockBuildOptions = false;
bool ModState::s_needUnlockBuildOptions = false;
bool ModState::s_isInstantBuild = false;
bool ModState::s_isInstantSuperweapons = false;
bool ModState::s_isDismissShroud = false;
bool ModState::s_isUnlimitedAmmo = false;

int ModState::s_harvesterBoost = 100;
int ModState::s_movementBoost = 10;

int ModState::s_tiberiumGrowthMultiplier = 1;

volatile long ModState::s_creditBoostAmount = 0;
volatile long ModState::s_powerBoostAmount = 0;

HookConfiguration ModState::s_hookConfiguration = { 0 };

const char* ModState::s_helpBaseText[] = {
    "",
    "Show help",
    "Toggle no damage",
    "Enable unlock build mode (cannot disable)",
    "Toggle instant build for buildings and units",
    "Toggle instant superweapon charging",
    "Toggle dismiss shroud",
    "Toggle unlimited ammo for aircrafts",
    "Boost credits",
    "Boost power",
    "Increase movement boost by 50% of normal",
    "Decrease movement boost by 50% of normal",
    "Increase Tiberium growth factor",
    "Decrease Tiberium growth factor",
    "Increase harvester load by 50% of normal",
    "Decrease harvester load by 50% of normal",
    "Save current settings",
    "Spawn infantry",
    "Spawn vehicle",
    "Spawn aircraft",
};
char ModState::s_modifierKeyNames[3][32] = { 0 };
char ModState::s_helpMessages[KEYHOOK_MAXHOOKKEYS + 2][MaxModMessageLength] = { 0 };

bool ModState::s_needSaveSettings = false;

InfantryType ModState::s_spawnInfantryType = INFANTRY_NONE;
UnitType ModState::s_spawnUnitType = UNIT_NONE;
AircraftType ModState::s_spawnAircraftType = AIRCRAFT_NONE;

InfantryType ModState::s_allowedInfantryTypes[INFANTRY_COUNT] = { INFANTRY_NONE };
UnitType ModState::s_allowedUnitTypes[UNIT_COUNT] = { UNIT_NONE };
AircraftType ModState::s_allowedAircraftTypes[AIRCRAFT_COUNT] = { AIRCRAFT_NONE };

int ModState::s_messageSkipFrames = 2;
char ModState::s_modMessageBuffers[MaxModMessages][MaxModMessageLength] = { 0 };
ModMessage ModState::s_modMessages[MaxModMessages] = { 0 };
int ModState::s_modMessageReadIndex = 0;
int ModState::s_modMessageWriteIndex = 0;
CRITICAL_SECTION ModState::s_modMessageCritSec = { 0 };

static const char* SettingsRegPath = "SOFTWARE\\Electronic Arts\\Command & Conquer Remastered Collection\\Mod\\2254499142";


void ModState::Initialize(void)
{
    InitializeCriticalSection(&s_modMessageCritSec);
    
    for (int index = 0; index < ARRAYSIZE(s_modMessages); index++)
    {
        s_modMessages[index].szMessage = s_modMessageBuffers[index];
        s_modMessages[index].iTimeout = 0;
    }

    ModState::AddModMessage(StartupMessage);

    ModState::LoadSettings();
}


bool ModState::ToggleNoDamage(void)
{
    s_isNoDamage = !s_isNoDamage;
    if (s_isNoDamage)
    {
        s_needHealing = true;
    }

    return s_isNoDamage;
}


bool ModState::ToggleUnlockBuildOptions(void)
{
    if (!s_isUnlockBuildOptions)
    {
        s_isUnlockBuildOptions = !s_isUnlockBuildOptions;
        s_needUnlockBuildOptions = true;
    }

    return s_isUnlockBuildOptions;
}

bool ModState::ToggleInstantBuild(void)
{
    s_isInstantBuild = !s_isInstantBuild;
    return s_isInstantBuild;
}

bool ModState::ToggleInstantSuperweapons(void)
{
    s_isInstantSuperweapons = !s_isInstantSuperweapons;
    return s_isInstantSuperweapons;
}

bool ModState::ToggleDismissShroud(void)
{
    s_isDismissShroud = !s_isDismissShroud;
    return s_isDismissShroud;
}

bool ModState::ToggleUnlimitedAmmo(void)
{
    s_isUnlimitedAmmo = !s_isUnlimitedAmmo;
    return s_isUnlimitedAmmo;
}

bool ModState::IncreaseHarvesterBoost(void)
{
    if (s_harvesterBoost < 150)
    {
        s_harvesterBoost += 5;
        return true;
    }

    return false;
}

bool ModState::DecreaseHarvesterBoost(void)
{
    if (s_harvesterBoost > 10)
    {
        s_harvesterBoost -= 5;
        return true;
    }

    return false;
}

bool ModState::IncreaseMovementBoost(void)
{
    if (s_movementBoost < 50)
    {
        s_movementBoost += 5;
        return true;
    }

    return false;
}

bool ModState::DecreaseMovementBoost(void)
{
    if (s_movementBoost > 5)
    {
        s_movementBoost -= 5;
        return true;
    }

    return false;
}

bool ModState::IncreaseTiberiumGrowthMultiplier(void)
{
    if (s_tiberiumGrowthMultiplier < 50)
    {
        s_tiberiumGrowthMultiplier++;
        return true;
    }

    return false;
}

bool ModState::DecreaseTiberiumGrowthMultiplier(void)
{
    if (s_tiberiumGrowthMultiplier > 1)
    {
        s_tiberiumGrowthMultiplier--;
        return true;
    }

    return false;
}

bool ModState::TriggerNeedShowHelp(void)
{
    static char primaryKeyName[32] = { 0 };
    static char endKeyName[32] = { 0 };

    if (s_modifierKeyNames[0][0] == 0)
    {
        LONG lParam = 0;

        UINT uiAltScanCode = MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC);
        UINT uiCtrlScanCode = MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC);
        UINT uiShiftScanCode = MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);

        lParam = (uiAltScanCode << 16) | (1 << 25);
        GetKeyNameText(lParam, s_modifierKeyNames[0], ARRAYSIZE(s_modifierKeyNames[0]));

        lParam = (uiCtrlScanCode << 16) | (1 << 25);
        GetKeyNameText(lParam, s_modifierKeyNames[1], ARRAYSIZE(s_modifierKeyNames[1]));

        lParam = (uiShiftScanCode << 16) | (1 << 25);
        GetKeyNameText(lParam, s_modifierKeyNames[2], ARRAYSIZE(s_modifierKeyNames[2]));
    }

    if (s_helpMessages[0][0] == 0)
    {
        sprintf_s(s_helpMessages[0], "Tiberian Dawn Cheat Mod Help:\n");
        sprintf_s(s_helpMessages[1], " \n");

        for (int index = 0; index < (int)(s_hookConfiguration.dwEntries); index++)
        {
            KeyConfiguration* pKey = &(s_hookConfiguration.kcEntries[index]);
            DWORD dwPrimaryKey = pKey->dwPrimaryKey;
            DWORD dwBaseKey = dwPrimaryKey & 0xFF;
            UINT uiScanCode = MapVirtualKey(dwBaseKey, MAPVK_VK_TO_VSC);

            GetKeyNameText((uiScanCode << 16) | (1 << 25), primaryKeyName, ARRAYSIZE(primaryKeyName));

            bool isAlt = (dwPrimaryKey & KEYSTATE_ALT) != 0;
            bool isCtrl = (dwPrimaryKey & KEYSTATE_CTRL) != 0;
            bool isShift = (dwPrimaryKey & KEYSTATE_SHIFT) != 0;

            auto pMessage = s_helpMessages[index + 2];

            sprintf_s(pMessage, MaxModMessageLength, "%s: %s%s%s%s%s%s%s",
                s_helpBaseText[pKey->uMessage - WM_USER],
                isCtrl ? s_modifierKeyNames[1] : "",
                isCtrl ? "+" : "",
                isAlt ? s_modifierKeyNames[0] : "",
                isAlt ? "+" : "",
                isShift ? s_modifierKeyNames[2] : "",
                isShift ? "+" : "",
                primaryKeyName);

            if (pKey->bIsChorded && (pKey->dwMaxKeys > 1))
            {
                DWORD dwFillerKeys = pKey->dwMaxKeys - ((pKey->dwEndKey != 0) ? 2 : 1);
                for (DWORD dwIndex = 0; dwIndex < dwFillerKeys; dwIndex++)
                {
                    strcat_s(pMessage, MaxModMessageLength, ", <key>");
                }

                if (pKey->dwEndKey != 0)
                {
                    strcat_s(pMessage, MaxModMessageLength, ", ");

                    dwBaseKey = pKey->dwEndKey & 0xFF;
                    uiScanCode = MapVirtualKey(dwBaseKey, MAPVK_VK_TO_VSC);

                    GetKeyNameText((uiScanCode << 16) | (1 << 25), endKeyName, ARRAYSIZE(endKeyName));

                    isAlt = (pKey->dwEndKey & KEYSTATE_ALT) != 0;
                    isCtrl = (pKey->dwEndKey & KEYSTATE_CTRL) != 0;
                    isShift = (pKey->dwEndKey & KEYSTATE_SHIFT) != 0;

                    if (isCtrl)
                    {
                        strcat_s(pMessage, MaxModMessageLength, s_modifierKeyNames[1]);
                        strcat_s(pMessage, MaxModMessageLength, "+");
                    }

                    if (isAlt)
                    {
                        strcat_s(pMessage, MaxModMessageLength, s_modifierKeyNames[0]);
                        strcat_s(pMessage, MaxModMessageLength, "+");
                    }

                    if (isShift)
                    {
                        strcat_s(pMessage, MaxModMessageLength, s_modifierKeyNames[0]);
                        strcat_s(pMessage, MaxModMessageLength, "+");
                    }

                    strcat_s(pMessage, MaxModMessageLength, endKeyName);
                }
            }
        }
    }

    for (DWORD dwIndex = 0; dwIndex < (s_hookConfiguration.dwEntries + 2); dwIndex++)
    {
        ModState::AddModMessage(s_helpMessages[dwIndex], 30);
    }

    return true;
}

bool ModState::TriggerNeedSaveSettings(void)
{
    s_needSaveSettings = true;

    return true;
}

void ModState::SaveCurrentSettings(void)
{
    s_needSaveSettings = false;

    HKEY hkSettings = SaveSettings();

    RegCloseKey(hkSettings);
}

bool ModState::SetSpawnInfantryFromKeyData(HookKeyData& hkdData)
{
    if (hkdData.dwEntries != 3)
    {
        return false;
    }

    if (s_spawnInfantryType != INFANTRY_NONE)
    {
        return false;
    }

    if (s_allowedInfantryTypes[0] == INFANTRY_NONE)
    {
        int fillIndex = 0;

        for (InfantryType index = INFANTRY_FIRST; index < INFANTRY_COUNT; index++)
        {
            InfantryTypeClass const & typeClass = InfantryTypeClass::As_Reference(index);

            if ((typeClass.Level > 98) || (typeClass.Scenario > 98))
            {
                continue;
            }

            s_allowedInfantryTypes[fillIndex] = index;
            fillIndex++;
        }

        while (fillIndex < INFANTRY_COUNT)
        {
            s_allowedInfantryTypes[fillIndex++] = INFANTRY_NONE;
        }
    }

    int index = GetIndexFromKeyData(hkdData);

    if ((1 <= index) && (index <= INFANTRY_COUNT) && (s_allowedInfantryTypes[index - 1] != INFANTRY_NONE))
    {
        s_spawnInfantryType = s_allowedInfantryTypes[index - 1];
        return true;
    }

    return false;
}

bool ModState::SetSpawnUnitFromKeyData(HookKeyData& hkdData)
{
    if (hkdData.dwEntries != 3)
    {
        return false;
    }

    if (s_spawnUnitType != UNIT_NONE)
    {
        return false;
    }

    if (s_allowedUnitTypes[0] == UNIT_NONE)
    {
        int fillIndex = 0;

        for (UnitType index = UNIT_FIRST; index < UNIT_COUNT; index++)
        {
            UnitTypeClass const & typeClass = UnitTypeClass::As_Reference(index);

            if ((typeClass.Level > 98) || (typeClass.Scenario > 98))
            {
                continue;
            }

            s_allowedUnitTypes[fillIndex] = index;
            fillIndex++;
        }

        while (fillIndex < UNIT_COUNT)
        {
            s_allowedUnitTypes[fillIndex++] = UNIT_NONE;
        }
    }

    int index = GetIndexFromKeyData(hkdData);

    if ((1 <= index) && (index <= UNIT_COUNT) && (s_allowedUnitTypes[index - 1] != UNIT_NONE))
    {
        s_spawnUnitType = s_allowedUnitTypes[index - 1];
        return true;
    }

    return false;
}

bool ModState::SetSpawnAircraftFromKeyData(HookKeyData& hkdData)
{
    if (hkdData.dwEntries != 3)
    {
        return false;
    }

    if (s_spawnAircraftType != AIRCRAFT_NONE)
    {
        return false;
    }

    if (s_allowedAircraftTypes[0] == AIRCRAFT_NONE)
    {
        int fillIndex = 0;

        for (AircraftType index = AIRCRAFT_FIRST; index < AIRCRAFT_COUNT; index++)
        {
            AircraftTypeClass const & typeClass = AircraftTypeClass::As_Reference(index);

            if ((typeClass.Level > 98) || (typeClass.Scenario > 98))
            {
                continue;
            }

            s_allowedAircraftTypes[fillIndex] = index;
            fillIndex++;
        }

        while (fillIndex < AIRCRAFT_COUNT)
        {
            s_allowedAircraftTypes[fillIndex++] = AIRCRAFT_NONE;
        }
    }

    int index = GetIndexFromKeyData(hkdData);

    if ((1 <= index) && (index <= AIRCRAFT_COUNT) && (s_allowedAircraftTypes[index - 1] != AIRCRAFT_NONE))
    {
        s_spawnAircraftType = s_allowedAircraftTypes[index - 1];
        return true;
    }

    return false;
}

void ModState::MarkFrame(void)
{
    EnterCriticalSection(&s_modMessageCritSec);

    if (s_messageSkipFrames > 0)
    {
        s_messageSkipFrames--;
    }

    if (s_messageSkipFrames < 0)
    {
        s_messageSkipFrames = 0;
    }

    LeaveCriticalSection(&s_modMessageCritSec);
}


void ModState::AddModMessage(const char* message, int timeout)
{
    if (message != NULL)
    {
        EnterCriticalSection(&s_modMessageCritSec);

        strncpy_s(s_modMessageBuffers[s_modMessageWriteIndex], message, ARRAYSIZE(s_modMessageBuffers[s_modMessageWriteIndex]));
        s_modMessages[s_modMessageWriteIndex].iTimeout = (timeout > 0) ? timeout : 5;

        s_modMessageWriteIndex++;
        if (s_modMessageWriteIndex >= ARRAYSIZE(s_modMessageBuffers))
        {
            s_modMessageWriteIndex = 0;
        }

        LeaveCriticalSection(&s_modMessageCritSec);
    }
}

void ModState::AddModMessages(const char* messages[], int count, int timeout)
{
    EnterCriticalSection(&s_modMessageCritSec);

    for (int index = 0; index < count; index++)
    {
        if (messages[index] != NULL)
        {
            strncpy_s(s_modMessageBuffers[s_modMessageWriteIndex], messages[index], ARRAYSIZE(s_modMessageBuffers[s_modMessageWriteIndex]));
            s_modMessages[s_modMessageWriteIndex].iTimeout = (timeout > 0) ? timeout : 5;

            s_modMessageWriteIndex++;
            if (s_modMessageWriteIndex >= ARRAYSIZE(s_modMessageBuffers))
            {
                s_modMessageWriteIndex = 0;
            }
        }
    }

    LeaveCriticalSection(&s_modMessageCritSec);
}

const ModMessage* ModState::GetNextModMessage(void)
{
    const ModMessage* message = NULL;

    EnterCriticalSection(&s_modMessageCritSec);

    if (!s_messageSkipFrames && (s_modMessageReadIndex != s_modMessageWriteIndex))
    {
        message = &(s_modMessages[s_modMessageReadIndex]);
        s_modMessageReadIndex++;
        if (s_modMessageReadIndex >= ARRAYSIZE(s_modMessageBuffers))
        {
            s_modMessageReadIndex = 0;
        }
    }

    LeaveCriticalSection(&s_modMessageCritSec);

    return message;
}


int ModState::GetIndexFromKeyData(HookKeyData& hkdData)
{
    int result = 0;
    bool bHaveValue = false;

    for (DWORD dwIndex = 1; dwIndex < hkdData.dwEntries; dwIndex++)
    {
        DWORD dwKey = hkdData.dwKeys[dwIndex] & 0xFF;

        if (((DWORD)'0' <= dwKey) && (dwKey <= (DWORD)'9'))
        {
            bHaveValue = true;
            result = (result * 10) + (dwKey - (DWORD)'0');
        }
        else
        {
            break;
        }
    }

    return bHaveValue ? result : -1;
}

void ModState::SetBoolFromRegistry(HKEY hkSettings, LPCSTR szValue, LPCSTR szName, bool* pbValue)
{
    char buffer[MaxModMessageLength] = { 0 };

    DWORD dwData;
    DWORD dwSize;

    if (::RegGetValue(hkSettings, NULL, szValue, RRF_RT_REG_DWORD, NULL, &dwData, &dwSize) == ERROR_SUCCESS)
    {
        bool bNewValue = (dwData != 0);
        bool bDifferent = (*pbValue != bNewValue);
        
        *pbValue = bNewValue;

        if (bDifferent)
        {
            sprintf_s(buffer, "%s: %s", szName, bNewValue ? "enabled" : "disabled");
            ModState::AddModMessage(buffer);
        }
    }
}

void ModState::SetHookKeyEntry(KeyConfiguration& entry, UINT uMessage, DWORD dwPrimaryKey, BOOL bIsChorded, DWORD dwMaxKeys, DWORD dwEndKey)
{
    entry.uMessage = uMessage;
    entry.dwPrimaryKey = dwPrimaryKey;
    entry.bIsChorded = bIsChorded;
    entry.dwMaxKeys = dwMaxKeys;
    entry.dwEndKey = dwEndKey;
}

void ModState::SetHookKeyEntryFromRegistry(
    KeyConfiguration& entry,
    HKEY hkSettings,
    LPCSTR szValue,
    UINT uMessage)
{
    BYTE abData[sizeof(KeyConfiguration)];
    DWORD dwSize;
    KeyConfiguration* pData = (KeyConfiguration*)abData;

    if (::RegGetValue(hkSettings, NULL, szValue, RRF_RT_REG_BINARY, NULL, &abData, &dwSize) == ERROR_SUCCESS)
    {
        SetHookKeyEntry(entry, uMessage, pData->dwPrimaryKey, pData->bIsChorded, pData->dwMaxKeys, pData->dwEndKey);
    }
}

void ModState::SetHookKeyEntryToRegistry(
    KeyConfiguration& entry,
    HKEY hkSettings,
    LPCSTR szValue,
    LPCSTR szFailMessage)
{
    if (::RegSetValueEx(hkSettings, szValue, 0, REG_BINARY, (const BYTE*)&entry, sizeof(KeyConfiguration)) != ERROR_SUCCESS)
    {
        ModState::AddModMessage(szFailMessage);
    }
}

void ModState::LoadSettings(void)
{
    s_hookConfiguration.dwSize = sizeof(HookConfiguration);
    s_hookConfiguration.dwEntries = 19;

    SetHookKeyEntry(s_hookConfiguration.kcEntries[0], WM_USER + 1, VK_OEM_2 | KEYSTATE_ALT | KEYSTATE_CTRL);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[1], WM_USER + 2, VK_N | KEYSTATE_ALT | KEYSTATE_CTRL);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[2], WM_USER + 3, VK_B | KEYSTATE_ALT | KEYSTATE_CTRL);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[3], WM_USER + 4, VK_I | KEYSTATE_ALT | KEYSTATE_CTRL);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[4], WM_USER + 5, VK_P | KEYSTATE_ALT | KEYSTATE_CTRL);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[5], WM_USER + 6, VK_H | KEYSTATE_ALT | KEYSTATE_CTRL);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[6], WM_USER + 7, VK_U | KEYSTATE_ALT | KEYSTATE_CTRL);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[7], WM_USER + 8, VK_M | KEYSTATE_ALT | KEYSTATE_CTRL);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[8], WM_USER + 9, VK_O | KEYSTATE_ALT | KEYSTATE_CTRL);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[9], WM_USER + 10, VK_OEM_6 | KEYSTATE_ALT | KEYSTATE_CTRL);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[10], WM_USER + 11, VK_OEM_4 | KEYSTATE_ALT | KEYSTATE_CTRL);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[11], WM_USER + 12, VK_OEM_PERIOD | KEYSTATE_ALT | KEYSTATE_CTRL);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[12], WM_USER + 13, VK_OEM_COMMA | KEYSTATE_ALT | KEYSTATE_CTRL);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[13], WM_USER + 14, VK_OEM_PLUS | KEYSTATE_ALT | KEYSTATE_CTRL);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[14], WM_USER + 15, VK_OEM_MINUS | KEYSTATE_ALT | KEYSTATE_CTRL);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[15], WM_USER + 16, VK_S | KEYSTATE_ALT | KEYSTATE_CTRL);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[16], WM_USER + 17, VK_I | KEYSTATE_ALT | KEYSTATE_SHIFT, TRUE, 3);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[17], WM_USER + 18, VK_V | KEYSTATE_ALT | KEYSTATE_SHIFT, TRUE, 3);
    SetHookKeyEntry(s_hookConfiguration.kcEntries[18], WM_USER + 19, VK_A | KEYSTATE_ALT | KEYSTATE_SHIFT, TRUE, 3);

    HKEY hkSettings;
    LSTATUS result = ::RegOpenKeyEx(HKEY_CURRENT_USER, SettingsRegPath, 0, KEY_READ | KEY_WRITE, &hkSettings);

    if (result == ERROR_FILE_NOT_FOUND)
    {
        hkSettings = ModState::SaveSettings();
    }

    if (hkSettings == NULL)
    {
        return;
    }

    char buffer[MaxModMessageLength] = { 0 };

    DWORD dwData;
    DWORD dwSize;
    bool bShowMessage = false;

    SetBoolFromRegistry(hkSettings, "EnableKeyboardHook", "Keyboard hook", &s_isKeyboardHook);
    SetBoolFromRegistry(hkSettings, "EnableNoDamage", "No damage mode", &s_isNoDamage);
    SetBoolFromRegistry(hkSettings, "EnableUnlockBuildOptions", "Unlock build mode", &s_isUnlockBuildOptions);
    SetBoolFromRegistry(hkSettings, "EnableInstantBuild", "Instant build mode", &s_isInstantBuild);
    SetBoolFromRegistry(hkSettings, "EnableInstantSuperweapons", "Instant superweapons mode", &s_isInstantSuperweapons);
    SetBoolFromRegistry(hkSettings, "EnableDismissShroud", "Dismiss shroud mode", &s_isDismissShroud);
    SetBoolFromRegistry(hkSettings, "EnableUnlimitedAmmo", "Unlimited ammo for aircrafts", &s_isUnlimitedAmmo);

    s_needHealing = s_isNoDamage;
    s_needUnlockBuildOptions = s_isUnlockBuildOptions;

    if (::RegGetValue(hkSettings, NULL, "HarvesterBoost", RRF_RT_REG_DWORD, NULL, &dwData, &dwSize) == ERROR_SUCCESS)
    {
        int iNewValue = (int)(MAX(10UL, MIN(dwData, 150UL)));
        bool bIsDifferent = (s_harvesterBoost != iNewValue);
        s_harvesterBoost = iNewValue;

        if (bIsDifferent)
        {
            sprintf_s(buffer, "Harvester load: %.0f%% of normal", ModState::GetHarvestorBoost() * 100.0f);
            ModState::AddModMessage(buffer);
        }
    }

    if (::RegGetValue(hkSettings, NULL, "MovementBoost", RRF_RT_REG_DWORD, NULL, &dwData, &dwSize) == ERROR_SUCCESS)
    {
        int iNewValue = (int)(MAX(5UL, MIN(dwData, 50UL)));;
        bool bIsDifferent = (s_movementBoost != iNewValue);
        s_movementBoost = iNewValue;

        if (bIsDifferent)
        {
            sprintf_s(buffer, "Movement boost: %.0f%%", ModState::GetMovementBoost() * 100.0f);
            ModState::AddModMessage(buffer);
        }
    }

    if (::RegGetValue(hkSettings, NULL, "TiberiumGrowthMultiplier", RRF_RT_REG_DWORD, NULL, &dwData, &dwSize) == ERROR_SUCCESS)
    {
        int iNewValue = (int)(MAX(1UL, MIN(dwData, 50UL)));;
        bool bIsDifferent = (s_tiberiumGrowthMultiplier != iNewValue);
        s_tiberiumGrowthMultiplier = iNewValue;

        if (bIsDifferent)
        {
            sprintf_s(buffer, "Tiberium growth multiplier: %d", ModState::GetTiberiumGrowthMultiplier());
            ModState::AddModMessage(buffer);
        }
    }

    if (::RegGetValue(hkSettings, NULL, "InitialCreditBoost", RRF_RT_REG_DWORD, NULL, &dwData, &dwSize) == ERROR_SUCCESS)
    {
        s_creditBoostAmount = MAX(0L, (long)dwData);

        if (s_creditBoostAmount > 0)
        {
            ModState::AddModMessage("Credits boosted");
        }
    }

    if (::RegGetValue(hkSettings, NULL, "InitialPowerBoost", RRF_RT_REG_DWORD, NULL, &dwData, &dwSize) == ERROR_SUCCESS)
    {
        s_powerBoostAmount = MAX(0L, (long)dwData);

        if (s_powerBoostAmount > 0)
        {
            ModState::AddModMessage("Power boosted");
        }
    }

    s_hookConfiguration.dwSize = sizeof(HookConfiguration);
    s_hookConfiguration.dwEntries = 19;

    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[0], hkSettings, "KeyHelp", WM_USER + 1);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[1], hkSettings, "KeyToggleNoDamage", WM_USER + 2);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[2], hkSettings, "KeyToggleUnlockBuildOptions", WM_USER + 3);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[3], hkSettings, "KeyToggleInstantBuild", WM_USER + 4);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[4], hkSettings, "KeyToggleInstantSuperweapons", WM_USER + 5);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[5], hkSettings, "KeyToggleDismissShroud", WM_USER + 6);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[6], hkSettings, "KeyToggleUnlimitedAmmo", WM_USER + 7);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[7], hkSettings, "KeyCreditBoost", WM_USER + 8);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[8], hkSettings, "KeyPowerBoost", WM_USER + 9);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[9], hkSettings, "KeyIncreaseMovementBoost", WM_USER + 10);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[10], hkSettings, "KeyDecreaseMovementBoost", WM_USER + 11);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[11], hkSettings, "KeyIncreaseTiberiumGrowth", WM_USER + 12);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[12], hkSettings, "KeyDecreaseTiberiumGrowth", WM_USER + 13);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[13], hkSettings, "KeyIncreaseHarvesterBoost", WM_USER + 14);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[14], hkSettings, "KeyDecreaseHarvesterBoost", WM_USER + 15);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[15], hkSettings, "KeySaveSettings", WM_USER + 16);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[16], hkSettings, "KeySpawnInfantry", WM_USER + 17);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[17], hkSettings, "KeySpawnVehicle", WM_USER + 18);
    SetHookKeyEntryFromRegistry(s_hookConfiguration.kcEntries[18], hkSettings, "KeySpawnVehicle", WM_USER + 19);

    RegCloseKey(hkSettings);
}

HKEY ModState::SaveSettings(void)
{
    HKEY hkSettings;
    LSTATUS result = ::RegCreateKeyEx(HKEY_CURRENT_USER, SettingsRegPath, 0, NULL, 0, KEY_READ | KEY_WRITE, NULL, &hkSettings, NULL);

    if (result != ERROR_SUCCESS)
    {
        return NULL;
    }

    DWORD dwData = s_isKeyboardHook ? 1 : 0;
    if (::RegSetValueEx(hkSettings, "EnableKeyboardHook", 0, REG_DWORD, (LPCBYTE)&dwData, sizeof(dwData)) != ERROR_SUCCESS)
    {
        ModState::AddModMessage("Failed to save keyboard hook flag.");
    }

    dwData = s_isNoDamage ? 1 : 0;
    if (::RegSetValueEx(hkSettings, "EnableNoDamage", 0, REG_DWORD, (LPCBYTE)&dwData, sizeof(dwData)) != ERROR_SUCCESS)
    {
        ModState::AddModMessage("Failed to save no damage flag.");
    }

    dwData = s_isUnlockBuildOptions ? 1 : 0;
    if (::RegSetValueEx(hkSettings, "EnableUnlockBuildOptions", 0, REG_DWORD, (LPCBYTE)&dwData, sizeof(dwData)) != ERROR_SUCCESS)
    {
        ModState::AddModMessage("Failed to save no damage flag.");
    }

    dwData = s_isInstantBuild ? 1 : 0;
    if (::RegSetValueEx(hkSettings, "EnableInstantBuild", 0, REG_DWORD, (LPCBYTE)&dwData, sizeof(dwData)) != ERROR_SUCCESS)
    {
        ModState::AddModMessage("Failed to save instant build flag.");
    }

    dwData = s_isInstantSuperweapons ? 1 : 0;
    if (::RegSetValueEx(hkSettings, "EnableInstantSuperweapons", 0, REG_DWORD, (LPCBYTE)&dwData, sizeof(dwData)) != ERROR_SUCCESS)
    {
        ModState::AddModMessage("Failed to save instant superweapons flag.");
    }

    dwData = s_isDismissShroud ? 1 : 0;
    if (::RegSetValueEx(hkSettings, "EnableDismissShroud", 0, REG_DWORD, (LPCBYTE)&dwData, sizeof(dwData)) != ERROR_SUCCESS)
    {
        ModState::AddModMessage("Failed to save dismiss shroud flag.");
    }

    dwData = s_isUnlimitedAmmo ? 1 : 0;
    if (::RegSetValueEx(hkSettings, "EnableUnlimitedAmmo", 0, REG_DWORD, (LPCBYTE)&dwData, sizeof(dwData)) != ERROR_SUCCESS)
    {
        ModState::AddModMessage("Failed to save unlimited ammo flag.");
    }

    dwData = s_harvesterBoost;
    if (::RegSetValueEx(hkSettings, "HarvesterBoost", 0, REG_DWORD, (LPCBYTE)&dwData, sizeof(dwData)) != ERROR_SUCCESS)
    {
        ModState::AddModMessage("Failed to save harvester boost value.");
    }

    dwData = s_tiberiumGrowthMultiplier;
    if (::RegSetValueEx(hkSettings, "TiberiumGrowthMultiplier", 0, REG_DWORD, (LPCBYTE)&dwData, sizeof(dwData)) != ERROR_SUCCESS)
    {
        ModState::AddModMessage("Failed to save Tiberium growth multiplier value.");
    }

    dwData = s_movementBoost;
    if (::RegSetValueEx(hkSettings, "MovementBoost", 0, REG_DWORD, (LPCBYTE)&dwData, sizeof(dwData)) != ERROR_SUCCESS)
    {
        ModState::AddModMessage("Failed to save harvester boost value.");
    }

    dwData = 0;
    if (::RegSetValueEx(hkSettings, "InitialCreditBoost", 0, REG_DWORD, (LPCBYTE)&dwData, sizeof(dwData)) != ERROR_SUCCESS)
    {
        ModState::AddModMessage("Failed to save credit boost value.");
    }
    if (::RegSetValueEx(hkSettings, "InitialPowerBoost", 0, REG_DWORD, (LPCBYTE)&dwData, sizeof(dwData)) != ERROR_SUCCESS)
    {
        ModState::AddModMessage("Failed to save power boost value.");
    }

    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[0], hkSettings, "KeyHelp", "Failed to save help key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[1], hkSettings, "KeyToggleNoDamage", "Failed to save no damage key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[2], hkSettings, "KeyToggleUnlockBuildOptions", "Failed to save unlock build options key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[3], hkSettings, "KeyToggleInstantBuild", "Failed to save instant build key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[4], hkSettings, "KeyToggleInstantSuperweapons", "Failed to save instant superweapons key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[5], hkSettings, "KeyToggleDismissShroud", "Failed to save dismiss shroud key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[6], hkSettings, "KeyToggleUnlimitedAmmo", "Failed to save unlimited ammo key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[7], hkSettings, "KeyCreditBoost", "Failed to save credit boost key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[8], hkSettings, "KeyPowerBoost", "Failed to save power boost key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[9], hkSettings, "KeyIncreaseMovementBoost", "Failed to save increase movement boost key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[10], hkSettings, "KeyDecreaseMovementBoost", "Failed to save decrease movement boost key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[11], hkSettings, "KeyIncreaseTiberiumGrowth", "Failed to save increase Tiberium growth factor key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[12], hkSettings, "KeyDecreaseTiberiumGrowth", "Failed to save decrease Tiberium growth factor key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[13], hkSettings, "KeyIncreaseHarvesterBoost", "Failed to save increase harvester boost key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[14], hkSettings, "KeyDecreaseHarvesterBoost", "Failed to save decrease harvester boost key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[15], hkSettings, "KeySaveSettings", "Failed to save save settings key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[16], hkSettings, "KeySpawnInfantry", "Failed to save spawn infantry key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[17], hkSettings, "KeySpawnVehicle", "Failed to save spawn vehicle key configuration.");
    SetHookKeyEntryToRegistry(s_hookConfiguration.kcEntries[18], hkSettings, "KeySpawnVehicle", "Failed to save spawn aircraft key configuration.");

    return hkSettings;
}