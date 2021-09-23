#include "universal.inl"
#include "xinput.h"

#include <tlhelp32.h>


static const std::string XInputGetCapsTypeString(BYTE code) noexcept;
static const std::string XInputGetCapsSubtypeString(BYTE code) noexcept;
static const std::string XInputGetCapsFlagsString(WORD code) noexcept;
static const std::string XInputGetButtonsString(WORD code) noexcept;
static const std::string XInputGetKeystrokeFlagsString(WORD code) noexcept;
static const std::string XInputGetKeystrokeVKString(WORD code) noexcept;

std::ostream& operator<<(std::ostream& os, const XINPUT_CAPABILITIES& caps)
{
    return os <<
        "{\n" <<
        " \t Type       =" << XInputGetCapsTypeString(caps.Type)       << "\n"
        " \t Subtypes   =" << XInputGetCapsSubtypeString(caps.SubType) << "\n"
        " \t Flags      =" << XInputGetCapsFlagsString(caps.Flags)     << "\n"
        " \t Gamepad: Buttons       =" << XInputGetButtonsString(caps.Gamepad.wButtons) << "\n"
        " \t          LeftTrigger   =" << (uint32_t)caps.Gamepad.bLeftTrigger  << "\n"
        " \t          RightTrigger  =" << (uint32_t)caps.Gamepad.bRightTrigger << "\n"
        " \t          ThumbLX       =" << caps.Gamepad.sThumbLX << "\n"
        " \t          ThumbLY       =" << caps.Gamepad.sThumbLY << "\n"
        " \t          ThumbRX       =" << caps.Gamepad.sThumbRX << "\n"
        " \t          ThumbRY       =" << caps.Gamepad.sThumbRY << "\n"
        " \t Vibration: LeftMotorSpeed=" << caps.Vibration.wLeftMotorSpeed << ", RightMotorSpeed=" << caps.Vibration.wRightMotorSpeed << "\n"
        "}";
}

std::ostream& operator<<(std::ostream& os, const XINPUT_STATE& state)
{
    return os <<
        "{\n" <<
        " \t PacketNumber=" << state.dwPacketNumber << "\n"
        " \t Gamepad: Buttons       =" << XInputGetButtonsString(state.Gamepad.wButtons) << "\n"
        " \t          LeftTrigger   =" << (uint32_t)state.Gamepad.bLeftTrigger  << "\n"
        " \t          RightTrigger  =" << (uint32_t)state.Gamepad.bRightTrigger << "\n"
        " \t          ThumbLX       =" << state.Gamepad.sThumbLX << "\n"
        " \t          ThumbLY       =" << state.Gamepad.sThumbLY << "\n"
        " \t          ThumbRX       =" << state.Gamepad.sThumbRX << "\n"
        " \t          ThumbRY       =" << state.Gamepad.sThumbRY << "\n"
        "}";
}

std::ostream& operator<<(std::ostream& os, const XINPUT_KEYSTROKE& keystroke)
{
    return os <<
        "{\n" <<
        " \t VirtualKey=" << XInputGetKeystrokeVKString(keystroke.VirtualKey) << "\n"
        " \t HidCode   =" << keystroke.HidCode    << "\n"
        " \t Flags     =" << XInputGetKeystrokeFlagsString(keystroke.Flags) << "\n"
        "}";
}

