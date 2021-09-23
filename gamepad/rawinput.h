#pragma once


class RawInputSpy : public base::Singleton<RawInputSpy>
{
public:
    static constexpr char MODULE_NAME[] = u8"user32.dll";

public:
    HRESULT Attach() noexcept;

private:
    HRESULT HookOnStatic(HMODULE module = nullptr) noexcept;
    HRESULT HookOnDynamic() noexcept;
};
