#include "universal.inl"
#include "dinput.h"
#include "vmt.h"

#include <dbghelp.h>
#include <wrl.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dbghelp.lib")

using namespace Microsoft;


static const std::string DIGetDeviceTypeString(DWORD code) noexcept;
static const std::string DIGetDataOffsetString(DWORD code) noexcept;
static const std::string DIGetDataFormatFlagsString(DWORD code) noexcept;
static const std::string DIGetCooperativeLevelFlagsString(DWORD code) noexcept;

std::ostream& operator<<(std::ostream& os, const DIDATAFORMAT& caps)
{
    return os <<
        "{\n" <<
        " \t Flags          =" << DIGetDataFormatFlagsString(caps.dwFlags) << "\n"
        " \t SizeOfObject   =" << caps.dwObjSize  << "\n"
        " \t NumberOfObject =" << caps.dwNumObjs  << "\n"
        " \t SizeOfData     =" << caps.dwDataSize << "\n"
        "}";
}

std::ostream& operator<<(std::ostream& os, const DIDEVICEOBJECTDATA& keystroke)
{
    return os <<
        "{\n" <<
        " \t Offset    =" << DIGetDataOffsetString(keystroke.dwOfs) << "\n"
        " \t Data      =" << std::hex << keystroke.dwData << std::dec << "\n"
        " \t TimeStamp =" << keystroke.dwTimeStamp  << "\n"
        " \t Sequence  =" << keystroke.dwSequence   << "\n"
        " \t AppData   =" << std::hex << keystroke.uAppData << std::dec << "\n"
        "}";
}

std::ostream& operator<<(std::ostream& os, const DIJOYSTATE& state)
{
    return os <<
        "{\n" <<
        " \t       X    =" << state.lX << "\n"
        " \t       Y    =" << state.lY << "\n"
        " \t       Z    =" << state.lZ << "\n"
        " \t      RX    =" << state.lRx << "\n"
        " \t      RY    =" << state.lRy << "\n"
        " \t      RZ    =" << state.lRz << "\n"
        " \t  Slider[0] =" << state.rglSlider[0] << "\n"
        " \t  Slider[1] =" << state.rglSlider[1] << "\n"
        " \t     POV[0] =" << std::hex << state.rgdwPOV[0] << std::dec << "\n"
        " \t     POV[1] =" << std::hex << state.rgdwPOV[1] << std::dec << "\n"
        " \t     POV[2] =" << std::hex << state.rgdwPOV[2] << std::dec << "\n"
        " \t     POV[3] =" << std::hex << state.rgdwPOV[3] << std::dec << "\n"
        " \t Buttons[0] =" << std::hex << (uint32_t)state.rgbButtons[0] << std::dec << "\n"
        " \t Buttons[1] =" << std::hex << (uint32_t)state.rgbButtons[1] << std::dec << "\n"
        " \t Buttons[2] =" << std::hex << (uint32_t)state.rgbButtons[2] << std::dec << "\n"
        " \t Buttons[3] =" << std::hex << (uint32_t)state.rgbButtons[3] << std::dec << "\n"
        " \t Buttons[4] =" << std::hex << (uint32_t)state.rgbButtons[4] << std::dec << "\n"
        " \t Buttons[5] =" << std::hex << (uint32_t)state.rgbButtons[5] << std::dec << "\n"
        " \t Buttons[6] =" << std::hex << (uint32_t)state.rgbButtons[6] << std::dec << "\n"
        " \t Buttons[7] =" << std::hex << (uint32_t)state.rgbButtons[7] << std::dec << "\n"
        " \t Buttons[8] =" << std::hex << (uint32_t)state.rgbButtons[8] << std::dec << "\n"
        " \t Buttons[9] =" << std::hex << (uint32_t)state.rgbButtons[9] << std::dec << "\n"
        "}";
}

std::ostream& operator<<(std::ostream& os, const DIJOYSTATE2& state)
{
    return operator<<(os, *reinterpret_cast<const DIJOYSTATE*>(&state));
}

namespace DIJoyStick
{
    static constexpr auto INVALID_USER_INDEX = -1;
    static std::shared_mutex _UserIndicesMutex;
    static std::unordered_map<const void* /*IDirectInputDevice8*/, int /*UserIndex*/> _UserIndices;

