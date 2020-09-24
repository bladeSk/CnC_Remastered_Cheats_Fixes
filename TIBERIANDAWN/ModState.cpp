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

const char* StartupMessage = "Mod started.\n";

bool ModState::s_isNoDamageMode = false;
bool ModState::s_needsHealing = false;
bool ModState::s_isUnlockBuildOptions = false;
bool ModState::s_needUnlockBuildOptions = false;
bool ModState::s_isInstantBuild = false;
bool ModState::s_isInstantSuperweapons = false;
bool ModState::s_isDismissShroud = false;

float ModState::s_harvesterBoost = 10.0f;
float ModState::s_movementBoost = 1.0f;

int ModState::s_tiberiumGrowthMultiplier = 1;

volatile long ModState::s_creditBoostAmount = 0;
volatile long ModState::s_powerBoostAmount = 0;

char ModState::s_debugMessageBuffer[256] = { 0 };
bool ModState::s_isDebugMessageRead = true;
bool ModState::s_isFirstMessage = true;


bool ModState::ToggleNoDamageMode(void)
{
    s_isNoDamageMode = !s_isNoDamageMode;
    s_needsHealing = true;
    return s_isNoDamageMode;
}


bool ModState::ToggleUnlockBuildOptions(void)
{
    s_isUnlockBuildOptions = !s_isUnlockBuildOptions;
    s_needUnlockBuildOptions = true;
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

bool ModState::IncreaseHarvesterBoost(void)
{
    if (s_harvesterBoost < 15.0f)
    {
        s_harvesterBoost += 0.5f;
        if (s_harvesterBoost > 15.0f)
        {
            s_harvesterBoost = 15.0f;
        }

        return true;
    }

    return false;
}

bool ModState::DecreaseHarvesterBoost(void)
{
    if (s_harvesterBoost > 1.0f)
    {
        s_harvesterBoost -= 0.5f;
        if (s_harvesterBoost < 1.0f)
        {
            s_harvesterBoost = 1.0f;
        }

        return true;
    }

    return false;
}

bool ModState::IncreaseMovementBoost(void)
{
    if (s_movementBoost < 5.0f)
    {
        s_movementBoost += 0.5f;
        if (s_movementBoost > 5.0f)
        {
            s_movementBoost = 5.0f;
        }

        return true;
    }

    return false;
}

bool ModState::DecreaseMovementBoost(void)
{
    if (s_movementBoost > 0.5f)
    {
        s_movementBoost -= 0.5f;
        if (s_movementBoost < 0.5f)
        {
            s_movementBoost = 0.5f;
        }

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