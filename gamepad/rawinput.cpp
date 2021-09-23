#include "universal.inl"
#include "rawinput.h"


HRESULT RawInputSpy::Attach() noexcept
{
    return SUCCEEDED(HookOnStatic()) ? S_OK : HookOnDynamic();
}

HRESULT RawInputSpy::HookOnStatic(HMODULE /*module*/ /*= nullptr*/) noexcept
{
    return S_OK;
}

HRESULT RawInputSpy::HookOnDynamic() noexcept
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

            auto This = static_cast<RawInputSpy*>(Context);
            if (This == nullptr)
            {
                return;
            }

            auto DllBase = NotificationData->Loaded.DllBase;
            auto DllName = std::wstring(
                NotificationData->Loaded.BaseDllName->Buffer,
                NotificationData->Loaded.BaseDllName->Length / sizeof(wchar_t));

            if (DllName == base::mbstowcs(RawInputSpy::MODULE_NAME))
            {
                This->HookOnStatic((HMODULE)DllBase);
            }

        }, this);

    return S_OK;
}
