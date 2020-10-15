#ifndef SETTINGS_H
#define SETTINGS_H

template <typename P>
class SettingBase
{
protected:
    const char* const _szValueName;
    bool _bCanRead;
    bool _bCanWrite;

    P _pPropertyBag;

    bool _bIsChanged;
    bool _bIsReadError;
    bool _bIsWriteError;

public:
    SettingBase(__in_z const char* const szValueName, const P& pPropertyBag, bool bCanRead = true, bool bCanWrite = true) :
        _szValueName(szValueName),
        _pPropertyBag(pPropertyBag),
        _bCanRead(bCanRead),
        _bCanWrite(bCanWrite)
    {
    }

    virtual const char* const GetValueName(void) const { return _szValueName; }
    virtual const P& GetPropertyBag(void) const { return _pPropertyBag; }

    virtual bool CanRead(void) const { return _bCanRead; }
    virtual bool CanWrite(void) const { return _bCanWrite; }

    virtual bool IsChanged(void) const { return _bIsChanged; }
    virtual bool IsReadError(void) const { return _bIsReadError; }
    virtual bool IsWriteError(void) const { return _bIsWriteError; }

    virtual bool ReadValue(HKEY hkKey)
    {
        if (!_bCanRead)
        {
            return true;
        }

        return OnReadValue(hkKey);
    }
    
    virtual bool WriteValue(HKEY hkKey)
    {
        if (!_bCanWrite)
        {
            return true;
        }

        return OnWriteValue(hkKey);
    }

protected:
    virtual bool OnReadValue(HKEY hkKey) = 0;
    virtual bool OnWriteValue(HKEY hkKey) = 0;

    virtual bool ReadRawValue(HKEY hkKey, DWORD dwFlag, LPVOID pvData, LPDWORD pcbData)
    {
        _bIsReadError = (::RegGetValue(hkKey, NULL, _szValueName, dwFlag, NULL, pvData, pcbData) != ERROR_SUCCESS);
        if (_bIsReadError)
        {
            DWORD dwResult = ::GetLastError();
            if (dwResult == ERROR_FILE_NOT_FOUND)
            {
                // Not found isn't a read error.
                _bIsReadError = false;
                return false;
            }
        }

        return !_bIsReadError;
    }

    virtual bool WriteRawValue(HKEY hkKey, DWORD dwType, LPCBYTE pbValue, DWORD dwSize)
    {
        _bIsWriteError = (::RegSetValueEx(hkKey, _szValueName, 0, dwType, pbValue, dwSize) != ERROR_SUCCESS);

        if (!_bIsWriteError)
        {
            _bIsChanged = false;
            return true;
        }

        return false;
    }
};

template <typename T, typename P>
class Setting : public SettingBase<P>
{
protected:
    T _tCurrent;
    T _tOriginal;

public:
    Setting(__in_z const char* const szValueName, const P& pPropertyBag, const T& tValue, bool bCanRead = true, bool bCanWrite = true)
        : SettingBase(szValueName, pPropertyBag, bCanRead, bCanWrite)
    {
        _tOriginal = tValue;
        Revert();
    }

    const T& GetCurrent(void) const
    {
        return _tCurrent;
    }

    T& GetCurrent(void)
    {
        return _tCurrent;
    }

    const T& GetOriginal(void) const
    {
        return _tOriginal;
    }

    void Revert(void)
    {
        _tCurrent = _tOriginal;
        _bIsChanged = false;
    }

    const T& operator *(void) const
    {
        return _tCurrent;
    }

    T& operator *(void)
    {
        return _tCurrent;
    }
};

template <typename P>
class BooleanSetting : public Setting<bool, P>
{
protected:
    bool _bIsNegated;

public:
    BooleanSetting(__in_z const char* const szValueName, const P& pPropertyBag, const bool& bValue, bool bIsNegated = false, bool bCanRead = true, bool bCanWrite = true) :
        Setting(szValueName, pPropertyBag, bValue, bCanRead, bCanWrite),
        _bIsNegated(bIsNegated)
    {
    }

protected:
    virtual bool OnReadValue(HKEY hkKey)
    {
        DWORD dwData;
        DWORD dwSize = sizeof(dwData);

        if (ReadRawValue(hkKey, RRF_RT_REG_DWORD, &dwData, &dwSize))
        {
            bool bNewValue = ((dwData != 0) != _bIsNegated);
            _bIsChanged = (_tOriginal != bNewValue);
            _tCurrent = bNewValue;

            return true;
        }

        return false;
    }