namespace Hook
{
    namespace X9_1_0
    {
        DetourMakeMacro(DWORD, WINAPI, XInputGetCapabilities)(
            _In_  DWORD                dwUserIndex,                     // Index of the gamer associated with the device
            _In_  DWORD                dwFlags,                         // Input flags that identify the device type
            _Out_ XINPUT_CAPABILITIES* pCapabilities                    // Receives the capabilities
            )
        {
            DWORD Result = DetourCallMacro(XInputGetCapabilities)(
                dwUserIndex,
                dwFlags,
                pCapabilities);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}, Caps={}", Result, dwUserIndex, *pCapabilities);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputSetState)(
            _In_ DWORD             dwUserIndex,                         // Index of the gamer associated with the device
            _In_ XINPUT_VIBRATION* pVibration                           // The vibration information to send to the controller
            )
        {
            DWORD Result = DetourCallMacro(XInputSetState)(
                dwUserIndex,
                pVibration);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "(): JoyID={}, Vibration: LeftMotorSpeed={}, RightMotorSpeed={}.",
                    dwUserIndex,
                    pVibration->wLeftMotorSpeed,
                    pVibration->wRightMotorSpeed);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputGetState)(
            _In_  DWORD         dwUserIndex,                            // Index of the gamer associated with the device
            _Out_ XINPUT_STATE* pState                                  // Receives the current state
            )
        {
            DWORD Result = DetourCallMacro(XInputGetState)(
                dwUserIndex,
                pState);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}, State={}", Result, dwUserIndex, *pState);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }
    }

    namespace X1_1
    {
        DetourMakeMacro(DWORD, WINAPI, XInputGetCapabilities)(
            _In_  DWORD                dwUserIndex,                     // Index of the gamer associated with the device
            _In_  DWORD                dwFlags,                         // Input flags that identify the device type
            _Out_ XINPUT_CAPABILITIES* pCapabilities                    // Receives the capabilities
            )
        {
            DWORD Result = DetourCallMacro(XInputGetCapabilities)(
                dwUserIndex,
                dwFlags,
                pCapabilities);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}, Caps={}", Result, dwUserIndex, *pCapabilities);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputSetState)(
            _In_ DWORD             dwUserIndex,                         // Index of the gamer associated with the device
            _In_ XINPUT_VIBRATION* pVibration                           // The vibration information to send to the controller
            )
        {
            DWORD Result = DetourCallMacro(XInputSetState)(
                dwUserIndex,
                pVibration);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "(): JoyID={}, Vibration: LeftMotorSpeed={}, RightMotorSpeed={}.",
                    dwUserIndex,
                    pVibration->wLeftMotorSpeed,
                    pVibration->wRightMotorSpeed);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputGetState)(
            _In_  DWORD         dwUserIndex,                            // Index of the gamer associated with the device
            _Out_ XINPUT_STATE* pState                                  // Receives the current state
            )
        {
            DWORD Result = DetourCallMacro(XInputGetState)(
                dwUserIndex,
                pState);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}, State={}", Result, dwUserIndex, *pState);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }
    }

    namespace X1_2
    {
        DetourMakeMacro(DWORD, WINAPI, XInputGetCapabilities)(
            _In_  DWORD                dwUserIndex,                     // Index of the gamer associated with the device
            _In_  DWORD                dwFlags,                         // Input flags that identify the device type
            _Out_ XINPUT_CAPABILITIES* pCapabilities                    // Receives the capabilities
            )
        {
            DWORD Result = DetourCallMacro(XInputGetCapabilities)(
                dwUserIndex,
                dwFlags,
                pCapabilities);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}, Caps={}", Result, dwUserIndex, *pCapabilities);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputSetState)(
            _In_ DWORD             dwUserIndex,                         // Index of the gamer associated with the device
            _In_ XINPUT_VIBRATION* pVibration                           // The vibration information to send to the controller
            )
        {
            DWORD Result = DetourCallMacro(XInputSetState)(
                dwUserIndex,
                pVibration);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "(): JoyID={}, Vibration: LeftMotorSpeed={}, RightMotorSpeed={}.",
                    dwUserIndex,
                    pVibration->wLeftMotorSpeed,
                    pVibration->wRightMotorSpeed);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputGetState)(
            _In_  DWORD         dwUserIndex,                            // Index of the gamer associated with the device
            _Out_ XINPUT_STATE* pState                                  // Receives the current state
            )
        {
            DWORD Result = DetourCallMacro(XInputGetState)(
                dwUserIndex,
                pState);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}, State={}", Result, dwUserIndex, *pState);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }
    }

    namespace X1_3
    {
        DetourMakeMacro(DWORD, WINAPI, XInputGetBatteryInformation)(
            _In_  DWORD                       dwUserIndex,              // Index of the gamer associated with the device
            _In_  BYTE                        devType,                  // Which device on this user index
            _Out_ XINPUT_BATTERY_INFORMATION* pBatteryInformation       // Contains the level and types of batteries
            )
        {
            DWORD Result = DetourCallMacro(XInputGetBatteryInformation)(
                dwUserIndex,
                devType,
                pBatteryInformation);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "(): JoyID={}, BatteryType={}, BatteryLevel={}.",
                    dwUserIndex, pBatteryInformation->BatteryType, pBatteryInformation->BatteryLevel);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputGetCapabilities)(
            _In_  DWORD                dwUserIndex,                     // Index of the gamer associated with the device
            _In_  DWORD                dwFlags,                         // Input flags that identify the device type
            _Out_ XINPUT_CAPABILITIES* pCapabilities                    // Receives the capabilities
            )
        {
            DWORD Result = DetourCallMacro(XInputGetCapabilities)(
                dwUserIndex,
                dwFlags,
                pCapabilities);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}, Caps={}", Result, dwUserIndex, *pCapabilities);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputSetState)(
            _In_ DWORD             dwUserIndex,                         // Index of the gamer associated with the device
            _In_ XINPUT_VIBRATION* pVibration                           // The vibration information to send to the controller
            )
        {
            DWORD Result = DetourCallMacro(XInputSetState)(
                dwUserIndex,
                pVibration);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "(): JoyID={}, Vibration: LeftMotorSpeed={}, RightMotorSpeed={}.",
                    dwUserIndex,
                    pVibration->wLeftMotorSpeed,
                    pVibration->wRightMotorSpeed);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputGetState)(
            _In_  DWORD         dwUserIndex,                            // Index of the gamer associated with the device
            _Out_ XINPUT_STATE* pState                                  // Receives the current state
            )
        {
            DWORD Result = DetourCallMacro(XInputGetState)(
                dwUserIndex,
                pState);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}, State={}", Result, dwUserIndex, *pState);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputGetStateEx)(
            _In_  DWORD         dwUserIndex,                            // Index of the gamer associated with the device
            _Out_ struct XINPUT_STATEEX* pState                         // Receives the current state
            )
        {
            DWORD Result = DetourCallMacro(XInputGetStateEx)(
                dwUserIndex,
                pState);

            if (Result == ERROR_SUCCESS)
            {
                auto State = reinterpret_cast<XINPUT_STATE*>(pState);
                spdlog::info(__FUNCTION__ "({}): JoyID={}, State={}", Result, dwUserIndex, *State);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputGetKeystroke)(
            _In_       DWORD dwUserIndex,                               // Index of the gamer associated with the device
            _Reserved_ DWORD dwReserved,                                // Reserved for future use
            _Out_      PXINPUT_KEYSTROKE pKeystroke                     // Pointer to an XINPUT_KEYSTROKE structure that receives an input event.
            )
        {
            DWORD Result = DetourCallMacro(XInputGetKeystroke)(
                dwUserIndex,
                dwReserved,
                pKeystroke);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}, Keystroke={}", Result, dwUserIndex, *pKeystroke);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }
    }

    namespace X1_4
    {
        DetourMakeMacro(DWORD, WINAPI, XInputGetBaseBusInformation)(
            _In_  DWORD                dwUserIndex,
            _Out_ struct XINPUT_BUSINFO* pBusInfo
            )
        {
            DWORD Result = DetourCallMacro(XInputGetBaseBusInformation)(
                dwUserIndex,
                pBusInfo);

            spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputGetBatteryInformation)(
            _In_  DWORD                       dwUserIndex,              // Index of the gamer associated with the device
            _In_  BYTE                        devType,                  // Which device on this user index
            _Out_ XINPUT_BATTERY_INFORMATION* pBatteryInformation       // Contains the level and types of batteries
            )
        {
            DWORD Result = DetourCallMacro(XInputGetBatteryInformation)(
                dwUserIndex,
                devType,
                pBatteryInformation);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "(): JoyID={}, BatteryType={}, BatteryLevel={}.",
                    dwUserIndex, pBatteryInformation->BatteryType, pBatteryInformation->BatteryLevel);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputGetAudioDeviceIds)(
            _In_  DWORD                             dwUserIndex,        // Index of the gamer associated with the device
            _Out_writes_opt_(*pRenderCount) LPWSTR  pRenderDeviceId,    // Windows Core Audio device ID string for render (speakers)
            _Inout_opt_ UINT* pRenderCount,                             // Size of render device ID string buffer (in wide-chars)
            _Out_writes_opt_(*pCaptureCount) LPWSTR pCaptureDeviceId,   // Windows Core Audio device ID string for capture (microphone)
            _Inout_opt_ UINT* pCaptureCount                             // Size of capture device ID string buffer (in wide-chars)
            )
        {
            DWORD Result = DetourCallMacro(XInputGetAudioDeviceIds)(
                dwUserIndex,
                pRenderDeviceId,
                pRenderCount,
                pCaptureDeviceId,
                pCaptureCount);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "(): JoyID={}, RenderDeviceId={}, CaptureDeviceId={}.",
                    dwUserIndex, base::wcstombs(pRenderDeviceId), base::wcstombs(pCaptureDeviceId));
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputGetCapabilities)(
            _In_  DWORD                dwUserIndex,                     // Index of the gamer associated with the device
            _In_  DWORD                dwFlags,                         // Input flags that identify the device type
            _Out_ XINPUT_CAPABILITIES* pCapabilities                    // Receives the capabilities
            )
        {
            DWORD Result = DetourCallMacro(XInputGetCapabilities)(
                dwUserIndex,
                dwFlags,
                pCapabilities);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}, Caps={}", Result, dwUserIndex, *pCapabilities);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputGetCapabilitiesEx)(
            _In_  DWORD                dwAlways1,
            _In_  DWORD                dwUserIndex,                     // Index of the gamer associated with the device
            _In_  DWORD                dwFlags,                         // Input flags that identify the device type
            _Out_ struct XINPUT_CAPABILITIESEX* pCapabilities           // Receives the capabilities
            )
        {
            DWORD Result = DetourCallMacro(XInputGetCapabilitiesEx)(
                dwAlways1,
                dwUserIndex,
                dwFlags,
                pCapabilities);

            if (Result == ERROR_SUCCESS)
            {
                auto Capabilities = reinterpret_cast<XINPUT_CAPABILITIES*>(pCapabilities);
                spdlog::info(__FUNCTION__ "({}): JoyID={}, Caps={}", Result, dwUserIndex, *Capabilities);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputSetState)(
            _In_ DWORD             dwUserIndex,                         // Index of the gamer associated with the device
            _In_ XINPUT_VIBRATION* pVibration                           // The vibration information to send to the controller
            )
        {
            DWORD Result = DetourCallMacro(XInputSetState)(
                dwUserIndex,
                pVibration);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "(): JoyID={}, Vibration: LeftMotorSpeed={}, RightMotorSpeed={}.",
                    dwUserIndex,
                    pVibration->wLeftMotorSpeed,
                    pVibration->wRightMotorSpeed);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputGetState)(
            _In_  DWORD         dwUserIndex,                            // Index of the gamer associated with the device
            _Out_ XINPUT_STATE* pState                                  // Receives the current state
            )
        {
            DWORD Result = DetourCallMacro(XInputGetState)(
                dwUserIndex,
                pState);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}, State={}", Result, dwUserIndex, *pState);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputGetStateEx)(
            _In_  DWORD         dwUserIndex,                            // Index of the gamer associated with the device
            _Out_ struct XINPUT_STATEEX* pState                         // Receives the current state
            )
        {
            DWORD Result = DetourCallMacro(XInputGetStateEx)(
                dwUserIndex,
                pState);

            if (Result == ERROR_SUCCESS)
            {
                auto State = reinterpret_cast<XINPUT_STATE*>(pState);
                spdlog::info(__FUNCTION__ "({}): JoyID={}, State={}", Result, dwUserIndex, *State);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }

        DetourMakeMacro(DWORD, WINAPI, XInputGetKeystroke)(
            _In_       DWORD dwUserIndex,                               // Index of the gamer associated with the device
            _Reserved_ DWORD dwReserved,                                // Reserved for future use
            _Out_      PXINPUT_KEYSTROKE pKeystroke                     // Pointer to an XINPUT_KEYSTROKE structure that receives an input event.
            )
        {
            DWORD Result = DetourCallMacro(XInputGetKeystroke)(
                dwUserIndex,
                dwReserved,
                pKeystroke);

            if (Result == ERROR_SUCCESS)
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}, Keystroke={}", Result, dwUserIndex, *pKeystroke);
            }
            else
            {
                spdlog::info(__FUNCTION__ "({}): JoyID={}", Result, dwUserIndex);
            }

            return Result;
        }
    }
}

