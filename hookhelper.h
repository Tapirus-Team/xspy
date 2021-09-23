#pragma once


// Init
#define DetourInitFunc(Module, FunName) \
    __##FunName = (decltype(::FunName)*)GetProcAddress(Module, #FunName)

#define DetourInitFuncModName(ModuleName, FunName) \
    __##FunName = (decltype(::FunName)*)GetProcAddress(GetModuleHandleA(ModuleName), #FunName)

#define DetourInitFuncEx(Module, FunName, Ordinal) \
    __##FunName = (decltype(::FunName)*)GetProcAddress(Module, (LPSTR)Ordinal)

#define DetourInitFuncExModName(ModuleName, FunName, Ordinal) \
    __##FunName = (decltype(::FunName)*)GetProcAddress(GetModuleHandleA(ModuleName), (LPSTR)Ordinal)

#define DetourInitFuncOffset(Module, FunName, FunOffset) \
    __##FunName = (decltype(::FunName)*)(void*)((uint8_t*)Module + FunOffset)

#define DetourInitFuncAddress(Module, FunName, FunAddress) \
    __##FunName = (decltype(::FunName)*)(void*)FunAddress

// Def & Make detour function.
#define DetourDefineMacro(ReturnT, CallT, FunName) \
    __declspec(selectany) extern auto __##FunName = (decltype(::FunName)*)nullptr; \
    ReturnT CallT Detour$##FunName

#define DetourMakeMacro(ReturnT, CallT, FunName) \
    ReturnT CallT Detour$##FunName

// Call
#define DetourCallMacro(FunName) \
    __##FunName

#define DetourCallMacroEx(FunName, ErrorReturn) \
    (!__##FunName) ? ErrorReturn : __##FunName

// Begin & Commit
#define DetourBegin()  DetourTransactionBegin();DetourUpdateThread(GetCurrentThread())

#define DetourCommit() DetourTransactionCommit()

// Attach & Detach
#define DetourAttachMacro(Module, FunName) \
    DetourInitFunc(Module, FunName); \
    __##FunName ? DetourAttach(&(PVOID&)__##FunName, Detour$##FunName) : ERROR_NOT_SUPPORTED

#define DetourDetachMacro(Module, FunName) \
    __##FunName ? DetourDetach(&(PVOID&)__##FunName, Detour$##FunName) : ERROR_NOT_SUPPORTED

#define DetourAttachMacroModName(ModuleName, FunName) \
    DetourInitFuncModName(ModuleName, FunName); \
    __##FunName ? DetourAttach(&(PVOID&)__##FunName, Detour$##FunName) : ERROR_NOT_SUPPORTED

#define DetourDetachMacroModName(Module, FunName) \
    __##FunName ? DetourDetach(&(PVOID&)__##FunName, Detour$##FunName) : ERROR_NOT_SUPPORTED

#define DetourAttachMacroEx(Module, FunName, Ordinal) \
    DetourInitFuncEx(Module, FunName, Ordinal); \
    __##FunName ? DetourAttach(&(PVOID&)__##FunName, Detour$##FunName) : ERROR_NOT_SUPPORTED

#define DetourDetachMacroEx(Module, FunName, Ordinal) \
    __##FunName ? DetourDetach(&(PVOID&)__##FunName, Detour$##FunName) : ERROR_NOT_SUPPORTED

#define DetourAttachMacroExModName(ModuleName, FunName, Ordinal) \
    DetourInitFuncExModName(ModuleName, FunName, Ordinal); \
    __##FunName ? DetourAttach(&(PVOID&)__##FunName, Detour$##FunName) : ERROR_NOT_SUPPORTED

#define DetourDetachMacroExModName(Module, FunName, Ordinal) \
    __##FunName ? DetourDetach(&(PVOID&)__##FunName, Detour$##FunName) : ERROR_NOT_SUPPORTED

#define DetourAttachMacroOffset(Module, FunName, FunOffset) \
    DetourInitFuncOffset(Module, FunName, FunOffset); \
    __##FunName ? DetourAttach(&(PVOID&)__##FunName, Detour$##FunName) : ERROR_NOT_SUPPORTED

#define DetourDetachMacroOffset(Module, FunName, FunOffset) \
    __##FunName ? DetourDetach(&(PVOID&)__##FunName, Detour$##FunName) : ERROR_NOT_SUPPORTED

#define DetourAttachMacroAddress(Module, FunName, FunAddress) \
    DetourInitFuncAddress(Module, FunName, FunAddress); \
    __##FunName ? DetourAttach(&(PVOID&)__##FunName, Detour$##FunName) : ERROR_NOT_SUPPORTED

#define DetourDetachMacroAddress(Module, FunName, FunAddress) \
    __##FunName ? DetourDetach(&(PVOID&)__##FunName, Detour$##FunName) : ERROR_NOT_SUPPORTED
