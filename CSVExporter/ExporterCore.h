#pragma once

#pragma region Header
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <conio.h>
#include <locale>

#include <windows.h>
#include <shobjidl.h>
#include <atlbase.h>
#include <io.h>
#include <fcntl.h>
#include "OpenXLSX.hpp"
#pragma endregion Header

#pragma region Type
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
#pragma endregion Type

#pragma region Enum
#pragma endregion Enum

#pragma region Value
#pragma endregion Value

#pragma region Struct
struct ComInit 
{
    ComInit() { CoInitialize(nullptr); }
    ~ComInit() { CoUninitialize(); }
};
#pragma endregion Struct

#pragma region Preprocess
#ifdef _DEBUG
#define LOG_LINE std::wcout << __FUNCTION__ << "(" << __LINE__ << ") ";
#else
#define LOG_LINE ((void)0)
#endif
#define LOG(str, ...) LOG_LINE wprintf_s(TEXT(str), ##__VA_ARGS__);std::wcout << std::endl
#define CHECK(expr, ret, str) if(!(expr)) {LOG(str); return ret;}
#define END_OF_PROGRAM(ret) std::wcout << TEXT("Please Press Any Key"); while (true) if (_kbhit()) break; return ret
#define TYPE(container) decltype(*container.data())
#pragma endregion Preprocess

#pragma region Global
namespace GLOBAL
{
    extern const std::string SERVER_POST_FIX_NAME;
    extern const std::string CLIENT_POST_FIX_NAME;
    extern const std::string CSV_OUTPUT_DIR_FILE_NAME;
    extern std::string CURRENT_DIR;
}
#pragma endregion Global

#pragma region Function
bool InitSystem();
void NormalizeDir(std::string& _Path);
std::wstring UTF8ToWstring(const std::string& _UTF8);
std::string WstringToUTF8(const std::wstring& _UTF16);
template<class _Dst, class _Src>
_Dst ConvertString(const _Src& String)
{
	if constexpr (std::is_same_v<_Dst, _Src>)
		return String;

    if constexpr (std::is_same_v<_Dst, std::wstring> && std::is_same_v<_Src, std::string>)
        return UTF8ToWstring(String);

    else if constexpr (std::is_same_v<_Dst, std::string> && std::is_same_v<_Src, std::wstring>)
        return WstringToUTF8(String);
}
#pragma endregion Function