#pragma once

#pragma warning( disable : 4083 )
#pragma warning( disable : 4819 )
#pragma warning( disable : 4996 )

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
#include <cmath>

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

enum EDIR_FLAG
{
	CURRENT		=		1 << 1,
	OUTPUT		=		1 << 2,

	CSV			=		1 << 3,
	HEADER		=		1 << 4,
	FORMAT		=		1 << 5,

	CLIENT		=		1 << 6,
	SERVER		=		1 << 7,
};

enum class EHEADER_FORMAT : uint8
{
	ENUM = 0,
	STRUCT,
	PRE_PROCESS,
	DATA_TYPE,

	END,
};

enum class EDATA_TYPE : uint8
{
	INT=1<<1,
	FLOAT=1<<2,
	STRING=1<<3,
	ENUM=1<<4,
};

enum class EUSES : uint8
{
	ALL,
	CLIENT,
	SERVER,
};

enum class ELINE_TYPE : uint8
{
	ROW,
	COLUMN,
};
#pragma endregion Enum

#pragma region Struct
struct ComInit
{
	ComInit() { CoInitialize(nullptr); }
	~ComInit() { CoUninitialize(); }
};

struct DataType
{
	EDATA_TYPE dataType{};
	bool bIsArray{};
	std::string metaData{};
	std::set<std::string> enumSet{};
};

struct Point
{
	uint32 x{};
	uint32 y{};

	Point() :x(0), y(0) {}
	Point(int32 _X, int32 _Y) :x(_X), y(_Y) {}
	Point(const Point& _Other) :x(_Other.x), y(_Other.y) {}
	Point(Point&& _Other)noexcept :x(std::move(_Other.x)), y(std::move(_Other.y)) {}
	Point& operator=(const Point& _Other) { x = _Other.x; y = _Other.y; return *this; }
	Point& operator=(Point&& _Other)noexcept { x = std::move(_Other.x); y = std::move(_Other.y); return *this; }
};

struct LineInfo
{
	ELINE_TYPE type{};
	uint32 lineIdx{};
	uint32 start{};
	uint32 end{};
	uint32 GetCount()const { return end - start; }

	bool operator==(const LineInfo& _Other) const { return type == _Other.type && lineIdx == _Other.lineIdx && start == _Other.start && end == _Other.end; }
};

struct SheetMetaData
{
	std::vector<EUSES> usesList{};
	std::vector<DataType> dataTypeList{};
};

struct SheetInfo
{
	Point rightBottom{};
	LineInfo idRow{};
	LineInfo dataTypeRow{};
	LineInfo usesRow{};
	SheetMetaData metaData{};
	std::vector<std::vector<std::string>> csv{};
};

// 해시 생성
namespace std
{
	template <>
	struct hash<LineInfo>
	{
		size_t operator()(const LineInfo& _Ref) const noexcept
		{
			using std::hash;
			size_t h1 = hash<uint8>()(static_cast<uint8>(_Ref.type));
			size_t h2 = hash<uint32>()(_Ref.lineIdx);
			size_t h3 = hash<uint32>()(_Ref.start);
			size_t h4 = hash<uint32>()(_Ref.end);

			size_t res{ h1 };
			res ^= (h2 << 1);
			res ^= (h3 << 2);
			res ^= (h4 << 3);
			return res;
		}
	};

	template <>
	struct hash<DataType>
	{
		size_t operator()(const DataType& _Ref) const noexcept
		{
			using std::hash;
			size_t h1 = hash<uint32>()(static_cast<uint32>(_Ref.dataType));
			size_t h2 = hash<uint32>()(_Ref.bIsArray);

			return h1 | h2;
		}
	};
}
#pragma endregion Struct

#pragma region Preprocess
#ifdef _DEBUG
#define LOG_LINE std::cout << __FUNCTION__ << "(" << __LINE__ << ") ";
#else
#define LOG_LINE ((void)0)
#endif
#define LOG(str, ...) LOG_LINE; printf_s(str, ##__VA_ARGS__);std::cout << std::endl
#define CHECK(expr, ret, str) if(!(expr)) {LOG(str); return ret;}
#define END_OF_PROGRAM(ret) std::cout << "Please Press Any Key"; while (true) if (_kbhit()) break; return ret
#pragma endregion Preprocess

#pragma region Value
namespace GLOBAL
{
	extern const std::string SERVER_POST_FIX;
	extern const std::string CLIENT_POST_FIX;
	extern const std::string CSV_POST_FIX;

	extern const std::string ERROR_NAME;
	extern const std::string COMMENT;
}

namespace INIT
{
	extern const std::string EXPORTER_INI_FILE_NAME;

	extern const std::string PROJECT_NAME_FLAG;
	extern const std::unordered_map<int32, std::string> OUTDIR_FLAG_MAP;
}

namespace MARK
{
	extern const std::string PROJECT_NAME;
}

namespace HEADER_GEN
{
	extern const std::array<std::string, static_cast<int32>(EHEADER_FORMAT::END)> FORMAT_FILE_NAMES;
}

namespace USES
{
	extern const std::string ALL;
	extern const std::string CLIENT;
	extern const std::string SERVER;
}

namespace DATA_TYPE
{
	extern const std::string INT;
	extern const std::string FLOAT;
	extern const std::string ENUM;
	extern const std::string STRING;
	extern const std::string ARRAY;
	extern const std::string META_DATA_FLAG;
}
#pragma endregion Value

#pragma region Function
void NormalizeDir(std::string& _Path);
std::wstring UTF8ToWstring(const std::string& _UTF8);
std::string WstringToUTF8(const std::wstring& _UTF16);
std::string UsesToString(EUSES _Uses);
std::string DataTypeToString(const DataType& _DataType);
EUSES StringToUses(const std::string& _Uses);
DataType StringToDataType(std::string _DataType);
void ToLower(std::string& _Str);
void ToUpper(std::string& _Str);
bool CompareIgnoreCase(std::string _Lhs, std::string _Rhs);
void UnparseEnumData(std::string _EnumData, std::set<std::string>& _OutSet);
std::string ToScreamingSnake(const std::string& _Input);
void ReplaceString(std::string& _Target, const std::string& _From, const std::string& _To);
void ReplaceString(std::string& _Target, const std::unordered_map<std::string, std::string>& _FromToMap);
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