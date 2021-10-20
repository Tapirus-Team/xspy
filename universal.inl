//Copyright 2021 MiroKaku
//
//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
//http ://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissionsand
//limitations under the License.


#pragma once

// Global define

// System Header
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define UMDF_USING_NTSTATUS
#include <windows.h>
#include <windowsx.h>
#include <winioctl.h>
#include <ntstatus.h>
#include <strsafe.h>

// C/C++ Header
#include <type_traits>
#include <string>
#include <memory>
#include <cwctype>
#include <algorithm>

// 3rdParty Header
#pragma warning(push)
#pragma warning(disable: 4702 4996)
#define SPDLOG_NO_EXCEPTIONS
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/dup_filter_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <detours/detours.h>
#include <base/include/libbase.h>
#pragma warning(pop)

#include "defhook.h"

// Global Var/Fun define
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#define NullMsgBox() MessageBox(0, 0, 0, 0)
