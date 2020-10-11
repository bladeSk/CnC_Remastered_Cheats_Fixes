#pragma once

constexpr auto MaxKeyData = 256;

constexpr auto ADD_SUCCESS = 1;
constexpr auto ADD_SUCCESS_ENDKEY = 2;
constexpr auto ADD_SUCCESS_LIMIT = 3;

constexpr auto ADD_FAILED_NOCONFIG = 0;
constexpr auto ADD_FAILED_LIMIT = -1;

class HookData
{
private:
    HookConfiguration _configuration;
    bool _isConfigured;
    int _activeConfigurationIndex;

    HookKeyData _data[MaxKeyData];
    int _writeIndex;

public:
    void Initialize(void);

    void SetConfiguration(const HookConfiguration* const pConfiguration);

    __inline DWORD GetMessageThreadId(void)
    {
        if (!_isConfigured)
        {
            return 0;
        }

        return _configuration.dwMessageThreadId;
    }

    void ClearActiveConfiguration(void);
    int FindConfiguration(DWORD dwKey);
    const KeyConfiguration* GetActiveConfiguration(void);
    int GetActiveConfigurationIndex(void);
    bool SetActiveConfiguration(int index);

    int AddKey(DWORD dwKey);
    int FinalizeCurrentData(void);

    bool CopyTo(int index, HookKeyData* const lpKeyData);
};