static const std::string XInputGetCapsTypeString(BYTE code) noexcept
{
    std::string str;

    switch (code)
    {
    default:
        str = std::to_string(code);
        break;

#define _CASE_(x) case x: str = #x; break
        _CASE_(XINPUT_DEVTYPE_GAMEPAD);
#undef _CASE_
    }

    return str;
}

static const std::string XInputGetCapsSubtypeString(BYTE code) noexcept
{
    std::string str;

    switch (code)
    {
    default:
        str = "XInput Device #[1+JoyID]";
        break;

//#define _CASE_(x) case x: str = #x; break
//        _CASE_(XINPUT_DEVSUBTYPE_UNKNOWN);
//        _CASE_(XINPUT_DEVSUBTYPE_WHEEL);
//        _CASE_(XINPUT_DEVSUBTYPE_ARCADE_STICK);
//        _CASE_(XINPUT_DEVSUBTYPE_FLIGHT_STICK);
//        _CASE_(XINPUT_DEVSUBTYPE_DANCE_PAD);
//        _CASE_(XINPUT_DEVSUBTYPE_GUITAR);
//        _CASE_(XINPUT_DEVSUBTYPE_GUITAR_ALTERNATE);
//        _CASE_(XINPUT_DEVSUBTYPE_DRUM_KIT);
//        _CASE_(XINPUT_DEVSUBTYPE_GUITAR_BASS);
//        _CASE_(XINPUT_DEVSUBTYPE_ARCADE_PAD);
//#undef _CASE_

    case XINPUT_DEVSUBTYPE_GAMEPAD:
        str = "XInput Controller #[1+JoyID]";
        break;

    case XINPUT_DEVSUBTYPE_WHEEL:
        str = "XInput Wheel #[1+JoyID]";
        break;

    case XINPUT_DEVSUBTYPE_ARCADE_STICK:
        str = "XInput ArcadeStick #[1+JoyID]";
        break;

    case XINPUT_DEVSUBTYPE_FLIGHT_STICK:
        str = "XInput FlightStick #[1+JoyID]";
        break;

    case XINPUT_DEVSUBTYPE_DANCE_PAD:
        str = "XInput DancePad #[1+JoyID]";
        break;

    case XINPUT_DEVSUBTYPE_GUITAR:
    case XINPUT_DEVSUBTYPE_GUITAR_ALTERNATE:
    case XINPUT_DEVSUBTYPE_GUITAR_BASS:
        str = "XInput Guitar #[1+JoyID]";
        break;

    case XINPUT_DEVSUBTYPE_DRUM_KIT:
        str = "XInput DrumKit #[1+JoyID]";
        break;

    case XINPUT_DEVSUBTYPE_ARCADE_PAD:
        str = "XInput ArcadePad #[1+JoyID]";
        break;
    }

    return str;
}

