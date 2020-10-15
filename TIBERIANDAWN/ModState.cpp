#include "function.h"

#ifdef MOD_BETA
static const DWORD ModVersion = 0x02060000;
static const char* SettingsRegPath = "SOFTWARE\\Electronic Arts\\Command & Conquer Remastered Collection\\Mod\\2254499142";

static const char* StartupMessage = "Tiberian Dawn Cheat Mod (BETA) started.\n";
#else
static const DWORD ModVersion = 0x03000000;
static const char* SettingsRegPath = "SOFTWARE\\Electronic Arts\\Command & Conquer Remastered Collection\\Mod\\2236325862";

static const char* StartupMessage = "Tiberian Dawn Cheat Mod started.\n";
#endif



constexpr auto MaxUnitLevel = 99;
constexpr auto MaxUnitScenario = 99;

DWordSetting<SettingInfo> ModState::s_lastLoadedVersion("LastLoadedVersion", { "Last loaded version" }, ModVersion);

BooleanSetting<SettingInfo> ModState::s_isKeyboardHook("EnableKeyboardHook", { "Keyboard hook" }, true);
BooleanSetting<SettingInfo> ModState::s_isNoDamage("EnableNoDamage", { "No damage mode" }, false);
BooleanSetting<SettingInfo> ModState::s_needHealing("EnableNoDamage", { "No damage mode" }, false, false, true, false);
BooleanSetting<SettingInfo> ModState::s_isUnlockBuildOptions("EnableUnlockBuildOptions", { "Unlock build mode" }, false);
BooleanSetting<SettingInfo> ModState::s_needUnlockBuildOptions("EnableUnlockBuildOptions", { "Unlock build mode" }, false, false, true, false);
BooleanSetting<SettingInfo> ModState::s_isInstantBuild("EnableInstantBuild", { "Instant build mode" }, false);
BooleanSetting<SettingInfo> ModState::s_isInstantSuperweapons("EnableInstantSuperweapons", { "Instant superweapons mode" }, false);
BooleanSetting<SettingInfo> ModState::s_isDismissShroud("EnableDismissShroud", { "Dismiss shroud mode" }, false);
BooleanSetting<SettingInfo> ModState::s_isUnlimitedAmmo("EnableUnlimitedAmmo", { "Unlimited ammo for aircrafts" }, false);

const BooleanSetting<SettingInfo>* ModState::s_booleanSettings[] =
{
    &ModState::s_isKeyboardHook,
    &ModState::s_isNoDamage,
    &ModState::s_isUnlockBuildOptions,
    &ModState::s_isInstantBuild,
    &ModState::s_isInstantSuperweapons,
    &ModState::s_isDismissShroud,
    &ModState::s_isUnlimitedAmmo,
};

DWordSetting<SettingInfo> ModState::s_harvesterBoost("HarvesterBoost", { "Harvester boost" }, 100);
DWordSetting<SettingInfo> ModState::s_movementBoost("MovementBoost", { "Movement boost" }, 10);

DWordSetting<SettingInfo> ModState::s_tiberiumGrowthMultiplier("TiberiumGrowthMultiplier", { "Tiberium growth multiplier" }, 1);

DWordSetting<SettingInfo> ModState::s_initialCreditBoost("InitialCreditBoost", { "Initial credit boost" }, 0);
DWordSetting<SettingInfo> ModState::s_initialPowerBoost("InitialPowerBoost", { "Initial power boost" }, 0);

BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keyHelpKeySetting(
    "KeyHelp",
    { "Show help" },
    { VK_OEM_2 | KEYSTATE_ALT | KEYSTATE_CTRL, FALSE, 1, 0, WM_USER + 1 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keyToggleNoDamage(
    "KeyToggleNoDamage",
    { "Toggle no damage" },
    { VK_N | KEYSTATE_ALT | KEYSTATE_CTRL, FALSE, 1, 0, WM_USER + 2 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keyToggleUnlockBuildOptions(
    "KeyToggleUnlockBuildOptions",
    { "Enable unlock build mode (cannot disable)" },
    { VK_B | KEYSTATE_ALT | KEYSTATE_CTRL, FALSE, 1, 0, WM_USER + 3 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keyToggleInstantBuild(
    "KeyToggleInstantBuild",
    { "Toggle instant build for buildings and units" },
    { VK_I | KEYSTATE_ALT | KEYSTATE_CTRL, FALSE, 1, 0, WM_USER + 4 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keyToggleInstantSuperweapons(
    "KeyToggleInstantSuperweapons",
    { "Toggle instant superweapon charging" },
    { VK_P | KEYSTATE_ALT | KEYSTATE_CTRL, FALSE, 1, 0, WM_USER + 5 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keyToggleDismissShroud(
    "KeyToggleDismissShroud",
    { "Toggle dismiss shroud" },
    { VK_H | KEYSTATE_ALT | KEYSTATE_CTRL, FALSE, 1, 0, WM_USER + 6 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keyToggleUnlimitedAmmo(
    "KeyToggleUnlimitedAmmo",
    { "Toggle unlimited ammo for aircrafts" },
    { VK_U | KEYSTATE_ALT | KEYSTATE_CTRL, FALSE, 1, 0, WM_USER + 7 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keyCreditBoost(
    "KeyCreditBoost",
    { "Boost credits" },
    { VK_M | KEYSTATE_ALT | KEYSTATE_CTRL, FALSE, 1, 0, WM_USER + 8 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keyPowerBoost(
    "KeyPowerBoost",
    { "Boost power" },
    { VK_O | KEYSTATE_ALT | KEYSTATE_CTRL, FALSE, 1, 0, WM_USER + 9 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keyIncreaseMovementBoost(
    "KeyIncreaseMovementBoost",
    { "Increase movement boost by 50% of normal" },
    { VK_OEM_6 | KEYSTATE_ALT | KEYSTATE_CTRL, FALSE, 1, 0, WM_USER + 10 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keyDecreaseMovementBoost(
    "KeyDecreaseMovementBoost",
    { "Decrease movement boost by 50% of normal" },
    { VK_OEM_4 | KEYSTATE_ALT | KEYSTATE_CTRL, FALSE, 1, 0, WM_USER + 11 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keyIncreaseTiberiumGrowth(
    "KeyIncreaseTiberiumGrowth",
    { "Increase Tiberium growth factor" },
    { VK_OEM_PERIOD | KEYSTATE_ALT | KEYSTATE_CTRL, FALSE, 1, 0, WM_USER + 12 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keyDecreaseTiberiumGrowth(
    "KeyDecreaseTiberiumGrowth",
    { "Decrease Tiberium growth factor" },
    { VK_OEM_COMMA | KEYSTATE_ALT | KEYSTATE_CTRL, FALSE, 1, 0, WM_USER + 13 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keyDecreaseHarvesterBoost(
    "KeyIncreaseHarvesterBoost",
    { "Increase harvester load by 50% of normal" },
    { VK_OEM_PLUS | KEYSTATE_ALT | KEYSTATE_CTRL, FALSE, 1, 0, WM_USER + 14 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keyIncreaseHarvesterBoost(
    "KeyDecreaseHarvesterBoost",
    { "Decrease harvester load by 50% of normal" },
    { VK_OEM_MINUS | KEYSTATE_ALT | KEYSTATE_CTRL, FALSE, 1, 0, WM_USER + 15 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keySaveSettings(
    "KeySaveSettings",
    { "Save current settings" },
    { VK_S | KEYSTATE_ALT | KEYSTATE_CTRL, FALSE, 1, 0, WM_USER + 16 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keySpawnInfantry(
    "KeySpawnInfantry",
    { "Spawn infantry" },
    { VK_I | KEYSTATE_ALT | KEYSTATE_SHIFT, TRUE, 3, 0, WM_USER + 17 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keySpawnVehicle(
    "KeySpawnVehicle",
    { "Spawn vehicle" },
    { VK_V | KEYSTATE_ALT | KEYSTATE_SHIFT, TRUE, 3, 0, WM_USER + 18 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keySpawnAircraft(
    "KeySpawnAircraft",
    { "Spawn aircraft" },
    { VK_A | KEYSTATE_ALT | KEYSTATE_SHIFT, TRUE, 3, 0, WM_USER + 19 });
BinarySetting<KeyConfiguration, SettingInfo> ModState::s_keyCapture(
    "KeyCapture",
    { "Capture units and buildings" },
    { VK_C | KEYSTATE_ALT | KEYSTATE_SHIFT, TRUE, 2, 0, WM_USER + 20 });

BinarySetting<KeyConfiguration, SettingInfo>* ModState::s_keyBindings[] =
{
    &ModState::s_keyHelpKeySetting,
    &ModState::s_keyToggleNoDamage,
    &ModState::s_keyToggleUnlockBuildOptions,
    &ModState::s_keyToggleInstantBuild,
    &ModState::s_keyToggleInstantSuperweapons,
    &ModState::s_keyToggleDismissShroud,
    &ModState::s_keyToggleUnlimitedAmmo,
    &ModState::s_keyCreditBoost,
    &ModState::s_keyPowerBoost,
    &ModState::s_keyIncreaseMovementBoost,
    &ModState::s_keyDecreaseMovementBoost,
    &ModState::s_keyIncreaseTiberiumGrowth,
    &ModState::s_keyDecreaseTiberiumGrowth,
    &ModState::s_keyDecreaseHarvesterBoost,
    &ModState::s_keyIncreaseHarvesterBoost,
    &ModState::s_keySaveSettings,
    &ModState::s_keySpawnInfantry,
    &ModState::s_keySpawnVehicle,
    &ModState::s_keySpawnAircraft,
    &ModState::s_keyCapture,
};

Settings<SettingInfo> ModState::s_settings;

volatile long ModState::s_creditBoostAmount = 0;
volatile long ModState::s_powerBoostAmount = 0;

HookConfiguration ModState::s_hookConfiguration = { 0 };

char ModState::s_modifierKeyNames[3][32] = { 0 };
char ModState::s_helpMessages[KEYHOOK_MAXHOOKKEYS + 2][MaxModMessageLength] = { 0 };

bool ModState::s_needSaveSettings = false;

InfantryType ModState::s_spawnInfantryType = INFANTRY_NONE;
UnitType ModState::s_spawnUnitType = UNIT_NONE;
AircraftType ModState::s_spawnAircraftType = AIRCRAFT_NONE;

InfantryType ModState::s_allowedInfantryTypes[INFANTRY_COUNT] = { INFANTRY_NONE };
UnitType ModState::s_allowedUnitTypes[UNIT_COUNT] = { UNIT_NONE };
AircraftType ModState::s_allowedAircraftTypes[AIRCRAFT_COUNT] = { AIRCRAFT_NONE };

HousesType ModState::s_captureHouseType = HOUSE_NONE;

int ModState::s_messageSkipFrames = 2;
char ModState::s_modMessageBuffers[MaxModMessages][MaxModMessageLength] = { 0 };
ModMessage ModState::s_modMessages[MaxModMessages] = { 0 };
int ModState::s_modMessageReadIndex = 0;
int ModState::s_modMessageWriteIndex = 0;
CRITICAL_SECTION ModState::s_modMessageCritSec = { 0 };


void ModState::Initialize(void)
{
    InitializeCriticalSection(&s_modMessageCritSec);
    
    for (int index = 0; index < ARRAYSIZE(s_modMessages); index++)
    {
        s_modMessages[index].szMessage = s_modMessageBuffers[index];
        s_modMessages[index].iTimeout = 0;
    }

    ModState::AddModMessage(StartupMessage);

    ModState::s_settings.Add(s_lastLoadedVersion);

    ModState::s_settings.Add(s_isKeyboardHook);
    ModState::s_settings.Add(s_isNoDamage);
    ModState::s_settings.Add(s_needHealing);
    ModState::s_settings.Add(s_isUnlockBuildOptions);
    ModState::s_settings.Add(s_needUnlockBuildOptions);
    ModState::s_settings.Add(s_isInstantBuild);
    ModState::s_settings.Add(s_isInstantSuperweapons);
    ModState::s_settings.Add(s_isDismissShroud);
    ModState::s_settings.Add(s_isUnlimitedAmmo);

    ModState::s_settings.Add(s_harvesterBoost);
    ModState::s_settings.Add(s_movementBoost);

    ModState::s_settings.Add(s_tiberiumGrowthMultiplier);

    ModState::s_settings.Add(s_keyHelpKeySetting);
    ModState::s_settings.Add(s_keyToggleNoDamage);
    ModState::s_settings.Add(s_keyToggleUnlockBuildOptions);
    ModState::s_settings.Add(s_keyToggleInstantBuild);
    ModState::s_settings.Add(s_keyToggleInstantSuperweapons);
    ModState::s_settings.Add(s_keyToggleDismissShroud);
    ModState::s_settings.Add(s_keyToggleUnlimitedAmmo);
    ModState::s_settings.Add(s_keyCreditBoost);
    ModState::s_settings.Add(s_keyPowerBoost);
    ModState::s_settings.Add(s_keyIncreaseMovementBoost);
    ModState::s_settings.Add(s_keyDecreaseMovementBoost);
    ModState::s_settings.Add(s_keyIncreaseTiberiumGrowth);
    ModState::s_settings.Add(s_keyDecreaseTiberiumGrowth);
    ModState::s_settings.Add(s_keyDecreaseHarvesterBoost);
    ModState::s_settings.Add(s_keyIncreaseHarvesterBoost);
    ModState::s_settings.Add(s_keySaveSettings);
    ModState::s_settings.Add(s_keySpawnInfantry);
    ModState::s_settings.Add(s_keySpawnVehicle);
    ModState::s_settings.Add(s_keySpawnAircraft);
    ModState::s_settings.Add(s_keyCapture);

    ModState::LoadSettings();
}


bool ModState::ToggleNoDamage(void)
{
    *s_isNoDamage = !(*s_isNoDamage);
    if (*s_isNoDamage)
    {
        *s_needHealing = true;
    }

    return *s_isNoDamage;
}


bool ModState::ToggleUnlockBuildOptions(void)
{
    if (!(*s_isUnlockBuildOptions))
    {
        *s_isUnlockBuildOptions = !(*s_isUnlockBuildOptions);
        *s_needUnlockBuildOptions = true;
    }

    return *s_isUnlockBuildOptions;
}

bool ModState::ToggleInstantBuild(void)
{
    *s_isInstantBuild = !(*s_isInstantBuild);
    return *s_isInstantBuild;
}

bool ModState::ToggleInstantSuperweapons(void)
{
    *s_isInstantSuperweapons = !(*s_isInstantSuperweapons);
    return *s_isInstantSuperweapons;
}

bool ModState::ToggleDismissShroud(void)
{
    *s_isDismissShroud = !(*s_isDismissShroud);
    return *s_isDismissShroud;
}

bool ModState::ToggleUnlimitedAmmo(void)
{
    *s_isUnlimitedAmmo = !(*s_isUnlimitedAmmo);
    return *s_isUnlimitedAmmo;
}

bool ModState::IncreaseHarvesterBoost(void)
{
    if (*s_harvesterBoost < 150)
    {
        *s_harvesterBoost += 5;
        return true;
    }

    return false;
}

bool ModState::DecreaseHarvesterBoost(void)
{
    if (*s_harvesterBoost > 10)
    {
        *s_harvesterBoost -= 5;
        return true;
    }

    return false;
}

bool ModState::IncreaseMovementBoost(void)
{
    if (*s_movementBoost < 50)
    {
        *s_movementBoost += 5;
        return true;
    }

    return false;
}

bool ModState::DecreaseMovementBoost(void)
{
    if (*s_movementBoost > 5)
    {
        *s_movementBoost -= 5;
        return true;
    }

    return false;
}

bool ModState::IncreaseTiberiumGrowthMultiplier(void)
{
    if (*s_tiberiumGrowthMultiplier < 50)
    {
        (*s_tiberiumGrowthMultiplier)++;
        return true;
    }

    return false;
}

bool ModState::DecreaseTiberiumGrowthMultiplier(void)
{
    if (*s_tiberiumGrowthMultiplier > 1)
    {
        (*s_tiberiumGrowthMultiplier)--;
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

        for (int index = 0; index < ARRAYSIZE(s_keyBindings); index++)
        {
            KeyConfiguration& key = *(*s_keyBindings[index]);
            DWORD dwPrimaryKey = key.dwPrimaryKey;
            DWORD dwBaseKey = dwPrimaryKey & 0xFF;
            UINT uiScanCode = MapVirtualKey(dwBaseKey, MAPVK_VK_TO_VSC);

            GetKeyNameText((uiScanCode << 16) | (1 << 25), primaryKeyName, ARRAYSIZE(primaryKeyName));

            bool isAlt = (dwPrimaryKey & KEYSTATE_ALT) != 0;
            bool isCtrl = (dwPrimaryKey & KEYSTATE_CTRL) != 0;
            bool isShift = (dwPrimaryKey & KEYSTATE_SHIFT) != 0;

            auto pMessage = s_helpMessages[index + 2];

            sprintf_s(pMessage, MaxModMessageLength, "%s: %s%s%s%s%s%s%s",
                s_keyBindings[index]->GetPropertyBag()._szDescription,
                isCtrl ? s_modifierKeyNames[1] : "",
                isCtrl ? "+" : "",
                isAlt ? s_modifierKeyNames[0] : "",
                isAlt ? "+" : "",
                isShift ? s_modifierKeyNames[2] : "",
                isShift ? "+" : "",
                primaryKeyName);

            if (key.bIsChorded && (key.dwMaxKeys > 1))
            {
                DWORD dwFillerKeys = key.dwMaxKeys - ((key.dwEndKey != 0) ? 2 : 1);
                for (DWORD dwIndex = 0; dwIndex < dwFillerKeys; dwIndex++)
                {
                    strcat_s(pMessage, MaxModMessageLength, ", <key>");
                }

                if (key.dwEndKey != 0)
                {
                    strcat_s(pMessage, MaxModMessageLength, ", ");

                    dwBaseKey = key.dwEndKey & 0xFF;
                    uiScanCode = MapVirtualKey(dwBaseKey, MAPVK_VK_TO_VSC);

                    GetKeyNameText((uiScanCode << 16) | (1 << 25), endKeyName, ARRAYSIZE(endKeyName));

                    isAlt = (key.dwEndKey & KEYSTATE_ALT) != 0;
                    isCtrl = (key.dwEndKey & KEYSTATE_CTRL) != 0;
                    isShift = (key.dwEndKey & KEYSTATE_SHIFT) != 0;

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

    SaveSettings();
}

InfantryType ModState::SetSpawnInfantryFromKeyData(HookKeyData& hkdData)
{
    if (hkdData.dwEntries != 3)
    {
        return INFANTRY_NONE;
    }

    if (s_spawnInfantryType != INFANTRY_NONE)
    {
        return INFANTRY_NONE;
    }

    if (s_allowedInfantryTypes[0] == INFANTRY_NONE)
    {
        int fillIndex = 0;

        for (InfantryType index = INFANTRY_FIRST; index < INFANTRY_COUNT; index++)
        {
            InfantryTypeClass const & typeClass = InfantryTypeClass::As_Reference(index);

            if (!IsSpawnable(&typeClass))
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

    s_spawnInfantryType = (1 <= index) && (index <= INFANTRY_COUNT) && (s_allowedInfantryTypes[index - 1] != INFANTRY_NONE) ?
        s_allowedInfantryTypes[index - 1] :
        INFANTRY_COUNT;

    return s_spawnInfantryType;
}

UnitType ModState::SetSpawnUnitFromKeyData(HookKeyData& hkdData)
{
    if (hkdData.dwEntries != 3)
    {
        return UNIT_NONE;
    }

    if (s_spawnUnitType != UNIT_NONE)
    {
        return UNIT_NONE;
    }

    if (s_allowedUnitTypes[0] == UNIT_NONE)
    {
        int fillIndex = 0;

        for (UnitType index = UNIT_FIRST; index < UNIT_COUNT; index++)
        {
            UnitTypeClass const & typeClass = UnitTypeClass::As_Reference(index);

            if (!IsSpawnable(&typeClass) && (typeClass.Type != UNIT_VICE))
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

    s_spawnUnitType = (1 <= index) && (index <= UNIT_COUNT) && (s_allowedUnitTypes[index - 1] != UNIT_NONE) ?
        s_allowedUnitTypes[index - 1] :
        UNIT_COUNT;

    return s_spawnUnitType;
}

AircraftType ModState::SetSpawnAircraftFromKeyData(HookKeyData& hkdData)
{
    if (hkdData.dwEntries != 3)
    {
        return AIRCRAFT_NONE;
    }

    if (s_spawnAircraftType != AIRCRAFT_NONE)
    {
        return AIRCRAFT_NONE;
    }

    if (s_allowedAircraftTypes[0] == AIRCRAFT_NONE)
    {
        int fillIndex = 0;

        for (AircraftType index = AIRCRAFT_FIRST; index < AIRCRAFT_COUNT; index++)
        {
            AircraftTypeClass const & typeClass = AircraftTypeClass::As_Reference(index);

            if (!IsSpawnable(&typeClass))
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

    s_spawnAircraftType = (1 <= index) && (index <= AIRCRAFT_COUNT) && (s_allowedAircraftTypes[index - 1] != AIRCRAFT_NONE) ?
        s_allowedAircraftTypes[index - 1] :
        AIRCRAFT_COUNT;

    return s_spawnAircraftType;
}

HousesType ModState::SetCaptureHouseFromKeyData(HookKeyData& hkdData)
{
    if (hkdData.dwEntries != 2)
    {
        return HOUSE_NONE;
    }

    if (s_captureHouseType != HOUSE_NONE)
    {
        return HOUSE_NONE;
    }

    int index = GetIndexFromKeyData(hkdData);
    if (index == -1)
    {
        index = (PlayerPtr) ? PlayerPtr->Class->House : HOUSE_NONE;
    }
    else
    {
        index--;
    }

    if (index != -1)
    {
        bool bFound = false;

        for (int houseIndex = 0; houseIndex < Houses.Count(); houseIndex++)
        {
            const HouseClass* pHouse = Houses.Ptr(houseIndex);
            if (pHouse->Class->House == index)
            {
                bFound = true;
                break;
            }
        }

        if (!bFound)
        {
            index = HOUSE_NONE;
        }
    }

    s_captureHouseType = (HousesType)index;

    return (HousesType)(index);
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


bool ModState::IsSpawnable(const TechnoTypeClass* type)
{
    return (type->Level <= MaxUnitLevel) &&
           (type->Scenario <= MaxUnitScenario) &&
           ((type->Pre != STRUCTF_NONE) || !(type->IsInsignificant)) &&
           (type->IsSelectable);
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

void ModState::LoadSettings(void)
{
    char buffer[MaxModMessageLength] = { 0 };

    HKEY hkSettings;
    LSTATUS result = ::RegOpenKeyEx(HKEY_CURRENT_USER, SettingsRegPath, 0, KEY_READ | KEY_WRITE, &hkSettings);

    if (result == ERROR_FILE_NOT_FOUND)
    {
        SaveSettings();

        result = ::RegOpenKeyEx(HKEY_CURRENT_USER, SettingsRegPath, 0, KEY_READ | KEY_WRITE, &hkSettings);
    }

    if (result != ERROR_SUCCESS)
    {
        AddModMessage("Failed to access settings, using defaults.");
        return;
    }

    if (!ModState::s_settings.ReadSettings(hkSettings))
    {
        for (int index = 0; index < s_settings.Count(); index++)
        {
            SettingBase<SettingInfo>& setting = s_settings[index];

            if (setting.IsReadError())
            {
                sprintf_s(buffer, "Failed to read setting: %s (%s)", setting.GetValueName(), setting.GetPropertyBag()._szDescription);
                AddModMessage(buffer);
            }
        }
    }

    if (s_lastLoadedVersion.IsChanged())
    {
        sprintf_s(buffer, "Mod updated to %d.%d.%d.%d from %d.%d.%d.%d",
            (ModVersion >> 24) & 0xFF,
            (ModVersion >> 16) & 0xFF,
            (ModVersion >>  8) & 0xFF,
            (ModVersion      ) & 0xFF,
            (*s_lastLoadedVersion >> 24) & 0xFF,
            (*s_lastLoadedVersion >> 16) & 0xFF,
            (*s_lastLoadedVersion >>  8) & 0xFF,
            (*s_lastLoadedVersion      ) & 0xFF);
        AddModMessage(buffer);

        *s_lastLoadedVersion = ModVersion;
        s_lastLoadedVersion.WriteValue(hkSettings);
    }

    for (int index = 0; index < ARRAYSIZE(s_booleanSettings); index++)
    {
        auto setting = s_booleanSettings[index];

        if (setting->IsChanged())
        {
            sprintf_s(buffer, "%s: %s", setting->GetPropertyBag()._szDescription, *(*setting) ? "enabled" : "disabled");
            AddModMessage(buffer);
        }
    }

    if (s_harvesterBoost.IsChanged())
    {
        *s_harvesterBoost = MAX(10UL, MIN(*s_harvesterBoost, 150UL));
        if (*s_harvesterBoost != s_harvesterBoost.GetOriginal())
        {
            sprintf_s(buffer, "Harvester load: %.0f%% of normal", GetHarvestorBoost() * 100.0f);
            AddModMessage(buffer);
        }
    }

    if (s_movementBoost.IsChanged())
    {
        *s_movementBoost = MAX(5UL, MIN(*s_movementBoost, 50UL));
        if (*s_movementBoost != s_movementBoost.GetOriginal())
        {
            sprintf_s(buffer, "Movement boost: %.0f%%", ModState::GetMovementBoost() * 100.0f);
            ModState::AddModMessage(buffer);
        }
    }

    if (s_tiberiumGrowthMultiplier.IsChanged())
    {
        *s_tiberiumGrowthMultiplier = MAX(1UL, MIN(*s_tiberiumGrowthMultiplier, 50UL));
        if (*s_tiberiumGrowthMultiplier != s_tiberiumGrowthMultiplier.GetOriginal())
        {
            sprintf_s(buffer, "Tiberium growth multiplier: %d", ModState::GetTiberiumGrowthMultiplier());
            ModState::AddModMessage(buffer);
        }
    }

    if (s_initialCreditBoost.IsChanged())
    {
        s_creditBoostAmount = MIN(*s_initialCreditBoost, (DWORD)MAXLONG);
        if (s_creditBoostAmount > 0)
        {
            ModState::AddModMessage("Credits boosted");
        }
    }

    if (s_initialPowerBoost.IsChanged())
    {
        s_powerBoostAmount = MIN(*s_initialPowerBoost, (DWORD)MAXLONG);
        if (s_powerBoostAmount > 0)
        {
            ModState::AddModMessage("Power boosted");
        }
    }

    s_hookConfiguration.dwSize = sizeof(HookConfiguration);
    s_hookConfiguration.dwEntries = ARRAYSIZE(s_keyBindings);

    for (int index = 0; index < ARRAYSIZE(s_keyBindings); index++)
    {
        (*(*s_keyBindings[index])).uMessage = s_keyBindings[index]->GetOriginal().uMessage;
        s_hookConfiguration.kcEntries[index] = *(*s_keyBindings[index]);
    }

    RegCloseKey(hkSettings);
}

void ModState::SaveSettings(void)
{
    char buffer[MaxModMessageLength] = { 0 };

    HKEY hkSettings;
    LSTATUS result = ::RegCreateKeyEx(HKEY_CURRENT_USER, SettingsRegPath, 0, NULL, 0, KEY_READ | KEY_WRITE, NULL, &hkSettings, NULL);

    if (result != ERROR_SUCCESS)
    {
        return;
    }

    if (!s_settings.WriteSettings(hkSettings))
    {
        for (int index = 0; index < s_settings.Count(); index++)
        {
            SettingBase<SettingInfo>& setting = s_settings[index];
            
            if (setting.IsWriteError())
            {
                sprintf_s(buffer, "Failed to write setting: %s (%s)", setting.GetValueName(), setting.GetPropertyBag()._szDescription);
                AddModMessage(buffer);
            }
        }
    }

    ::RegCloseKey(hkSettings);
    return;
}