    int GetNextUserIndex()
    {
        static std::atomic_int _UserIndexCounter = 0;
        return _UserIndexCounter++;
    }

    void SetDeviceUserIndex(const void* device, int index)
    {
        std::unique_lock Guard(_UserIndicesMutex);
        _UserIndices[device] = index;
    }

    int GetDeviceUserIndex(const void* device)
    {
        auto Item = _UserIndices.find(device);
        if (Item != _UserIndices.end())
        {
            return Item->second;
        }

        return INVALID_USER_INDEX;
    }
}

namespace Hook
{
    // Call: 1
    DetourMakeMacro(HRESULT, STDMETHODCALLTYPE, CDIDev_SetDataFormat)(
        THIS_ IDirectInputDevice8* This,
        LPCDIDATAFORMAT DataFormat
        )
    {
        HRESULT Result = DetourCallMacro(CDIDev_SetDataFormat)(This, DataFormat);

        DIDEVICEINSTANCE DevInfo{ sizeof(DIDEVICEINSTANCE) };
        if (FAILED(This->GetDeviceInfo(&DevInfo)))
        {
            DIDEVCAPS Caps{ sizeof(DIDEVCAPS) };
            if (SUCCEEDED(This->GetCapabilities(&Caps)))
            {
                DevInfo.dwDevType = Caps.dwDevType;
            }
        }

        if (GET_DIDEVICE_TYPE(DevInfo.dwDevType) == DI8DEVTYPE_GAMEPAD ||
            GET_DIDEVICE_TYPE(DevInfo.dwDevType) == DI8DEVTYPE_JOYSTICK)
        {
            auto UserIndex = DIJoyStick::GetNextUserIndex();
            DIJoyStick::SetDeviceUserIndex(This, UserIndex);

            spdlog::info(__FUNCTION__ "({}): JoyID={}, Device={}, DevType={}, Format={}",
                Result, UserIndex, base::wcstombs(DevInfo.tszInstanceName), DIGetDeviceTypeString(DevInfo.dwDevType), *DataFormat);
        }

        return Result;
    }

    // Call: 2
    DetourMakeMacro(HRESULT, STDMETHODCALLTYPE, CDIDev_SetCooperativeLevel)(
        THIS_ IDirectInputDevice8* This,
        _In_ HWND Window,
        _In_ DWORD Flags
        )
    {
        HRESULT Result = DetourCallMacro(CDIDev_SetCooperativeLevel)(This, Window, Flags);

        auto UserIndex = DIJoyStick::GetDeviceUserIndex(This);
        if (UserIndex != DIJoyStick::INVALID_USER_INDEX)
        {
            spdlog::info(__FUNCTION__ "({}): JoyID={}, Window={}, Flags={}.",
                Result, UserIndex, (void*)Window, DIGetCooperativeLevelFlagsString(Flags));
        }

        return Result;
    }

    // Call: 3
    DetourMakeMacro(HRESULT, STDMETHODCALLTYPE, CDIDev_SetProperty)(
        THIS_ IDirectInputDevice8* This,
        _In_ REFGUID PropGuid,
        _In_ LPCDIPROPHEADER Prop
        )
    {
        HRESULT Result = DetourCallMacro(CDIDev_SetProperty)(This, PropGuid, Prop);

        auto UserIndex = DIJoyStick::GetDeviceUserIndex(This);
        if (UserIndex != DIJoyStick::INVALID_USER_INDEX)
        {
            if (&PropGuid == &DIPROP_BUFFERSIZE && Prop->dwHow == DIPH_DEVICE)
            {
                auto PropDW = reinterpret_cast<const DIPROPDWORD*>(Prop);

                spdlog::info(__FUNCTION__ "({}): JoyID={}, SizeOfData={}.", Result, UserIndex, PropDW->dwData);
            }
        }

        return Result;
    }

