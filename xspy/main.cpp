#include "universal.inl"
#include <shellapi.h>


namespace std
{
    namespace fs = filesystem;
}

HRESULT Launcher(
    _In_ std::wstring DllName,
    _In_ std::wstring Application,
    _In_ std::wstring Args,
    _Out_opt_ HANDLE* ProcessHandle,
    _Out_opt_ HANDLE* ThreadHandle
)
{
    HRESULT Result = S_OK;

    do
    {
        auto DllPath = base::stdext::fs::module_path().parent_path()
            .append(DllName);

        if (!Args.empty())
        {
            Application.append(1, L' ').append(Args);
        }

        STARTUPINFOW StartupArgs = { sizeof(STARTUPINFOW) };
        PROCESS_INFORMATION ProcessInformation = { nullptr };

        if (!DetourCreateProcessWithDllExW(
            nullptr,
            Application.data(),
            nullptr,
            nullptr,
            FALSE,
            CREATE_NEW_CONSOLE,
            nullptr,
            std::fs::path(Application).parent_path().c_str(),
            &StartupArgs,
            &ProcessInformation,
            DllPath.generic_string().c_str(),
            nullptr))
        {
            Result = HRESULT_FROM_WIN32(GetLastError());
            break;
        }

        if (ProcessHandle)
            *ProcessHandle = ProcessInformation.hProcess;
        else
            CloseHandle(ProcessInformation.hProcess);

        if (ThreadHandle)
            *ThreadHandle = ProcessInformation.hThread;
        else
            CloseHandle(ProcessInformation.hThread);

    } while (false);

    return Result;
}

HRESULT DuplicateCommandLine(
    _In_  uint32_t Position,
    _In_  uint32_t PositionEnd,
    _Out_ std::wstring& Application,
    _Out_ std::wstring& Args
)
{
    UINT32  Argc = 0;
    LPWSTR* Argv = CommandLineToArgvW(GetCommandLineW(), (INT*)&Argc);
    if (Argv == nullptr)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    if (Position > Argc)
    {
        return E_INVALIDARG;
    }

    if (PositionEnd == (uint32_t)~0 || PositionEnd > Argc)
    {
        PositionEnd = Argc;
    }

    Application = Argv[Position];
    Args.clear();

    for (auto i = Position + 1; i < PositionEnd; ++i)
    {
        Args.append(Argv[i]).append(L" ");
    }
    if (!Args.empty() && Args.back() == L' ') Args.pop_back();

    if (Argv)
    {
        LocalFree(Argv);
    }

    return S_OK;
}

namespace Main
{
    extern "C" int __cdecl wmain(
        int argc, wchar_t* argv[]
    )
    {
        auto DllName = argv[1];

        std::wstring Application;
        std::wstring Args;

        auto Result = DuplicateCommandLine(2, ~0u, Application, Args);
        if (FAILED(Result))
        {
            return Result;
        }

        return Launcher(DllName, Application, Args, nullptr, nullptr);
    }
}
