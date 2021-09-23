#include "universal.inl"
#include "multimedia.h"

#include <crtdefs.h>


//////////////////////////////////////////////////////////////////////////

enum : WORD
{
    JOYSTICK_ID_MAX   = 16,

    JOYSTICK_AXIS_MIN = (WORD)0u,
    JOYSTICK_AXIS_MAX = (WORD)~JOYSTICK_AXIS_MIN,
    JOYSTICK_AXIS_MIDDLE = (WORD)(JOYSTICK_AXIS_MAX / 2u),
};

static const std::string MMGetErrorString(MMRESULT code) noexcept;
static const std::string MMGetCapabilitesString(UINT caps) noexcept;
static const std::string MMGetButtonsString(UINT button) noexcept;
static const std::string MMGetButtonsStateString(UINT state) noexcept;
static const std::string MMGetJoyPosFlagsString(UINT flags) noexcept;

std::ostream& operator<<(std::ostream& os, const JOYINFOEX& state)
{
    return os <<
        "{\n" <<
        " \t Flags  =" << MMGetJoyPosFlagsString(state.dwFlags) << "\n"
        " \t Buttons=" << MMGetButtonsString(state.dwButtons)   << "\n"
        " \t X pos  =" << state.dwXpos << "\n"
        " \t Y pos  =" << state.dwYpos << "\n"
        " \t Z pos  =" << state.dwZpos << "\n"
        " \t R pos  =" << state.dwRpos << "\n"
        " \t U pos  =" << state.dwUpos << "\n"
        " \t V pos  =" << state.dwVpos << "\n"
        " \t   POV  =" << state.dwPOV  << "\n"
        "}";
}

std::ostream& operator<<(std::ostream& os, const JOYCAPSW& caps)
{
    return os <<
        "{\n" <<
        " \t Name      =" << base::wcstombs(caps.szPname)  << "\n"
        " \t ManufactID=" << MMGetButtonsString(caps.wMid) << ", ProductID=" << caps.wPid << "\n"
        " \t RegKey    =" << base::wcstombs(caps.szRegKey) << ", OEMVxD   =" << base::wcstombs(caps.szOEMVxD) << "\n"
        " \t Capabilite=" << MMGetCapabilitesString(caps.wCaps) << "\n"
        " \t Axes   num=" << caps.wNumAxes << ", max=" << caps.wMaxAxes << "\n"
        " \t Button num=" << caps.wNumButtons << ", max=" << caps.wMaxButtons << "\n"
        " \t X      min=" << caps.wXmin << ", max=" << caps.wXmax << "\n"
        " \t Y      min=" << caps.wYmin << ", max=" << caps.wYmax << "\n"
        " \t Z      min=" << caps.wZmin << ", max=" << caps.wZmax << "\n"
        " \t R      min=" << caps.wRmin << ", max=" << caps.wRmax << "\n"
        " \t U      min=" << caps.wUmin << ", max=" << caps.wUmax << "\n"
        " \t V      min=" << caps.wVmin << ", max=" << caps.wVmax << "\n"
        " \t Period min=" << caps.wPeriodMin << ", max=" << caps.wPeriodMax << "\n"
        "}";
}

namespace Hook
{
#define JOY_BUTTON(x) (1u<<x)