    DetourMakeMacro(HRESULT, STDMETHODCALLTYPE, CDIDev_GetDeviceData)(
        THIS_ IDirectInputDevice8* This,
        _In_ DWORD SizeOfData,
        _Out_ LPDIDEVICEOBJECTDATA Data,
        _Inout_ LPDWORD NumberOfElements,
        _In_ DWORD Flags
        )
    {
        HRESULT Result = DetourCallMacro(CDIDev_GetDeviceData)(This, SizeOfData, Data, NumberOfElements, Flags);

        auto UserIndex = DIJoyStick::GetDeviceUserIndex(This);
        if (UserIndex != DIJoyStick::INVALID_USER_INDEX)
        {
            if (SUCCEEDED(Result))
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}, Flags={}, NumberOfElements={}.",
                    Result, UserIndex, (Flags == DIGDD_PEEK ? "DIGDD_PEEK" : "0"), *NumberOfElements);

                for (auto i = 0u; i < *NumberOfElements; ++i)
                {
                    spdlog::info(__FUNCTION__ "(): Keystroke[{}]={}.", i, Data[i]);
                }
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}, Flags={}, NumberOfElements={}.",
                    Result, UserIndex, (Flags == DIGDD_PEEK ? "DIGDD_PEEK" : "0"), *NumberOfElements);
            }
        }

        return Result;
    }

    DetourMakeMacro(HRESULT, STDMETHODCALLTYPE, CDIDev_GetDeviceState)(
        THIS_ IDirectInputDevice8* This,
        _In_ DWORD SizeOfData,
        _Out_ LPVOID Data
        )
    {
        HRESULT Result = DetourCallMacro(CDIDev_GetDeviceState)(This, SizeOfData, Data);

        auto UserIndex = DIJoyStick::GetDeviceUserIndex(This);
        if (UserIndex != DIJoyStick::INVALID_USER_INDEX)
        {
            if (SUCCEEDED(Result))
            {
                if (SizeOfData == sizeof(DIJOYSTATE))
                {
                    spdlog::info(__FUNCTION__ "({}): JoyID={}, SizeOfData={}, State={}.",
                        Result, UserIndex, SizeOfData, *static_cast<DIJOYSTATE*>(Data));
                }

                if (SizeOfData == sizeof(DIJOYSTATE2))
                {
                    spdlog::info(__FUNCTION__ "({}): JoyID={}, SizeOfData={}, State={}.",
                        Result, UserIndex, SizeOfData, *static_cast<DIJOYSTATE2*>(Data));
                }
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}, SizeOfData={}.", Result, UserIndex, SizeOfData);
            }
        }

        return Result;
    }

    DetourMakeMacro(BOOL, APIENTRY, DllMain)(
        _In_ HMODULE Module,
        _In_ DWORD  Reason,
        _In_opt_ LPVOID Reserved
        )
    {
        BOOL Result = DetourCallMacro(DllMain)(Module, Reason, Reserved);

        if (Result)
        {
            if (Reason == DLL_PROCESS_ATTACH)
            {
                DirectInputSpy::GetInstance().HookOnStatic((HMODULE)Module);
            }
        }

        return Result;
    }
}

static const std::string DIGetDeviceTypeString(DWORD code) noexcept
{
    std::string str;

    switch (GET_DIDEVICE_TYPE(code) & ~DIDEVTYPE_HID)
    {
    default:
        str = std::to_string(code);
        break;

#pragma warning(push)
#pragma warning(disable: 4644)
#define _CASE_(x) case x: str = #x; break
        _CASE_(DI8DEVTYPE_DEVICE);
        _CASE_(DI8DEVTYPE_MOUSE);
        _CASE_(DI8DEVTYPE_KEYBOARD);
        _CASE_(DI8DEVTYPE_JOYSTICK);
        _CASE_(DI8DEVTYPE_GAMEPAD);
        _CASE_(DI8DEVTYPE_DRIVING);
        _CASE_(DI8DEVTYPE_FLIGHT);
        _CASE_(DI8DEVTYPE_1STPERSON);
        _CASE_(DI8DEVTYPE_DEVICECTRL);
        _CASE_(DI8DEVTYPE_SCREENPOINTER);
        _CASE_(DI8DEVTYPE_REMOTE);
        _CASE_(DI8DEVTYPE_SUPPLEMENTAL);
#undef _CASE_
#pragma warning(pop)
    }

    return str;
}

