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