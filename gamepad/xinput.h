#pragma once
#include <Xinput.h>


class XNAInputSpy : public base::Singleton<XNAInputSpy>
{
public:
    // xinput1_4.dll
    // xinput1_3.dll
    // xinput1_2.dll
    // xinput1_1.dll
    // xinput9_1_0.dll
    static constexpr char MODULE_NAME[] = u8"xinput";

public:
    HRESULT Attach() noexcept;

private:
    HRESULT HookOnStatic(HMODULE module = nullptr) noexcept;
    HRESULT HookOnDynamic() noexcept;
};

EXTERN_C DWORD WINAPI XInputGetStateEx(
    _In_  DWORD         dwUserIndex,
    _Out_ struct XINPUT_STATEEX* pState
);

EXTERN_C DWORD WINAPI XInputGetCapabilitiesEx(
    _In_  DWORD                dwAlways1,
    _In_  DWORD                dwUserIndex,
    _In_  DWORD                dwFlags,
    _Out_ struct XINPUT_CAPABILITIESEX* pCapabilities
);

EXTERN_C DWORD WINAPI XInputWaitForGuideButton(
    _In_  DWORD                dwUserIndex,
    _In_  DWORD                dwFlags,
    _In_  LPVOID               pUnkonwn
);

EXTERN_C DWORD WINAPI XInputCancelGuideButtonWait(
    _In_  DWORD                dwUserIndex
);

EXTERN_C DWORD WINAPI XInputPowerOffController(
    _In_  DWORD                dwUserIndex
);

EXTERN_C DWORD WINAPI XInputGetBaseBusInformation(
    _In_  DWORD                dwUserIndex,
    _Out_ struct XINPUT_BUSINFO* pBusInfo
);

namespace Hook
{
    namespace X9_1_0
    {
        DetourDefineMacro(DWORD, WINAPI, XInputGetCapabilities)(
            _In_  DWORD                dwUserIndex,                     // Index of the gamer associated with the device
            _In_  DWORD                dwFlags,                         // Input flags that identify the device type
            _Out_ XINPUT_CAPABILITIES* pCapabilities                    // Receives the capabilities
            );

        DetourDefineMacro(DWORD, WINAPI, XInputSetState)(
            _In_ DWORD             dwUserIndex,                         // Index of the gamer associated with the device
            _In_ XINPUT_VIBRATION* pVibration                           // The vibration information to send to the controller
            );

        DetourDefineMacro(DWORD, WINAPI, XInputGetState)(
            _In_  DWORD         dwUserIndex,                            // Index of the gamer associated with the device
            _Out_ XINPUT_STATE* pState                                  // Receives the current state
            );
    }

    namespace X1_1
    {
        DetourDefineMacro(DWORD, WINAPI, XInputGetCapabilities)(
            _In_  DWORD                dwUserIndex,                     // Index of the gamer associated with the device
            _In_  DWORD                dwFlags,                         // Input flags that identify the device type
            _Out_ XINPUT_CAPABILITIES* pCapabilities                    // Receives the capabilities
            );

        DetourDefineMacro(DWORD, WINAPI, XInputSetState)(
            _In_ DWORD             dwUserIndex,                         // Index of the gamer associated with the device
            _In_ XINPUT_VIBRATION* pVibration                           // The vibration information to send to the controller
            );

        DetourDefineMacro(DWORD, WINAPI, XInputGetState)(
            _In_  DWORD         dwUserIndex,                            // Index of the gamer associated with the device
            _Out_ XINPUT_STATE* pState                                  // Receives the current state
            );
    }

    namespace X1_2
    {
        DetourDefineMacro(DWORD, WINAPI, XInputGetCapabilities)(
            _In_  DWORD                dwUserIndex,                     // Index of the gamer associated with the device
            _In_  DWORD                dwFlags,                         // Input flags that identify the device type
            _Out_ XINPUT_CAPABILITIES* pCapabilities                    // Receives the capabilities
            );

        DetourDefineMacro(DWORD, WINAPI, XInputSetState)(
            _In_ DWORD             dwUserIndex,                         // Index of the gamer associated with the device
            _In_ XINPUT_VIBRATION* pVibration                           // The vibration information to send to the controller
            );

        DetourDefineMacro(DWORD, WINAPI, XInputGetState)(
            _In_  DWORD         dwUserIndex,                            // Index of the gamer associated with the device
            _Out_ XINPUT_STATE* pState                                  // Receives the current state
            );
    }

    namespace X1_3
    {
        DetourDefineMacro(DWORD, WINAPI, XInputGetBatteryInformation)(
            _In_  DWORD                       dwUserIndex,              // Index of the gamer associated with the device
            _In_  BYTE                        devType,                  // Which device on this user index
            _Out_ XINPUT_BATTERY_INFORMATION* pBatteryInformation       // Contains the level and types of batteries
            );

