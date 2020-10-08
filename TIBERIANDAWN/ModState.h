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
    static bool s_isKeyboardHook;
    static bool s_isNoDamage;
    static bool s_needHealing;
    static bool s_isUnlockBuildOptions;
    static bool s_needUnlockBuildOptions;
    static bool s_isInstantBuild;
    static bool s_isInstantSuperweapons;
    static bool s_isDismissShroud;
    static bool s_isUnlimitedAmmo;

    static int s_harvesterBoost;
    static int s_movementBoost;

    static int s_tiberiumGrowthMultiplier;

    static volatile long s_creditBoostAmount;
    static volatile long s_powerBoostAmount;

    static const char* s_helpMessages[];

    static int s_messageSkipFrames;
    static char s_modMessageBuffers[256][256];
    static int s_modMessageReadIndex;
    static int s_modMessageWriteIndex;
    static CRITICAL_SECTION s_modMessageCritSec;

public:
    static void Initialize(void);

    static inline bool IsKeyboardHook(void) { return s_isKeyboardHook; }

    static bool ToggleNoDamage(void);

    static inline bool IsNoDamageEnabled(void) { return s_isNoDamage; };

    static inline bool NeedHealing(void)
    {
        if (s_needHealing)
        {
            s_needHealing = false;
            return true;
        }

        return false;
    }

    static inline bool IsCrushable(TechnoClass* target)
    {
        if (s_isNoDamage && target->House == PlayerPtr)
        {
            return false;
        }

        return target->Class_Of().IsCrushable;
    }

    static inline bool IsCrushable(ObjectClass* target)
    {
        if (s_isNoDamage && target->Is_Techno() && (((TechnoClass*)target)->House == PlayerPtr))
        {
            return false;
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

    static bool ToggleUnlimitedAmmo(void);

    static inline bool IsUnlimitedAmmoEnabled(void)
    {
        return s_isUnlimitedAmmo;
    }

    static inline bool IsUnlimitedAmmoEnabled(TechnoClass* object)
    {
        if (object->House == PlayerPtr)
        {
            if (object->What_Am_I() == RTTI_AIRCRAFT)
            {
                AircraftType type = *((AircraftClass*)object);

                if ((type == AIRCRAFT_ORCA) || (type == AIRCRAFT_HELICOPTER))
                {
                    return s_isUnlimitedAmmo;
                }
            }
        }

        return false;
    }

    static bool IncreaseHarvesterBoost(void);
    static bool DecreaseHarvesterBoost(void);

    static inline int GetHarvesterFullLoadCredits(void)
    {
        return (int)(UnitTypeClass::FULL_LOAD_CREDITS * s_harvesterBoost / 10.0f);
    }

    static inline float GetHarvestorBoost(void)
    {
        return (s_harvesterBoost / 10.0f);
    }

    static bool IncreaseMovementBoost(void);
    static bool DecreaseMovementBoost(void);

    static inline float GetMovementBoost(void)
    {
        return s_movementBoost / 10.0f;
    }

    static inline float GetGroundSpeedBias(HouseClass* house)
    {
        if (house == PlayerPtr)
        {
            return ((house->GroundspeedBias * s_movementBoost) / 10.0f);
        }

        return house->GroundspeedBias;
    }

    static inline float GetAirspeedBias(HouseClass* house, AircraftClass* object)
    {
        if (house == PlayerPtr)
        {
            float bias = ((house->AirspeedBias * s_movementBoost) / 10.0f);

            if ((*object) == AIRCRAFT_A10)
            {
                // If the A10 goes too fast, it misses the target and gets stuck circling the target forever.
                return min(bias, 1.5f);
            }

            return bias;
        }

        return house->AirspeedBias;
    }

    static bool IncreaseTiberiumGrowthMultiplier(void);
    static bool DecreaseTiberiumGrowthMultiplier(void);

    static inline int GetTiberiumGrowthMultiplier(void)
    {
        return s_tiberiumGrowthMultiplier;
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

    static bool TriggerNeedShowHelp(void);

    static void MarkFrame(void);

    static void AddModMessage(const char* message);
    static void AddModMessages(__in_ecount(count) const char* messages[], int count);
    static const char* GetNextModMessage(void);

private:
    static void SetBoolFromRegistry(__in HKEY hkSettings, __in_z LPCSTR szValue, __in_z LPCSTR szName, __inout bool* pbValue);

    static void LoadSettings(void);
    static HKEY SaveSettings(void);
};

#endif