    virtual bool OnWriteValue(HKEY hkKey)
    {
        DWORD dwValue = (_tCurrent != _bIsNegated) ? 1 : 0;

        return WriteRawValue(hkKey, REG_DWORD, (LPCBYTE)&dwValue, sizeof(dwValue));
    }
};

template <typename P>
class DWordSetting : public Setting<DWORD, P>
{
public:
    DWordSetting(__in_z const char* const szValueName, const P& pPropertyBag, const DWORD& dwValue, bool bCanRead = true, bool bCanWrite = true) :
        Setting(szValueName, pPropertyBag, dwValue, bCanRead, bCanWrite)
    {
    }

protected:
    virtual bool OnReadValue(HKEY hkKey)
    {
        DWORD dwData;
        DWORD dwSize = sizeof(dwData);

        if (ReadRawValue(hkKey, RRF_RT_REG_DWORD, &dwData, &dwSize))
        {
            _bIsChanged = (_tOriginal != dwData);
            _tCurrent = dwData;

            return true;
        }

        return false;
    }

    virtual bool OnWriteValue(HKEY hkKey)
    {
        DWORD dwValue = _tCurrent;

        return WriteRawValue(hkKey, REG_DWORD, (LPCBYTE)&dwValue, sizeof(dwValue));
    }
};

template <typename T, typename P>
class BinarySetting : public Setting<T, P>
{
public:
    BinarySetting(__in_z const char* const szValueName, const P& pPropertyBag, const T& tValue, bool bCanRead = true, bool bCanWrite = true) :
        Setting(szValueName, pPropertyBag, tValue, bCanRead, bCanWrite)
    {
    }

protected:
    virtual bool OnReadValue(HKEY hkKey)
    {
        BYTE bData[sizeof(T)];
        DWORD dwSize = sizeof(T);

        if (ReadRawValue(hkKey, RRF_RT_REG_BINARY, &bData[0], &dwSize))
        {
            if (dwSize != sizeof(T))
            {
                _bIsReadError = true;
            }
            else
            {
                _bIsChanged = (memcmp(&_tOriginal, &bData[0], sizeof(T)) != 0);
                _tCurrent = *((T*)&bData[0]);
            }
        }

        return !_bIsReadError;
    }

    virtual bool OnWriteValue(HKEY hkKey)
    {
        BYTE bData[sizeof(T)];
        memcpy_s(&bData[0], sizeof(T), &_tCurrent, sizeof(T));

        return WriteRawValue(hkKey, REG_BINARY, &bData[0], sizeof(T));
    }
};

constexpr auto MaxSettings = 256;

template <typename P>
class Settings
{
private:
    SettingBase<P>* _prsSettings[MaxSettings];
    int _iCount;

public:
    Settings(void) :
        _iCount(0)
    {
    }

    bool Add(SettingBase<P>& rsSetting)
    {
        if (_iCount < MaxSettings)
        {
            _prsSettings[_iCount] = &rsSetting;
            _iCount++;
        }

        return false;
    }
    
    int Count(void) const
    {
        return _iCount;
    }

    bool ReadSettings(HKEY hkKey)
    {
        bool bSucceeded = true;

        for (int iIndex = 0; iIndex < _iCount; iIndex++)
        {
            if ((_prsSettings[iIndex] != NULL) && !_prsSettings[iIndex]->ReadValue(hkKey))
            {
                bSucceeded = false;
            }
        }

        return bSucceeded;
    }

    bool WriteSettings(HKEY hkKey)
    {
        bool bSucceeded = true;

        for (int iIndex = 0; iIndex < _iCount; iIndex++)
        {
            if ((_prsSettings[iIndex] != NULL) && !_prsSettings[iIndex]->WriteValue(hkKey))
            {
                bSucceeded = false;
            }
        }

        return bSucceeded;
    }

    SettingBase<P>& operator[](int iIndex) const
    {
        if ((0 <= iIndex) && (iIndex < _iCount))
        {
            return *(_prsSettings[iIndex]);
        }

        return *((SettingBase<P>*)NULL);
    }
};

#endif
