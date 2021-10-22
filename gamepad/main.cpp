#include "universal.inl"
#include "dinput.h"
#include "xinput.h"
#include "rawinput.h"
#include "multimedia.h"


namespace Main
{
    EXTERN_C BOOL APIENTRY DllMainOnAttachProcess(_In_ HMODULE /*Module*/)
    {
#ifdef _DEBUG
        base::console::RedirectIOToConsole();
#endif

        // logging
        // %USERPROFILE%\xspy\log\Human_3647.log

        auto log_path = base::stdext::fs::expand_path(R"(%USERPROFILE%\xspy\log\)")
            .generic_string()
            .append(base::stdext::fs::module_path(GetModuleHandle(nullptr)).filename().replace_extension().string())
            .append("_")
            .append(std::to_string(GetCurrentProcessId()))
            .append(".log");

        spdlog::init_thread_pool(8192, 1);
        spdlog::flush_every(std::chrono::seconds(1));

        auto dup_filter = std::make_shared<spdlog::sinks::dup_filter_sink_mt>(std::chrono::seconds(5));

        auto sinks = spdlog::sinks_init_list
        {
            std::make_shared<spdlog::sinks::stdout_color_sink_mt >(),
            std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_path, 1024 * 1024 * 10, 3),
        };
        dup_filter->set_sinks(sinks);

        auto main_logger = std::make_shared<spdlog::async_logger>("Main", dup_filter, spdlog::thread_pool(), spdlog::async_overflow_policy::block);

        spdlog::register_logger   (main_logger);
        spdlog::set_default_logger(main_logger);

        spdlog::info("{} Called!", __FUNCTION__);

        // attach

        HRESULT Result = S_OK;
        do 
        {
            Result = base::ModuleNotification::GetInstance().Start();
            if (FAILED(Result))
            {
                break;
            }

            Result = MultiMediaSpy::GetInstance().Attach();
            if (FAILED(Result))
            {
                break;
            }

            Result = XNAInputSpy::GetInstance().Attach();
            if (FAILED(Result))
            {
                break;
            }

            Result = DirectInputSpy::GetInstance().Attach();
            if (FAILED(Result))
            {
                break;
            }

            Result = RawInputSpy::GetInstance().Attach();
            if (FAILED(Result))
            {
                break;
            }

        } while (false);

        return SUCCEEDED(Result);
    }

    EXTERN_C BOOL APIENTRY DllMainOnDetachProcess(_In_ HMODULE /*Module*/)
    {
        base::ModuleNotification::GetInstance().Stop();
        return TRUE;
    }

    EXTERN_C BOOL APIENTRY DllMainOnAttachThread(_In_ HMODULE /*Module*/)
    {
        return TRUE;
    }

    EXTERN_C BOOL APIENTRY DllMainOnDetachThread(_In_ HMODULE /*Module*/)
    {
        return TRUE;
    }

    EXTERN_C BOOL APIENTRY DllMain(
        _In_ HMODULE Module,
        _In_ DWORD  Reason,
        _In_opt_ LPVOID /*Reserved*/
    )
    {
        if (DetourIsHelperProcess())
            return TRUE;

        if (Reason == DLL_PROCESS_ATTACH)
            DetourRestoreAfterWith();

        switch (Reason)
        {
        case DLL_PROCESS_ATTACH: return DllMainOnAttachProcess(Module);
        case DLL_THREAD_ATTACH : return DllMainOnAttachThread (Module);
        case DLL_THREAD_DETACH : return DllMainOnDetachThread (Module);
        case DLL_PROCESS_DETACH: return DllMainOnDetachProcess(Module);
        }

        return TRUE;
    }
}