static const std::string XInputGetCapsFlagsString(WORD code) noexcept
{
    std::string str;

#define _APPEND_(x) if (code & x) str.append(#x).append(" | ")
    _APPEND_(XINPUT_CAPS_FFB_SUPPORTED);
    _APPEND_(XINPUT_CAPS_WIRELESS);
    _APPEND_(XINPUT_CAPS_VOICE_SUPPORTED);
    _APPEND_(XINPUT_CAPS_PMD_SUPPORTED);
    _APPEND_(XINPUT_CAPS_NO_NAVIGATION);
#undef _APPEND_

    if (!str.empty())
    {
        str.resize(str.size() - 3);
    }

    return str;
}

static const std::string XInputGetButtonsString(WORD code) noexcept
{
    std::string str;

#define _APPEND_(x) if (code & x) str.append(#x).append(" | ")
    _APPEND_(XINPUT_GAMEPAD_DPAD_UP);
    _APPEND_(XINPUT_GAMEPAD_DPAD_DOWN);
    _APPEND_(XINPUT_GAMEPAD_DPAD_LEFT);
    _APPEND_(XINPUT_GAMEPAD_DPAD_RIGHT);
    _APPEND_(XINPUT_GAMEPAD_START);
    _APPEND_(XINPUT_GAMEPAD_BACK);
    _APPEND_(XINPUT_GAMEPAD_LEFT_THUMB);
    _APPEND_(XINPUT_GAMEPAD_RIGHT_THUMB);
    _APPEND_(XINPUT_GAMEPAD_LEFT_SHOULDER);
    _APPEND_(XINPUT_GAMEPAD_RIGHT_SHOULDER);
    _APPEND_(XINPUT_GAMEPAD_A);
    _APPEND_(XINPUT_GAMEPAD_B);
    _APPEND_(XINPUT_GAMEPAD_X);
    _APPEND_(XINPUT_GAMEPAD_Y);
#undef _APPEND_

    if (!str.empty())
    {
        str.resize(str.size() - 3);
    }

    return str;
}

