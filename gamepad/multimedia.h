#pragma once
#include <mmsystem.h>


class MultiMediaSpy : public base::Singleton<MultiMediaSpy>
{
public:
    static constexpr char MODULE_NAME[] = u8"winmm.dll";

public:
    HRESULT Attach() noexcept;

private:
    HRESULT HookOnStatic (HMODULE module = nullptr) noexcept;
    HRESULT HookOnDynamic() noexcept;
};

namespace Hook
{
    DetourDefineMacro(MMRESULT, WINAPI, joyGetPosEx)(
        _In_ UINT uJoyID,
        _Out_ LPJOYINFOEX pji
        );

    DetourDefineMacro(UINT, WINAPI, joyGetNumDevs)(
        void
        );

    DetourDefineMacro(MMRESULT, WINAPI, joyGetDevCapsW)(
        _In_ UINT_PTR uJoyID,
        _Out_writes_bytes_(cbjc) LPJOYCAPSW pjc,
        _In_ UINT cbjc
        );

    DetourDefineMacro(MMRESULT, WINAPI, joySetCapture)(
        _In_ HWND hwnd,
        _In_ UINT uJoyID,
        _In_ UINT uPeriod,
        _In_ BOOL fChanged
        );

    DetourDefineMacro(MMRESULT, WINAPI, joyReleaseCapture)(
        _In_ UINT uJoyID
        );

    DetourDefineMacro(MMRESULT, WINAPI, joySetThreshold)(
        _In_ UINT uJoyID,
        _In_ UINT uThreshold
        );

    DetourDefineMacro(MMRESULT, WINAPI, joyGetThreshold)(
        _In_ UINT uJoyID,
        _Out_ LPUINT puThreshold
        );

    DetourDefineMacro(MMRESULT, WINAPI, joyConfigChanged)(
        _In_ DWORD dwFlags
        );
}
