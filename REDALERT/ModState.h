#ifndef MODSTATE_H
#define MODSTATE_H

constexpr auto MaxModMessages = 256;
constexpr auto MaxModMessageLength = 256;

extern bool Debug_Unshroud;

struct ModMessage
{
    const char* szMessage;
    int iTimeout;
};

struct SettingInfo
{
    const char* const _szDescription;
};

class ModState
{
private:
    static DWordSetting<SettingInfo> s_lastLoadedVersion;

    static BooleanSetting<SettingInfo> s_isKeyboardHook;
    static BooleanSetting<SettingInfo> s_isNoDamage;
    static BooleanSetting<SettingInfo> s_needHealing;
    static BooleanSetting<SettingInfo> s_isUnlockBuildOptions;
    static BooleanSetting<SettingInfo> s_needUnlockBuildOptions;
    static BooleanSetting<SettingInfo> s_isInstantBuild;
    static BooleanSetting<SettingInfo> s_isInstantSuperweapons;
    static BooleanSetting<SettingInfo> s_isDismissShroud;
    static BooleanSetting<SettingInfo> s_isUnlimitedAmmo;

    static const BooleanSetting<SettingInfo>* s_booleanSettings[];

    static DWordSetting<SettingInfo> s_harvesterBoost;
    static DWordSetting<SettingInfo> s_movementBoost;

    static DWordSetting<SettingInfo> s_resourceGrowthMultiplier;

    static DWordSetting<SettingInfo> s_initialCreditBoost;
    static DWordSetting<SettingInfo> s_initialPowerBoost;

    static BinarySetting<KeyConfiguration, SettingInfo> s_keyHelpKeySetting;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keyToggleNoDamage;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keyToggleUnlockBuildOptions;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keyToggleInstantBuild;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keyToggleInstantSuperweapons;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keyToggleDismissShroud;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keyToggleUnlimitedAmmo;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keyCreditBoost;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keyPowerBoost;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keyIncreaseMovementBoost;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keyDecreaseMovementBoost;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keyIncreaseResourceGrowth;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keyDecreaseResourceGrowth;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keyDecreaseHarvesterBoost;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keyIncreaseHarvesterBoost;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keySaveSettings;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keySpawnInfantry;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keySpawnVehicle;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keySpawnAircraft;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keySpawnVessel;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keyCapture;
    static BinarySetting<KeyConfiguration, SettingInfo> s_keyResetToDefault;

    static BinarySetting<KeyConfiguration, SettingInfo>* s_keyBindings[];

    static Settings<SettingInfo> s_settings;

    static volatile long s_creditBoostAmount;
    static volatile long s_powerBoostAmount;

    static HookConfiguration s_hookConfiguration;

    static const char* s_helpBaseText[];
    static char s_modifierKeyNames[3][32];
    static char s_helpMessages[KEYHOOK_MAXHOOKKEYS + 2][MaxModMessageLength];

    static bool s_needSaveSettings;
    static bool s_needResetSettingsToDefault;

    static InfantryType s_spawnInfantryType;
    static UnitType s_spawnUnitType;
    static AircraftType s_spawnAircraftType;
    static VesselType s_spawnVesselType;

    static InfantryType s_lastSpawnInfantryType;
    static UnitType s_lastSpawnUnitType;
    static AircraftType s_lastSpawnAircraftType;
    static VesselType s_lastSpawnVesselType;

    static int s_allowedInfantryTypes[INFANTRY_COUNT];
    static int s_allowedUnitTypes[UNIT_COUNT];
    static int s_allowedAircraftTypes[AIRCRAFT_COUNT];
    static int s_allowedVesselTypes[VESSEL_COUNT];

    static HousesType s_captureHouseType;

    static int s_messageSkipFrames;
    static char s_modMessageBuffers[MaxModMessages][MaxModMessageLength];
    static ModMessage s_modMessages[MaxModMessages];
    static int s_modMessageReadIndex;
    static int s_modMessageWriteIndex;
    static CRITICAL_SECTION s_modMessageCritSec;

public:
    static void Initialize(void);

    static inline bool IsKeyboardHook(void) { return *s_isKeyboardHook; }

    static bool ToggleNoDamage(void);

    static inline bool IsNoDamageEnabled(void) { return *s_isNoDamage; };

    static inline bool NeedHealing(void)
    {
        if (*s_needHealing)
        {
            *s_needHealing = false;
            return true;
        }

        return false;
    }

    static inline bool IsCrushable(const TechnoClass* target)
    {
        if (*s_isNoDamage && ((PlayerPtr == target->House) || target->House->IsPlayerControl))
        {
            return false;
        }

        return target->Class_Of().IsCrushable;
    }

    static inline bool IsCrushable(const ObjectClass* target)
    {
        if (*s_isNoDamage &&
            target->Is_Techno() &&
            ((PlayerPtr == ((TechnoClass*)target)->House) || ((TechnoClass*)target)->House->IsPlayerControl))
        {
            return false;
        }

        return target->Class_Of().IsCrushable;
    }

    static bool ToggleUnlockBuildOptions(void);

    static inline bool IsUnlockBuildOptionsEnabled(void)
    {
        return *s_isUnlockBuildOptions;
    }

    static inline bool NeedUpdateUnlockBuildOptions(void)
    {
        if (*s_needUnlockBuildOptions)
        {
            *s_needUnlockBuildOptions = false;
            return true;
        }

        return false;
    }

    static bool ToggleInstantBuild(void);

    static inline bool IsInstantBuildEnabled(void)
    {
        return *s_isInstantBuild;
    }

    static bool ToggleInstantSuperweapons(void);

    static inline bool IsInstantSuperweaponsEnabled(void)
    {
        return *s_isInstantSuperweapons;
    }

