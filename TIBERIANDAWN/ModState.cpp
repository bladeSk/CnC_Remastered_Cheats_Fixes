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

#include "ModState.h"

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

const char* ModState::s_helpMessages[] = {
    "Tiberian Dawn Cheat Mod Help:\n"
    "\n",
    "CTRL+ALT+B:   Enable unlock build mode (cannot disable)\n",
    "CTRL+ALT+H:   Toggle dismiss shroud\n",
    "CTRL+ALT+I:   Toggle instant build for buildings and units\n",
    "CTRL+ALT+N:   Toggle no damage\n",
    "CTRL+ALT+P:   Toggle instant superweapon charging\n",
    "CTRL+ALT+U:   Toggle unlimited ammo for aircrafts\n",
    "CTRL+ALT+M:   Boost credits\n",
    "CTRL+ALT+O:   Boost power\n",
    "CTRL+ALT+-/+: Decrease/Increase harvester load by 50% of normal\n"
    "CTRL+ALT+[/]: Decrease/Increase movement boost by 50% of normal\n",
    "CTRL+ALT+,/.: Decrease/Increase Tiberium growth factor\n",
};

int ModState::s_messageSkipFrames = 2;
char ModState::s_modMessageBuffers[MaxModMessages][MaxModMessageLength] = { 0 };
ModMessage ModState::s_modMessages[MaxModMessages] = { 0 };
int ModState::s_modMessageReadIndex = 0;
int ModState::s_modMessageWriteIndex = 0;
CRITICAL_SECTION ModState::s_modMessageCritSec = { 0 };

static const char* SettingsRegPath = "SOFTWARE\\Electronic Arts\\Command & Conquer Remastered Collection\\Mod\\2236325862";


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
    ModState::AddModMessages(s_helpMessages, ARRAYSIZE(s_helpMessages), 30);

    return true;
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

void ModState::LoadSettings(void)
{
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
        int iNewValue = (int)(MAX(0UL, MIN(dwData, 150UL)));
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

    return hkSettings;
}