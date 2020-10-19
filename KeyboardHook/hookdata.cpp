#pragma once

#include "pch.h"

#include "hook.h"
#include "hookdata.h"


void HookData::Initialize(void)
{
    memset(_data, 0, sizeof(_data));

    _isConfigured = false;
    _activeConfigurationIndex = -1;
    _writeIndex = 0;
}

void HookData::SetConfiguration(const HookConfiguration* const pConfiguration)
{
    memcpy_s(&_configuration, sizeof(HookConfiguration), pConfiguration, sizeof(HookConfiguration));
    _activeConfigurationIndex = -1;
    _isConfigured = true;
}

void HookData::ClearActiveConfiguration(void)
{
    _activeConfigurationIndex = -1;
}

int HookData::FindConfiguration(DWORD dwKey)
{
    if (!_isConfigured)
    {
        return -1;
    }

    for (DWORD dwIndex = 0; dwIndex < _configuration.dwEntries; dwIndex++)
    {
        KeyConfiguration* pConfiguration = &(_configuration.kcEntries[dwIndex]);

        if (pConfiguration->dwPrimaryKey == dwKey)
        {
            return (int)dwIndex;
        }
    }

    return -1;
}

const KeyConfiguration* HookData::GetActiveConfiguration(void)
{
    if (_activeConfigurationIndex >= 0)
    {
        return &(_configuration.kcEntries[_activeConfigurationIndex]);
    }

    return NULL;
}

int HookData::GetActiveConfigurationIndex(void)
{
    return _activeConfigurationIndex;
}

bool HookData::SetActiveConfiguration(int index)
{
    if (!_isConfigured)
    {
        return false;
    }

    if ((0 <= index) && (index < KEYHOOK_MAXHOOKKEYS))
    {
        _activeConfigurationIndex = index;
        return true;
    }

    return false;
}

int HookData::AddKey(DWORD dwKey)
{
    if (_activeConfigurationIndex < 0)
    {
        return ADD_FAILED_NOCONFIG;
    }

    KeyConfiguration* pConfiguration = &(_configuration.kcEntries[_activeConfigurationIndex]);
    HookKeyData* pData = &(_data[_writeIndex]);
    DWORD dwNumEntries = pData->dwEntries;

    if ((pConfiguration->bIsChorded || (dwNumEntries == 0)) &&
        (dwNumEntries < pConfiguration->dwMaxKeys) &&
        (dwNumEntries < KEYHOOK_MAXCHORDLEN))
    {
        pData->dwKeys[dwNumEntries] = dwKey;
        pData->dwEntries = ++dwNumEntries;

        if (!pConfiguration->bIsChorded)
        {
            return ADD_SUCCESS_LIMIT;
        }
        else if ((dwKey == pConfiguration->dwEndKey) && (dwNumEntries > 1))
        {
            return ADD_SUCCESS_ENDKEY;
        }
        else if ((dwNumEntries == pConfiguration->dwMaxKeys) || (dwNumEntries == KEYHOOK_MAXCHORDLEN))
        {
            return ADD_SUCCESS_LIMIT;
        }

        return ADD_SUCCESS;
    }
    else
    {
        return ADD_FAILED_LIMIT;
    }
}

int HookData::FinalizeCurrentData(void)
{
    int previousIndex = _writeIndex++;
    if (_writeIndex >= MaxKeyData)
    {
        _writeIndex = 0;
    }

    HookKeyData* pData = &(_data[_writeIndex]);
    pData->dwEntries = 0;
    memset(pData->dwKeys, 0, sizeof(pData->dwKeys));

    return previousIndex;
}

bool HookData::CopyTo(int index, HookKeyData* const lpKeyData)
{
    if ((0 <= index) && (index < MaxKeyData))
    {
        memcpy_s(lpKeyData, sizeof(HookKeyData), &(_data[index]), sizeof(HookKeyData));
        return true;
    }

    return false;
}