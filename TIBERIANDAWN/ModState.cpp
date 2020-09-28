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

bool ModState::s_needShowHelp = false;
int ModState::s_helpMessageIndex = 0;
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

char ModState::s_debugMessageBuffer[256] = { 0 };
bool ModState::s_isDebugMessageRead = true;
bool ModState::s_isFirstMessage = true;


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
    if (!s_needShowHelp)
    {
        s_needShowHelp = true;
    }

    return true;
}

const char* ModState::GetNextHelpMessageLine()
{
    if ((s_helpMessageIndex < 0) || (s_helpMessageIndex >= (sizeof(s_helpMessages) / sizeof(s_helpMessages[0]))))
    {
        s_needShowHelp = false;
        return NULL;
    }

    return s_helpMessages[s_helpMessageIndex++];
}

void ModState::SetDebugMessage(const char* message)
{
    if (message != NULL)
    {
        strncpy_s(s_debugMessageBuffer, message, sizeof(s_debugMessageBuffer) / sizeof(s_debugMessageBuffer[0]));
        s_isDebugMessageRead = false;
    }
}

const char* ModState::GetAndClearDebugMessage(void)
{
    if (s_isFirstMessage)
    {
        s_isFirstMessage = false;
        return StartupMessage;
    }
    else if (!s_isDebugMessageRead && (s_debugMessageBuffer[0] != '\0'))
    {
        s_isDebugMessageRead = true;
        return s_debugMessageBuffer;
    }

    return NULL;
}