static const std::string DIGetDataOffsetString(DWORD code) noexcept
{
    std::string str;

    switch (code)
    {
    default:
        str = std::to_string(code);
        break;

#pragma warning(push)
#pragma warning(disable: 4644)
#define _CASE_(x) case x: str = #x; break
        _CASE_(DIJOFS_X);
        _CASE_(DIJOFS_Y);
        _CASE_(DIJOFS_Z);
        _CASE_(DIJOFS_RX);
        _CASE_(DIJOFS_RY);
        _CASE_(DIJOFS_RZ);
        _CASE_(DIJOFS_SLIDER(0));
        _CASE_(DIJOFS_SLIDER(1));
        _CASE_(DIJOFS_POV(0));
        _CASE_(DIJOFS_POV(1));
        _CASE_(DIJOFS_POV(2));
        _CASE_(DIJOFS_POV(3));
        _CASE_(DIJOFS_BUTTON(0));
        _CASE_(DIJOFS_BUTTON(1));
        _CASE_(DIJOFS_BUTTON(2));
        _CASE_(DIJOFS_BUTTON(3));
        _CASE_(DIJOFS_BUTTON(4));
        _CASE_(DIJOFS_BUTTON(5));
        _CASE_(DIJOFS_BUTTON(6));
        _CASE_(DIJOFS_BUTTON(7));
        _CASE_(DIJOFS_BUTTON(8));
        _CASE_(DIJOFS_BUTTON(9));
        _CASE_(DIJOFS_BUTTON(10));
        _CASE_(DIJOFS_BUTTON(11));
        _CASE_(DIJOFS_BUTTON(12));
        _CASE_(DIJOFS_BUTTON(13));
        _CASE_(DIJOFS_BUTTON(14));
        _CASE_(DIJOFS_BUTTON(15));
        _CASE_(DIJOFS_BUTTON(16));
        _CASE_(DIJOFS_BUTTON(17));
        _CASE_(DIJOFS_BUTTON(18));
        _CASE_(DIJOFS_BUTTON(19));
        _CASE_(DIJOFS_BUTTON(20));
        _CASE_(DIJOFS_BUTTON(21));
        _CASE_(DIJOFS_BUTTON(22));
        _CASE_(DIJOFS_BUTTON(23));
        _CASE_(DIJOFS_BUTTON(24));
        _CASE_(DIJOFS_BUTTON(25));
        _CASE_(DIJOFS_BUTTON(26));
        _CASE_(DIJOFS_BUTTON(27));
        _CASE_(DIJOFS_BUTTON(28));
        _CASE_(DIJOFS_BUTTON(29));
        _CASE_(DIJOFS_BUTTON(30));
        _CASE_(DIJOFS_BUTTON(31));
#undef _CASE_
#pragma warning(pop)
    }

    return str;
}

static const std::string DIGetDataFormatFlagsString(DWORD code) noexcept
{
    std::string str;

    switch (code)
    {
    default:
        str = std::to_string(code);
        break;

#define _CASE_(x) case x: str = #x; break
        _CASE_(DIDF_ABSAXIS);
        _CASE_(DIDF_RELAXIS);
#undef _CASE_
    }

    return str;
}

static const std::string DIGetCooperativeLevelFlagsString(DWORD code) noexcept
{
    std::string str;

#define _APPEND_(x) if (code & x) str.append(#x).append(" | ")
    _APPEND_(DISCL_EXCLUSIVE);
    _APPEND_(DISCL_NONEXCLUSIVE);
    _APPEND_(DISCL_FOREGROUND);
    _APPEND_(DISCL_BACKGROUND);
    _APPEND_(DISCL_NOWINKEY);
#undef _APPEND_

    if (!str.empty())
    {
        str.resize(str.size() - 3);
    }

    return str;
}

HRESULT DirectInputSpy::Attach() noexcept
{
    return SUCCEEDED(HookOnStatic()) ? S_OK : HookOnDynamic();
}

