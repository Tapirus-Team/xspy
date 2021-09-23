#pragma once
#define DIRECTINPUT_VERSION DIRECTINPUT_HEADER_VERSION
#include <initguid.h>
#include <dinput.h>


#ifndef _DECL_DLLMAIN
EXTERN_C BOOL APIENTRY DllMain(
    _In_ HMODULE Module,
    _In_ DWORD  Reason,
    _In_opt_ LPVOID Reserved
);
#endif

namespace Hook
{
    DetourDefineMacro(BOOL, APIENTRY, DllMain)(
        _In_ HMODULE Module,
        _In_ DWORD  Reason,
        _In_opt_ LPVOID Reserved);
}

class DirectInputSpy : public base::Singleton<DirectInputSpy>
{
public:
    static constexpr char MODULE_NAME[] = u8"dinput8.dll";

public:
    HRESULT Attach() noexcept;

private:
    HRESULT HookOnStatic(HMODULE module = nullptr) noexcept;
    HRESULT HookOnStatic2(HMODULE module) noexcept;
    HRESULT HookOnDynamic() noexcept;

    friend BOOL WINAPI Hook::Detour$DllMain(
        _In_ HMODULE Module,
        _In_ DWORD  Reason,
        _In_opt_ LPVOID Reserved);
};

#undef IDirectInputDevice8
using  IDirectInputDevice8 = IDirectInputDevice8W;

EXTERN_C HRESULT STDMETHODCALLTYPE CDIDev_SetDataFormat(
    THIS_ IDirectInputDevice8* This,
    _In_ LPCDIDATAFORMAT DataFormat
);

EXTERN_C HRESULT STDMETHODCALLTYPE CDIDev_SetCooperativeLevel(
    THIS_ IDirectInputDevice8* This,
    _In_ HWND Window,
    _In_ DWORD Flags
);

EXTERN_C HRESULT STDMETHODCALLTYPE CDIDev_SetProperty(
    THIS_ IDirectInputDevice8* This,
    _In_ REFGUID PropGuid,
    _In_ LPCDIPROPHEADER Prop
);

EXTERN_C HRESULT STDMETHODCALLTYPE CDIDev_GetDeviceData(
    THIS_ IDirectInputDevice8* This,
    _In_ DWORD SizeOfData,
    _Out_ LPDIDEVICEOBJECTDATA Data,
    _Inout_ LPDWORD NumberOfElements,
    _In_ DWORD Flags
);

EXTERN_C HRESULT STDMETHODCALLTYPE CDIDev_GetDeviceState(
    THIS_ IDirectInputDevice8* This,
    _In_ DWORD SizeOfData,
    _Out_ LPVOID Data
);

namespace Hook
{
    DetourDefineMacro(HRESULT, STDMETHODCALLTYPE, CDIDev_SetDataFormat)(
        THIS_ IDirectInputDevice8* This,
        LPCDIDATAFORMAT DataFormat
        );

    DetourDefineMacro(HRESULT, STDMETHODCALLTYPE, CDIDev_SetCooperativeLevel)(
        THIS_ IDirectInputDevice8* This,
        _In_ HWND Window,
        _In_ DWORD Flags
        );

    DetourDefineMacro(HRESULT, STDMETHODCALLTYPE, CDIDev_SetProperty)(
        THIS_ IDirectInputDevice8* This,
        _In_ REFGUID PropGuid,
        _In_ LPCDIPROPHEADER Prop
        );

    DetourDefineMacro(HRESULT, STDMETHODCALLTYPE, CDIDev_GetDeviceData)(
        THIS_ IDirectInputDevice8* This,
        _In_ DWORD SizeOfData,
        _Out_ LPDIDEVICEOBJECTDATA Data,
        _Inout_ LPDWORD NumberOfElements,
        _In_ DWORD Flags
        );

    DetourDefineMacro(HRESULT, STDMETHODCALLTYPE, CDIDev_GetDeviceState)(
        THIS_ IDirectInputDevice8* This,
        _In_ DWORD SizeOfData,
        _Out_ LPVOID Data
        );
}

namespace Offsets
{
    enum class IDirectInputDevice8 : size_t
    {
        QueryInterface = 0,
        AddRef,
        Release,

        /*** IDirectInputDevice8 methods ***/
        GetCapabilities,
        EnumObjects,
        GetProperty,
        SetProperty,
        Acquire,
        Unacquire,
        GetDeviceState,
        GetDeviceData,
        SetDataFormat,
        SetEventNotification,
        SetCooperativeLevel,
        GetObjectInfo,
        GetDeviceInfo,
        RunControlPanel,
        Initialize,
        CreateEffect,
        EnumEffects,
        GetEffectInfo,
        GetForceFeedbackState,
        SendForceFeedbackCommand,
        EnumCreatedEffectObjects,
        Escape,
        Poll,
        SendDeviceData,
        EnumEffectsInFile,
        WriteEffectToFile,
        BuildActionMap,
        SetActionMap,
        GetImageInfo,
    };
}