static const std::string XInputGetKeystrokeFlagsString(WORD code) noexcept
{
    std::string str;

#define _APPEND_(x) if (code & x) str.append(#x).append(" | ")
    _APPEND_(XINPUT_KEYSTROKE_KEYDOWN);
    _APPEND_(XINPUT_KEYSTROKE_KEYUP);
    _APPEND_(XINPUT_KEYSTROKE_REPEAT);
#undef _APPEND_

    if (!str.empty())
    {
        str.resize(str.size() - 3);
    }

    return str;
}

static const std::string XInputGetKeystrokeVKString(WORD code) noexcept
{
    std::string str;

#define _APPEND_(x) if (code & x) str.append(#x).append(" | ")
    _APPEND_(VK_PAD_A);
    _APPEND_(VK_PAD_B);
    _APPEND_(VK_PAD_X);
    _APPEND_(VK_PAD_Y);
    _APPEND_(VK_PAD_RSHOULDER);
    _APPEND_(VK_PAD_LSHOULDER);
    _APPEND_(VK_PAD_LTRIGGER);
    _APPEND_(VK_PAD_RTRIGGER);

    _APPEND_(VK_PAD_DPAD_UP);
    _APPEND_(VK_PAD_DPAD_DOWN);
    _APPEND_(VK_PAD_DPAD_LEFT);
    _APPEND_(VK_PAD_DPAD_RIGHT);
    _APPEND_(VK_PAD_START);
    _APPEND_(VK_PAD_BACK);
    _APPEND_(VK_PAD_LTHUMB_PRESS);
    _APPEND_(VK_PAD_RTHUMB_PRESS);

    _APPEND_(VK_PAD_LTHUMB_UP);
    _APPEND_(VK_PAD_LTHUMB_DOWN);
    _APPEND_(VK_PAD_LTHUMB_RIGHT);
    _APPEND_(VK_PAD_LTHUMB_LEFT);
    _APPEND_(VK_PAD_LTHUMB_UPLEFT);
    _APPEND_(VK_PAD_LTHUMB_UPRIGHT);
    _APPEND_(VK_PAD_LTHUMB_DOWNRIGHT);
    _APPEND_(VK_PAD_LTHUMB_DOWNLEFT);

    _APPEND_(VK_PAD_RTHUMB_UP);
    _APPEND_(VK_PAD_RTHUMB_DOWN);
    _APPEND_(VK_PAD_RTHUMB_RIGHT);
    _APPEND_(VK_PAD_RTHUMB_LEFT);
    _APPEND_(VK_PAD_RTHUMB_UPLEFT);
    _APPEND_(VK_PAD_RTHUMB_UPRIGHT);
    _APPEND_(VK_PAD_RTHUMB_DOWNRIGHT);
    _APPEND_(VK_PAD_RTHUMB_DOWNLEFT);
#undef _APPEND_

    if (!str.empty())
    {
        str.resize(str.size() - 3);
    }

    return str;
}