    static auto Call_WindowProc = (WNDPROC)nullptr;
    static LRESULT CALLBACK SpyWindowProc(HWND window, UINT msg, WPARAM arg1, LPARAM arg2)
    {
        switch (msg)
        {
        case MM_JOY1MOVE:
        case MM_JOY2MOVE:
        {
            DWORD Xpos = LOWORD(arg2);
            DWORD Ypos = HIWORD(arg2);
            UINT Buttons = static_cast<UINT>(arg1);

            spdlog::info(__FUNCTION__ "(): Msg=MM_JOYMOVE, JoyID={}.", (msg & ~MM_JOY1MOVE));
            spdlog::info("\t Buttons={}.", MMGetButtonsString(Buttons));
            spdlog::info("\t Xpos={}, Ypos={}.", Xpos, Ypos);

            break;
        }
        case MM_JOY1ZMOVE:
        case MM_JOY2ZMOVE:
        {
            DWORD Zpos = LOWORD(arg2);
            UINT Buttons = static_cast<UINT>(arg1);

            spdlog::info(__FUNCTION__ "(): Msg=MM_JOYZMOVE, JoyID={}.", (msg & ~MM_JOY1ZMOVE));
            spdlog::info("\t Buttons={}.", MMGetButtonsString(Buttons));
            spdlog::info("\t Zpos={}.", Zpos);

            break;
        }
        case MM_JOY1BUTTONDOWN:
        case MM_JOY2BUTTONDOWN:
        {
            DWORD Xpos = LOWORD(arg2);
            DWORD Ypos = HIWORD(arg2);
            UINT ButtonsAndState = static_cast<UINT>(arg1);

            spdlog::info(__FUNCTION__ "(): Msg=MM_JOYBUTTONDOWN, JoyID={}.", (msg & ~MM_JOY1BUTTONDOWN));
            spdlog::info("\t Buttons={}, State={}.", MMGetButtonsString(ButtonsAndState), MMGetButtonsStateString(ButtonsAndState));
            spdlog::info("\t Xpos={}, Ypos={}.", Xpos, Ypos);

            break;
        }
        case MM_JOY1BUTTONUP:
        case MM_JOY2BUTTONUP:
        {
            DWORD Xpos = LOWORD(arg2);
            DWORD Ypos = HIWORD(arg2);
            UINT ButtonsAndState = static_cast<UINT>(arg1);

            spdlog::info(__FUNCTION__ "(): Msg=MM_JOYBUTTONUP, JoyID={}.", (msg & ~MM_JOY1BUTTONUP));
            spdlog::info("\t Buttons={}, State={}.", MMGetButtonsString(ButtonsAndState), MMGetButtonsStateString(ButtonsAndState));
            spdlog::info("\t Xpos={}, Ypos={}.", Xpos, Ypos);

            break;
        }
        }

        return CallWindowProc(Call_WindowProc, window, msg, arg1, arg2);
    }

    MMRESULT WINAPI Mock_joyGetPosEx(
        _In_ UINT uJoyID,
        _Inout_ LPJOYINFOEX pji)
    {
        if (pji == nullptr || pji->dwSize != sizeof(JOYINFOEX))
        {
            return JOYERR_PARMS;
        }

        if (uJoyID != JOYSTICKID1)
        {
            return JOYERR_UNPLUGGED;
        }

        static int down = 0;

        pji->dwXpos = JOYSTICK_AXIS_MAX;
        pji->dwYpos = JOYSTICK_AXIS_MAX;
        pji->dwZpos = JOYSTICK_AXIS_MAX;
        pji->dwRpos = JOYSTICK_AXIS_MAX;
        pji->dwUpos = JOYSTICK_AXIS_MAX;
        pji->dwVpos = 0;
        pji->dwPOV = JOY_POVCENTERED;
        if (down++ <= 20)
        {
            pji->dwButtons = JOY_BUTTON2;
            pji->dwButtonNumber = 1;
        }
        else
        {
            down = 0;
            pji->dwButtons = 0;
            pji->dwButtonNumber = 0;
        }
        pji->dwReserved1 = 0;
        pji->dwReserved2 = 0;

        return JOYERR_NOERROR;
    }

    DetourMakeMacro(MMRESULT, WINAPI, joyGetPosEx)(
        _In_ UINT uJoyID,
        _Out_ LPJOYINFOEX pji
        )
    {
         MMRESULT result = DetourCallMacro(joyGetPosEx)(uJoyID, pji);

        if (result == JOYERR_NOERROR)
        {
            spdlog::info(__FUNCTION__ "({}): JoyID={}, State={}", MMGetErrorString(result), uJoyID, *pji);
        }
        else
        {
            spdlog::info(__FUNCTION__ "({}): JoyID={}.", MMGetErrorString(result), uJoyID);
        }

        return result;
    }