        DetourDefineMacro(DWORD, WINAPI, XInputGetCapabilities)(
            _In_  DWORD                dwUserIndex,                     // Index of the gamer associated with the device
            _In_  DWORD                dwFlags,                         // Input flags that identify the device type
            _Out_ XINPUT_CAPABILITIES* pCapabilities                    // Receives the capabilities
            );

        DetourDefineMacro(DWORD, WINAPI, XInputSetState)(
            _In_ DWORD             dwUserIndex,                         // Index of the gamer associated with the device
            _In_ XINPUT_VIBRATION* pVibration                           // The vibration information to send to the controller
            );

        DetourDefineMacro(DWORD, WINAPI, XInputGetState)(
            _In_  DWORD         dwUserIndex,                            // Index of the gamer associated with the device
            _Out_ XINPUT_STATE* pState                                  // Receives the current state
            );

        DetourDefineMacro(DWORD, WINAPI, XInputGetStateEx)(
            _In_  DWORD         dwUserIndex,                            // Index of the gamer associated with the device
            _Out_ struct XINPUT_STATEEX* pState                         // Receives the current state
            );

        DetourDefineMacro(DWORD, WINAPI, XInputGetKeystroke)(
            _In_       DWORD dwUserIndex,                               // Index of the gamer associated with the device
            _Reserved_ DWORD dwReserved,                                // Reserved for future use
            _Out_      PXINPUT_KEYSTROKE pKeystroke                     // Pointer to an XINPUT_KEYSTROKE structure that receives an input event.
            );
    }

    namespace X1_4
    {
        DetourDefineMacro(DWORD, WINAPI, XInputGetBaseBusInformation)(
            _In_  DWORD                dwUserIndex,
            _Out_ struct XINPUT_BUSINFO* pBusInfo
            );

        DetourDefineMacro(DWORD, WINAPI, XInputGetBatteryInformation)(
            _In_  DWORD                       dwUserIndex,              // Index of the gamer associated with the device
            _In_  BYTE                        devType,                  // Which device on this user index
            _Out_ XINPUT_BATTERY_INFORMATION* pBatteryInformation       // Contains the level and types of batteries
            );

        DetourDefineMacro(DWORD, WINAPI, XInputGetAudioDeviceIds)(
            _In_  DWORD                             dwUserIndex,        // Index of the gamer associated with the device
            _Out_writes_opt_(*pRenderCount) LPWSTR  pRenderDeviceId,    // Windows Core Audio device ID string for render (speakers)
            _Inout_opt_ UINT* pRenderCount,                             // Size of render device ID string buffer (in wide-chars)
            _Out_writes_opt_(*pCaptureCount) LPWSTR pCaptureDeviceId,   // Windows Core Audio device ID string for capture (microphone)
            _Inout_opt_ UINT* pCaptureCount                             // Size of capture device ID string buffer (in wide-chars)
            );

        DetourDefineMacro(DWORD, WINAPI, XInputGetCapabilities)(
            _In_  DWORD                dwUserIndex,                     // Index of the gamer associated with the device
            _In_  DWORD                dwFlags,                         // Input flags that identify the device type
            _Out_ XINPUT_CAPABILITIES* pCapabilities                    // Receives the capabilities
            );

        DetourDefineMacro(DWORD, WINAPI, XInputGetCapabilitiesEx)(
            _In_  DWORD                dwAlways1,
            _In_  DWORD                dwUserIndex,                     // Index of the gamer associated with the device
            _In_  DWORD                dwFlags,                         // Input flags that identify the device type
            _Out_ struct XINPUT_CAPABILITIESEX* pCapabilities           // Receives the capabilities
            );

        DetourDefineMacro(DWORD, WINAPI, XInputSetState)(
            _In_ DWORD             dwUserIndex,                         // Index of the gamer associated with the device
            _In_ XINPUT_VIBRATION* pVibration                           // The vibration information to send to the controller
            );

        DetourDefineMacro(DWORD, WINAPI, XInputGetState)(
            _In_  DWORD         dwUserIndex,                            // Index of the gamer associated with the device
            _Out_ XINPUT_STATE* pState                                  // Receives the current state
            );

        DetourDefineMacro(DWORD, WINAPI, XInputGetStateEx)(
            _In_  DWORD         dwUserIndex,                            // Index of the gamer associated with the device
            _Out_ struct XINPUT_STATEEX* pState                         // Receives the current state
            );

        DetourDefineMacro(DWORD, WINAPI, XInputGetKeystroke)(
            _In_       DWORD dwUserIndex,                               // Index of the gamer associated with the device
            _Reserved_ DWORD dwReserved,                                // Reserved for future use
            _Out_      PXINPUT_KEYSTROKE pKeystroke                     // Pointer to an XINPUT_KEYSTROKE structure that receives an input event.
            );
    }
}
