#pragma once
#include <iostream>
#include <string>
#include <format>
#include <Windows.h>

// デバッグマクロの定義
#ifdef _DEBUG
#define DEBUG_LOG(fmt, ...) \
        OutputDebugStringW(std::format(L"[{}] " fmt L"\n", L"DEBUG", ##__VA_ARGS__).c_str())

#define DEBUG_LOG_ERROR(fmt, ...) \
        OutputDebugStringW(std::format(L"[{}] {}({}): " fmt L"\n", L"ERROR", __FILEW__, __LINE__, ##__VA_ARGS__).c_str())
#else
#define DEBUG_LOG(fmt, ...)
#define DEBUG_LOG_ERROR(fmt, ...)
#endif