    DetourMakeMacro(UINT, WINAPI, joyGetNumDevs)(
        void
        )
    {
        UINT num = DetourCallMacro(joyGetNumDevs)();

        spdlog::info(__FUNCTION__ "(): Number of Devices {}.", num);

        return num;
    }

    DetourMakeMacro(MMRESULT, WINAPI, joyGetDevCapsW)(
        _In_ UINT_PTR uJoyID,
        _Out_writes_bytes_(cbjc) LPJOYCAPSW pjc,
        _In_ UINT cbjc
        )
    {
        MMRESULT result = DetourCallMacro(joyGetDevCapsW)(uJoyID, pjc, cbjc);
        if (result == MMSYSERR_NOERROR)
        {
            spdlog::info(__FUNCTION__ "({}): JoyID={}, Caps={}", MMGetErrorString(result), uJoyID, *pjc);
        }
        else
        {
            spdlog::info(__FUNCTION__ "({}): JoyID={}.", MMGetErrorString(result), uJoyID);
        }

        return result;
    }

    DetourMakeMacro(MMRESULT, WINAPI, joySetCapture)(
        _In_ HWND hwnd,
        _In_ UINT uJoyID,
        _In_ UINT uPeriod,
        _In_ BOOL fChanged
        )
    {
        MMRESULT result = DetourCallMacro(joySetCapture)(hwnd, uJoyID, uPeriod, fChanged);

        spdlog::info(__FUNCTION__ "({}): JoyID={}, Window={}, Period={}, Changed={}.",
            MMGetErrorString(result), uJoyID, static_cast<void*>(hwnd), uPeriod, fChanged);

        if (result)
        {
            if (auto Proc = SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SpyWindowProc)))
            {
                Call_WindowProc = reinterpret_cast<WNDPROC>(Proc);

                spdlog::info(__FUNCTION__ "(): Swap WindowProc from {:#} to {:#}.",
                    reinterpret_cast<void*>(SpyWindowProc), reinterpret_cast<void*>(Proc));
            }
        }

        return result;
    }

    DetourMakeMacro(MMRESULT, WINAPI, joyReleaseCapture)(
        _In_ UINT uJoyID
        )
    {
        MMRESULT result = DetourCallMacro(joyReleaseCapture)(uJoyID);

        spdlog::info(__FUNCTION__ "({}): JoyID={}.", MMGetErrorString(result), uJoyID);

        return result;
    }

    DetourMakeMacro(MMRESULT, WINAPI, joySetThreshold)(
        _In_ UINT uJoyID,
        _In_ UINT uThreshold
        )
    {
        MMRESULT result = DetourCallMacro(joySetThreshold)(uJoyID, uThreshold);

        spdlog::info(__FUNCTION__ "({}): JoyID={}, Threshold={}", MMGetErrorString(result), uJoyID, uThreshold);

        return result;
    }

    DetourMakeMacro(MMRESULT, WINAPI, joyGetThreshold)(
        _In_ UINT uJoyID,
        _Out_ LPUINT puThreshold
        )
    {
        MMRESULT result = DetourCallMacro(joyGetThreshold)(uJoyID, puThreshold);
        if (result == MMSYSERR_NOERROR)
        {
            spdlog::info(__FUNCTION__ "({}): JoyID={}, Threshold={}", MMGetErrorString(result), uJoyID, *puThreshold);
        }
        else
        {
            spdlog::info(__FUNCTION__ "({}): JoyID={}.", MMGetErrorString(result), uJoyID);
        }

        return result;
    }

    DetourMakeMacro(MMRESULT, WINAPI, joyConfigChanged)(
        _In_ DWORD dwFlags
        )
    {
        MMRESULT result = DetourCallMacro(joyConfigChanged)(dwFlags);

        spdlog::info(__FUNCTION__ "({}): Called.", MMGetErrorString(result));

        return result;
    }
}

//////////////////////////////////////////////////////////////////////////