    static bool ToggleDismissShroud(void);

    static inline bool IsDismissShroudEnabled(void)
    {
        return *s_isDismissShroud;
    }

    static inline bool CanDismissShroud(void)
    {
        return (Debug_Unshroud || *s_isDismissShroud);
    }

    static bool ToggleUnlimitedAmmo(void);

    static inline bool IsUnlimitedAmmoEnabled(void)
    {
        return *s_isUnlimitedAmmo;
    }

    static inline bool IsUnlimitedAmmoEnabled(TechnoClass* object)
    {
        if ((object->House == PlayerPtr) || object->House->IsPlayerControl)
        {
            if (object->What_Am_I() == RTTI_AIRCRAFT)
            {
                AircraftType type = *((AircraftClass*)object);

                if ((type == AIRCRAFT_MIG) || (type == AIRCRAFT_YAK) || (type == AIRCRAFT_LONGBOW) || (type == AIRCRAFT_HIND))
                {
                    return *s_isUnlimitedAmmo;
                }
            }
        }

        return false;
    }

    static bool IncreaseHarvesterBoost(void);
    static bool DecreaseHarvesterBoost(void);

    static inline float GetHarvesterBoost(void)
    {
        return (*s_harvesterBoost / 10.0f);
    }

    static bool IncreaseMovementBoost(void);
    static bool DecreaseMovementBoost(void);

    static inline float GetMovementBoost(void)
    {
        return *s_movementBoost / 10.0f;
    }

    static inline fixed GetGroundSpeedBias(HouseClass* house)
    {
        if ((house == PlayerPtr) || house->IsPlayerControl)
        {
            return (house->GroundspeedBias * fixed(GetMovementBoost()));
        }

        return house->GroundspeedBias;
    }

    static inline fixed GetAirspeedBias(HouseClass* house, AircraftClass* object)
    {
        if ((house == PlayerPtr) || house->IsPlayerControl)
        {
            fixed bias = (house->AirspeedBias * fixed(GetMovementBoost()));

            if (((*object) == AIRCRAFT_BADGER) || ((*object) == AIRCRAFT_U2))
            {
                // Limit the non-player controllable aircrafts to some sane boost speeds.
                return min(bias, fixed(1.5f));
            }

            return bias;
        }

        return house->AirspeedBias;
    }

    static bool IncreaseResourceGrowthMultiplier(void);
    static bool DecreaseResourceGrowthMultiplier(void);

    static inline int GetResourceGrowthMultiplier(void)
    {
        return *s_resourceGrowthMultiplier;
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

    static const HookConfiguration* GetHookConfiguration(void)
    {
        return &s_hookConfiguration;
    }

    static void SetHookMessageThreadId(DWORD dwThreadId)
    {
        s_hookConfiguration.dwMessageThreadId = dwThreadId;
    }

    static bool TriggerNeedShowHelp(void);

    static bool TriggerNeedSaveSettings(void);

    static bool TriggerNeedResetSettingsToDefault(void);

    static bool NeedSaveSettings(void)
    {
        return s_needSaveSettings;
    }

    static bool NeedResetSettingsToDefault(void)
    {
        return s_needResetSettingsToDefault;
    }

    static void SaveCurrentSettings(void);
    static void ResetSettingsToDefault(void);

    static InfantryType SetSpawnInfantryFromKeyData(HookKeyData& hkdData);
    static UnitType SetSpawnUnitFromKeyData(HookKeyData& hkdData);
    static AircraftType SetSpawnAircraftFromKeyData(HookKeyData& hkData);
    static VesselType SetSpawnVesselFromKeyData(HookKeyData& hkData);

    static InfantryType GetSpawnInfantryType(void)
    {
        InfantryType result = s_spawnInfantryType;
        s_spawnInfantryType = INFANTRY_NONE;
        
        return result;
    }

    static UnitType GetSpawnUnitType(void)
    {
        UnitType result = s_spawnUnitType;
        s_spawnUnitType = UNIT_NONE;

        return result;
    }

    static AircraftType GetSpawnAircraftType(void)
    {
        AircraftType result = s_spawnAircraftType;
        s_spawnAircraftType = AIRCRAFT_NONE;

        return result;
    }

    static VesselType GetSpawnVesselType(void)
    {
        VesselType result = s_spawnVesselType;
        s_spawnVesselType = VESSEL_NONE;

        return result;
    }

    static void SetLastSpawnInfantryType(InfantryType type)
    {
        s_lastSpawnInfantryType = type;
    }

    static void SetLastSpawnUnitType(UnitType type)
    {
        s_lastSpawnUnitType = type;
    }

    static void SetLastSpawnAircraftType(AircraftType type)
    {
        s_lastSpawnAircraftType = type;
    }

    static void SetLastSpawnVesselType(VesselType type)
    {
        s_lastSpawnVesselType = type;
    }

    static HousesType SetCaptureHouseFromKeyData(HookKeyData& hkdData);

    static HousesType GetCaptureHouse(void)
    {
        HousesType result = s_captureHouseType;
        s_captureHouseType = HOUSE_NONE;

        return result;
    }

    static void MarkFrame(void);

    static void AddModMessage(const char* message, int timeout = 0);
    static void AddModMessages(__in_ecount(count) const char* messages[], int count, int timeout = 0);
    static const ModMessage* GetNextModMessage(void);

private:
    static bool IsSpawnable(const TechnoTypeClass* type);

    static int GetIndexFromKeyData(HookKeyData& hkdData, int iLastType, int* piMappings, int iNumMappings);

    static void LoadSettings(void);
    static void SaveSettings(void);
    static void BackupSettings(void);
};

#endif