HRESULT XNAInputSpy::Attach() noexcept
{
    return SUCCEEDED(HookOnStatic()) ? S_OK : HookOnDynamic();
}

HRESULT XNAInputSpy::HookOnStatic(HMODULE module) noexcept
{
    if (module == nullptr)
    {
        HRESULT Result = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);

        auto Snap = base::stdext::scope_resource(
            CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId()),
            [](HANDLE x) { if (x) CloseHandle(x); });
        if (Snap == nullptr)
        {
            Result = HRESULT_FROM_WIN32(GetLastError());
            return Result;
        }

        MODULEENTRY32 Entry{ sizeof(MODULEENTRY32) };

        if (!Module32First(Snap, &Entry))
        {
            Result = HRESULT_FROM_WIN32(GetLastError());
            return Result;
        }

        do 
        {
            auto DllName = std::wstring(Entry.szModule);
            std::transform(DllName.begin(), DllName.end(), DllName.begin(), [](wint_t c) { return (wchar_t)std::towlower(c); });

            if (DllName.find(base::mbstowcs(MODULE_NAME)) != 0)
            {
                continue;
            }

            Result = HookOnStatic(Entry.hModule);
            if (FAILED(Result))
            {
                break;
            }

        } while (Module32Next(Snap, &Entry));

        return Result;
    }

    auto DllName = base::stdext::fs::module_path(module).filename().replace_extension().generic_u8string();
    auto VerName = DllName.substr(_countof(MODULE_NAME) - _countof(u8""));

    spdlog::info(__FUNCTION__ "(): Called. {}", DllName);

    if (VerName == u8"1_4")
    {
        DetourBegin();
        {
            using namespace Hook::X1_4;

            DetourAttachMacro(module, XInputGetAudioDeviceIds);
            DetourAttachMacro(module, XInputGetBatteryInformation);
            //DetourAttachMacro(module, XInputGetCapabilities);
            DetourAttachMacro(module, XInputSetState);
            DetourAttachMacro(module, XInputGetState);
            DetourAttachMacro(module, XInputGetKeystroke);

            DetourAttachMacroEx(module, XInputGetStateEx, 100);
            DetourAttachMacroEx(module, XInputGetBaseBusInformation, 104);
            DetourAttachMacroEx(module, XInputGetCapabilitiesEx, 108);
        }
        return HRESULT_FROM_WIN32(DetourCommit());
    }
    if (VerName == u8"1_3")
    {
        DetourBegin();
        {
            using namespace Hook::X1_3;

            DetourAttachMacro(module, XInputGetBatteryInformation);
            DetourAttachMacro(module, XInputGetCapabilities);
            DetourAttachMacro(module, XInputSetState);
            DetourAttachMacro(module, XInputGetState);
            DetourAttachMacro(module, XInputGetKeystroke);

            DetourAttachMacroEx(module, XInputGetStateEx, 100);
        }
        return HRESULT_FROM_WIN32(DetourCommit());
    }
    if (VerName == u8"1_2")
    {
        DetourBegin();
        {
            using namespace Hook::X1_2;

            DetourAttachMacro(module, XInputGetCapabilities);
            DetourAttachMacro(module, XInputSetState);
            DetourAttachMacro(module, XInputGetState);
        }
        return HRESULT_FROM_WIN32(DetourCommit());
    }
    if (VerName == u8"1_1")
    {
        DetourBegin();
        {
            using namespace Hook::X1_1;

            DetourAttachMacro(module, XInputGetCapabilities);
            DetourAttachMacro(module, XInputSetState);
            DetourAttachMacro(module, XInputGetState);
        }
        return HRESULT_FROM_WIN32(DetourCommit());
    }
    if (VerName == u8"9_1_0")
    {
        DetourBegin();
        {
            using namespace Hook::X9_1_0;

            DetourAttachMacro(module, XInputGetCapabilities);
            DetourAttachMacro(module, XInputSetState);
            DetourAttachMacro(module, XInputGetState);
        }
        return HRESULT_FROM_WIN32(DetourCommit());
    }

    return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
}

HRESULT XNAInputSpy::HookOnDynamic() noexcept
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

            auto This = static_cast<XNAInputSpy*>(Context);
            if (This == nullptr)
            {
                return;
            }

            auto DllBase = NotificationData->Loaded.DllBase;
            auto DllName = std::wstring(
                NotificationData->Loaded.BaseDllName->Buffer,
                NotificationData->Loaded.BaseDllName->Length / sizeof(wchar_t));

            std::transform(DllName.begin(), DllName.end(), DllName.begin(), [](wint_t c) { return (wchar_t)std::towlower(c); });

            if (DllName.find(base::mbstowcs(XNAInputSpy::MODULE_NAME)) == 0u)
            {
                This->HookOnStatic((HMODULE)DllBase);
            }

        }, this);

    return S_OK;
}