static const std::string MMGetErrorString(MMRESULT code) noexcept
{
    std::string str;

    switch (code)
    {
    default:
        str = std::to_string(code);
        break;

#define _CASE_(x) case x: str = #x; break
        _CASE_(MMSYSERR_NOERROR);
        _CASE_(MMSYSERR_ERROR);
        _CASE_(MMSYSERR_BADDEVICEID);
        _CASE_(MMSYSERR_NOTENABLED);
        _CASE_(MMSYSERR_ALLOCATED);
        _CASE_(MMSYSERR_INVALHANDLE);
        _CASE_(MMSYSERR_NODRIVER);
        _CASE_(MMSYSERR_NOMEM);
        _CASE_(MMSYSERR_NOTSUPPORTED);
        _CASE_(MMSYSERR_BADERRNUM);
        _CASE_(MMSYSERR_INVALFLAG);
        _CASE_(MMSYSERR_INVALPARAM);
        _CASE_(MMSYSERR_HANDLEBUSY);
        _CASE_(MMSYSERR_INVALIDALIAS);
        _CASE_(MMSYSERR_BADDB);
        _CASE_(MMSYSERR_KEYNOTFOUND);
        _CASE_(MMSYSERR_READERROR);
        _CASE_(MMSYSERR_WRITEERROR);
        _CASE_(MMSYSERR_DELETEERROR);
        _CASE_(MMSYSERR_VALNOTFOUND);
        _CASE_(MMSYSERR_NODRIVERCB);
        _CASE_(MMSYSERR_MOREDATA);
#undef _CASE_
    }

    return str;
}

static const std::string MMGetCapabilitesString(UINT caps) noexcept
{
    std::string str;

#define _APPEND_(x) if (caps & x) str.append(#x).append(" | ")
    _APPEND_(JOYCAPS_HASZ);
    _APPEND_(JOYCAPS_HASR);
    _APPEND_(JOYCAPS_HASU);
    _APPEND_(JOYCAPS_HASV);
    _APPEND_(JOYCAPS_HASPOV);
    _APPEND_(JOYCAPS_POV4DIR);
    _APPEND_(JOYCAPS_POVCTS);
#undef _APPEND_

    if (!str.empty())
    {
        str.resize(str.size() - 3);
    }

    return str;
}

static const std::string MMGetButtonsString(UINT buttons) noexcept
{
    std::string str;

#define _APPEND_(x) if (buttons & x) str.append(#x).append(" | ")
    _APPEND_(JOY_BUTTON1);
    _APPEND_(JOY_BUTTON2);
    _APPEND_(JOY_BUTTON3);
    _APPEND_(JOY_BUTTON4);
    _APPEND_(JOY_BUTTON5);
    _APPEND_(JOY_BUTTON6);
    _APPEND_(JOY_BUTTON7);
    _APPEND_(JOY_BUTTON8);
    _APPEND_(JOY_BUTTON9);
    _APPEND_(JOY_BUTTON10);
    _APPEND_(JOY_BUTTON11);
    _APPEND_(JOY_BUTTON12);
    _APPEND_(JOY_BUTTON13);
    _APPEND_(JOY_BUTTON14);
    _APPEND_(JOY_BUTTON15);
    _APPEND_(JOY_BUTTON16);
    _APPEND_(JOY_BUTTON17);
    _APPEND_(JOY_BUTTON18);
    _APPEND_(JOY_BUTTON19);
    _APPEND_(JOY_BUTTON20);
    _APPEND_(JOY_BUTTON21);
    _APPEND_(JOY_BUTTON22);
    _APPEND_(JOY_BUTTON23);
    _APPEND_(JOY_BUTTON24);
    _APPEND_(JOY_BUTTON25);
    _APPEND_(JOY_BUTTON26);
    _APPEND_(JOY_BUTTON27);
    _APPEND_(JOY_BUTTON28);
    _APPEND_(JOY_BUTTON29);
    _APPEND_(JOY_BUTTON30);
    _APPEND_(JOY_BUTTON31);
    _APPEND_(JOY_BUTTON32);
#undef _APPEND_

    if (!str.empty())
    {
        str.resize(str.size() - 3);
    }

    return str;
}

