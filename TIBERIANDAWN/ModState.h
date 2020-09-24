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

#ifndef MODSTATE_H
#define MODSTATE_H

extern bool Debug_Unshroud;

class ModState
{
private:
    static bool s_isNoDamageMode;
    static bool s_needsHealing;
    static bool s_isUnlockBuildOptions;
    static bool s_needUnlockBuildOptions;
    static bool s_isInstantBuild;
    static bool s_isInstantSuperweapons;
    static bool s_isDismissShroud;

    static float s_harvesterBoost;

    static volatile long s_creditBoostAmount;
    static volatile long s_powerBoostAmount;

    static char s_debugMessageBuffer[256];
    static bool s_isDebugMessageRead;
    static bool s_isFirstMessage;

public:
    static bool ToggleNoDamageMode(void);
    
    static inline bool IsNoDamageModeEnabled(void) { return s_isNoDamageMode; };

    static inline bool NeedsHealing(void)
    {
        if (s_needsHealing)
        {
            s_needsHealing = false;
            return true;
        }

        return false;
    }
    
    static inline bool IsCrushable(TechnoClass* target)
    {
        if (target->House == PlayerPtr)
        {
            return !s_isNoDamageMode;
        }

        return target->Class_Of().IsCrushable;
    }
    
    static inline bool IsCrushable(ObjectClass* target)
    {
        if (target->Is_Techno() && (((TechnoClass*)target)->House == PlayerPtr))
        {
            return !s_isNoDamageMode;
        }

        return target->Class_Of().IsCrushable;
    }

    static bool ToggleUnlockBuildOptions(void);

    static inline bool IsUnlockBuildOptionsEnabled(void)
    {
        return s_isUnlockBuildOptions;
    }

    static inline bool NeedUpdateUnlockBuildOptions(void)
    {
        if (s_needUnlockBuildOptions)
        {
            s_needUnlockBuildOptions = false;
            return true;
        }

        return false;
    }

    static bool ToggleInstantBuild(void);
    
    static inline bool IsInstantBuildEnabled(void)
    {
        return s_isInstantBuild;
    }

    static bool ToggleInstantSuperweapons(void);

    static inline bool IsInstantSuperweaponsEnabled(void)
    {
        return s_isInstantSuperweapons;
    }

    static bool ToggleDismissShroud(void);

    static inline bool IsDismissShroudEnabled(void)
    {
        return s_isDismissShroud;
    }

    static inline bool CanDismissShroud(void)
    {
        return (Debug_Unshroud || s_isDismissShroud);
    }

    static bool IncreaseHarvesterBoost(void);
    static bool DecreaseHarvesterBoost(void);

    static inline int GetHarvesterFullLoadCredits(void)
    {
        return (int)(UnitTypeClass::FULL_LOAD_CREDITS * s_harvesterBoost);
    }

    static inline float GetHarvestorBoost(void)
    {
        return s_harvesterBoost;
    }

    static inline void IncreaseCreditBoost(void)
    {
        InterlockedAdd(&s_creditBoostAmount, 10000);
    }

    static inline long GetCreditBoost(void)
    {
        return InterlockedExchange(&s_creditBoostAmount, 0);
    }

    static inline void IncreasePowerBoost(void)
    {
        InterlockedAdd(&s_powerBoostAmount, 200);
    }

    static inline int GetPowerBoost(void)
    {
        return (int)InterlockedExchange(&s_powerBoostAmount, 0);
    }

    static void SetDebugMessage(const char* message);
    static const char* GetAndClearDebugMessage(void);
};

#endif