HRESULT DirectInputSpy::HookOnStatic(HMODULE module /*= nullptr*/) noexcept
{
    HRESULT Result = S_OK;

    module = module ? module : GetModuleHandleA(DirectInputSpy::MODULE_NAME);
    if (module == nullptr)
    {
        Result = HRESULT_FROM_WIN32(GetLastError());
        return Result;
    }

    spdlog::info(__FUNCTION__ "(): Called.");

    auto _DirectInput8Create = (decltype(::DirectInput8Create)*)base::dlsym(module, "DirectInput8Create");
    if (_DirectInput8Create == nullptr)
    {
        Result = HRESULT_FROM_WIN32(GetLastError());
        return Result;
    }

    WRL::ComPtr<IDirectInput8>       DirectInput;
    WRL::ComPtr<IDirectInputDevice8> DirectInputDevice;

    Result = _DirectInput8Create(HINST_THISCOMPONENT, DIRECTINPUT_VERSION, IID_IDirectInput8, &DirectInput, nullptr);
    if (FAILED(Result))
    {
        return Result;
    }

    Result = DirectInput->CreateDevice(GUID_SysMouse, &DirectInputDevice, nullptr);
    if (FAILED(Result))
    {
        return Result;
    }

    auto VMTBase = GetVMTBase(DirectInputDevice.Get());
    auto _CDIDev_SetDataFormat        = VMTBase[base::stdext::to_underlying(Offsets::IDirectInputDevice8::SetDataFormat)];
    auto _CDIDev_SetCooperativeLevel  = VMTBase[base::stdext::to_underlying(Offsets::IDirectInputDevice8::SetCooperativeLevel)];
    auto _CDIDev_SetProperty          = VMTBase[base::stdext::to_underlying(Offsets::IDirectInputDevice8::SetProperty)];
    auto _CDIDev_GetDeviceData        = VMTBase[base::stdext::to_underlying(Offsets::IDirectInputDevice8::GetDeviceData)];
    auto _CDIDev_GetDeviceState       = VMTBase[base::stdext::to_underlying(Offsets::IDirectInputDevice8::GetDeviceState)];

    DetourBegin();
    {
        using namespace Hook;

        DetourAttachMacroAddress(module, CDIDev_SetDataFormat, _CDIDev_SetDataFormat);
        DetourAttachMacroAddress(module, CDIDev_SetCooperativeLevel, _CDIDev_SetCooperativeLevel);
        DetourAttachMacroAddress(module, CDIDev_SetProperty, _CDIDev_SetProperty);
        DetourAttachMacroAddress(module, CDIDev_GetDeviceData, _CDIDev_GetDeviceData);
        DetourAttachMacroAddress(module, CDIDev_GetDeviceState, _CDIDev_GetDeviceState);
    }
    return HRESULT_FROM_WIN32(DetourCommit());
}

HRESULT DirectInputSpy::HookOnStatic2(HMODULE module) noexcept
{
    auto NtHeader = ImageNtHeader(module);
    if (NtHeader == nullptr)
    {
        return HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
    }

    DetourBegin();
    {
        using namespace Hook;

        DetourAttachMacroOffset(module, DllMain, NtHeader->OptionalHeader.AddressOfEntryPoint);
    }
    return HRESULT_FROM_WIN32(DetourCommit());
}

HRESULT DirectInputSpy::HookOnDynamic() noexcept
{
    base::ModuleNotification::GetInstance().RegisterNotificationCallback([](
        _In_ ULONG NotificationReason,
        _In_ PLDR_DLL_NOTIFICATION_DATA NotificationData,
        _In_opt_ PVOID Context)
        {
            if (NotificationReason == LDR_DLL_NOTIFICATION_REASON_UNLOADED)
            {
                return;
            }

            auto This = static_cast<DirectInputSpy*>(Context);
            if (This == nullptr)
            {
                return;
            }

            auto DllBase = NotificationData->Loaded.DllBase;
            auto DllName = std::wstring(
                NotificationData->Loaded.BaseDllName->Buffer,
                NotificationData->Loaded.BaseDllName->Length / sizeof(wchar_t));

            std::transform(DllName.begin(), DllName.end(), DllName.begin(), [](wint_t c) { return (wchar_t)std::towlower(c); });

            if (DllName == base::mbstowcs(DirectInputSpy::MODULE_NAME))
            {
                This->HookOnStatic2((HMODULE)DllBase);
            }

        }, this);

    return S_OK;
}