static const std::string MMGetButtonsStateString(UINT state) noexcept
{
    std::string str;

#define _APPEND_(x) if (state & x) str.append(#x).append(" | ")
    _APPEND_(JOY_BUTTON1CHG);
    _APPEND_(JOY_BUTTON2CHG);
    _APPEND_(JOY_BUTTON3CHG);
    _APPEND_(JOY_BUTTON4CHG);
#undef _APPEND_

    if (!str.empty())
    {
        str.resize(str.size() - 3);
    }

    return str;
}

static const std::string MMGetJoyPosFlagsString(UINT flags) noexcept
{
    std::string str;

#define _APPEND_(x) if (flags & x) str.append(#x).append(" | ")
    _APPEND_(JOY_RETURNX);
    _APPEND_(JOY_RETURNY);
    _APPEND_(JOY_RETURNZ);
    _APPEND_(JOY_RETURNR);
    _APPEND_(JOY_RETURNU);
    _APPEND_(JOY_RETURNV);
    _APPEND_(JOY_RETURNPOV);
    _APPEND_(JOY_RETURNBUTTONS);
    _APPEND_(JOY_RETURNRAWDATA);
    _APPEND_(JOY_RETURNPOVCTS);
    _APPEND_(JOY_RETURNCENTERED);
    _APPEND_(JOY_USEDEADZONE);
    _APPEND_(JOY_CAL_READALWAYS);
    _APPEND_(JOY_CAL_READXYONLY);
    _APPEND_(JOY_CAL_READ3);
    _APPEND_(JOY_CAL_READ4);
    _APPEND_(JOY_CAL_READXONLY);
    _APPEND_(JOY_CAL_READYONLY);
    _APPEND_(JOY_CAL_READ5);
    _APPEND_(JOY_CAL_READ6);
    _APPEND_(JOY_CAL_READZONLY);
    _APPEND_(JOY_CAL_READRONLY);
    _APPEND_(JOY_CAL_READUONLY);
    _APPEND_(JOY_CAL_READVONLY);
#undef _APPEND_

    if (!str.empty())
    {
        str.resize(str.size() - 3);
    }

    return str;
}

HRESULT MultiMediaSpy::Attach() noexcept
{
    return SUCCEEDED(HookOnStatic()) ? S_OK : HookOnDynamic();
}

HRESULT MultiMediaSpy::HookOnStatic(HMODULE module) noexcept
{
    module = module ? module : GetModuleHandleA(MultiMediaSpy::MODULE_NAME);
    if (module == nullptr)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    spdlog::info(__FUNCTION__ "(): Called.");

    DetourBegin();
    {
        using namespace Hook;

        DetourAttachMacro(module, joyGetPosEx);
        DetourAttachMacro(module, joyGetNumDevs);
        DetourAttachMacro(module, joyGetDevCapsW);
        DetourAttachMacro(module, joySetCapture);
        DetourAttachMacro(module, joyReleaseCapture);
        DetourAttachMacro(module, joySetThreshold);
        DetourAttachMacro(module, joyGetThreshold);
        DetourAttachMacro(module, joyConfigChanged);
    }
    return HRESULT_FROM_WIN32(DetourCommit());
}

HRESULT MultiMediaSpy::HookOnDynamic() noexcept
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

            auto This = static_cast<MultiMediaSpy*>(Context);
            if (This == nullptr)
            {
                return;
            }

            auto DllBase = NotificationData->Loaded.DllBase;
            auto DllName = std::wstring(
                NotificationData->Loaded.BaseDllName->Buffer,
                NotificationData->Loaded.BaseDllName->Length / sizeof(wchar_t));

            std::transform(DllName.begin(), DllName.end(), DllName.begin(), [](wint_t c) { return (wchar_t)std::towlower(c); });

            if (DllName == base::mbstowcs(MultiMediaSpy::MODULE_NAME))
            {
                This->HookOnStatic((HMODULE)DllBase);
            }

        }, this);

    return S_OK;
}
