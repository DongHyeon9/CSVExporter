#include "ExporterCore.h"

const std::string GLOBAL::SERVER_POST_FIX{ "_Server" };
const std::string GLOBAL::CLIENT_POST_FIX{ "_Client" };
const std::string GLOBAL::CSV_POST_FIX{ ".csv" };

const std::string GLOBAL::ERROR_NAME{ "Error" };
const std::string GLOBAL::COMMENT{ "#" };

const std::string GLOBAL::EXPORTER_INI_FILE_NAME{ "exporter.ini" };

std::string GLOBAL::CLIENT_CSV_OUT_PUT_DIR{};
std::string GLOBAL::SERVER_CSV_OUT_PUT_DIR{};

std::string GLOBAL::CLIENT_HEADER_OUT_PUT_DIR{};
std::string GLOBAL::SERVER_HEADER_OUT_PUT_DIR{};

const std::string USES::ALL{ "A" };
const std::string USES::CLIENT{ "C" };
const std::string USES::SERVER{ "S" };

const std::string DATA_TYPE::INT{ "int" };
const std::string DATA_TYPE::FLOAT{ "float" };
const std::string DATA_TYPE::ENUM{ "enum" };
const std::string DATA_TYPE::STRING{ "string" };
const std::string DATA_TYPE::ARRAY{ "[]" };
const std::string DATA_TYPE::META_DATA_FLAG{ ":" };

bool InitSystem()
{
	_wsetlocale(LC_ALL, TEXT("korean"));
	LOG("언어설정 : 한국");

	GLOBAL::CURRENT_DIR = std::filesystem::current_path().string();
	LOG("현재 경로 : %s", GLOBAL::CURRENT_DIR.c_str());

	GLOBAL::SERVER_CSV_OUT_PUT_DIR = GLOBAL::CURRENT_DIR;
	GLOBAL::CLIENT_CSV_OUT_PUT_DIR = GLOBAL::CURRENT_DIR;

	GLOBAL::CLIENT_CSV_OUT_PUT_DIR.erase(GLOBAL::CURRENT_DIR.find_last_of("/"));
	NormalizeDir(GLOBAL::CLIENT_CSV_OUT_PUT_DIR);

	std::ifstream csvOutputDirFile{ GLOBAL::CLIENT_CSV_OUTPUT_DIR_FILE_NAME.c_str() };
	if (!csvOutputDirFile.is_open())
	{
		LOG("CSV 출력 경로알려주는 %s파일이 없음", GLOBAL::CLIENT_CSV_OUTPUT_DIR_FILE_NAME.c_str());
		return false;
	}

	GLOBAL::CLIENT_CSV_ADDITIONAL_DIR = { (std::istreambuf_iterator<char>(csvOutputDirFile)), (std::istreambuf_iterator<char>()) };
	NormalizeDir(GLOBAL::CLIENT_CSV_ADDITIONAL_DIR);

	LOG("CSV 출력 경로 : %s", GLOBAL::CLIENT_CSV_ADDITIONAL_DIR.c_str());

	csvOutputDirFile.close();

	GLOBAL::CLIENT_CSV_OUT_PUT_DIR += GLOBAL::CLIENT_CSV_ADDITIONAL_DIR;

	LOG("시스템 초기화 성공!");
	return true;
}

bool CreateClientDir()
{
	return true;
}

bool CreateServerDir()
{
	return true;
}

void NormalizeDir(std::string& _Path)
{
	if (_Path.back() != '\\')
		_Path.push_back('\\');
}

std::wstring UTF8ToWstring(const std::string& _UTF8)
{
	if (_UTF8.empty()) return {};
	int32 sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, _UTF8.data(), (int32)_UTF8.size(), nullptr, 0);
	if (sizeNeeded == 0)
		return {};

	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, _UTF8.data(), (int32)_UTF8.size(), &result[0], sizeNeeded);
	return result;
}

std::string WstringToUTF8(const std::wstring& _UTF16)
{
	if (_UTF16.empty()) return {};
	int32 requiredBytes = WideCharToMultiByte(
		CP_UTF8, 0,
		_UTF16.data(), (int32)_UTF16.size(),
		nullptr, 0,
		nullptr, nullptr
	);
	if (requiredBytes == 0) {
		throw std::system_error(GetLastError(), std::system_category(), "WideCharToMultiByte failed");
	}
	std::string utf8(requiredBytes, '\0');
	WideCharToMultiByte(
		CP_UTF8, 0,
		_UTF16.data(), (int32)_UTF16.size(),
		&utf8[0], requiredBytes,
		nullptr, nullptr
	);
	return utf8;
}

std::string UsesToString(EUSES _Uses)
{
	switch (_Uses)
	{
	case EUSES::ALL:		return USES::ALL;
	case EUSES::CLIENT:		return USES::CLIENT;
	case EUSES::SERVER:		return USES::SERVER;
	}
	assert(false);
}

std::string DataTypeToString(const DataType& _DataType)
{
	std::string result{ GLOBAL::ERROR_NAME };
	switch (_DataType.dataType)
	{
	case EDATA_TYPE::INT:			result = DATA_TYPE::INT;			break;
	case EDATA_TYPE::FLOAT:			result = DATA_TYPE::FLOAT;			break;
	case EDATA_TYPE::STRING:		result = DATA_TYPE::STRING;			break;
	case EDATA_TYPE::ENUM:			result = DATA_TYPE::ENUM;			break;
	default: { assert(false); }
	}
	if (_DataType.bIsArray) 	result += DATA_TYPE::ARRAY;
	return result;
}

EUSES StringToUses(const std::string& _Uses)
{
	if (CompareIgnoreCase(_Uses, USES::CLIENT))					return EUSES::CLIENT;
	else if (CompareIgnoreCase(_Uses, USES::SERVER))			return EUSES::SERVER;
	else if (CompareIgnoreCase(_Uses, USES::ALL))				return EUSES::ALL;
	else 
	{
		LOG("알 수 없는 사용처 : %s", _Uses.c_str());
		assert(false);
	}
}

DataType StringToDataType(std::string _DataType)
{
	DataType result{};
	size_t idx{ _DataType.find(DATA_TYPE::ARRAY) };
	if (idx != std::string::npos)
	{
		result.bIsArray = true;
		_DataType.erase(idx, DATA_TYPE::ARRAY.length());
	}

	idx = _DataType.find(DATA_TYPE::META_DATA_FLAG);
	if (idx != std::string::npos)
	{
		result.metaData = _DataType.substr(idx + 1);
		_DataType.erase(idx);
	}

	if (CompareIgnoreCase(_DataType, DATA_TYPE::STRING))		result.dataType = EDATA_TYPE::STRING;
	else if (CompareIgnoreCase(_DataType, DATA_TYPE::INT))		result.dataType = EDATA_TYPE::INT;
	else if (CompareIgnoreCase(_DataType, DATA_TYPE::FLOAT))	result.dataType = EDATA_TYPE::FLOAT;
	else if (CompareIgnoreCase(_DataType, DATA_TYPE::ENUM))		result.dataType = EDATA_TYPE::ENUM;
	else 
	{ 
		LOG("알 수 없는 데이터 타입 : %s", _DataType.c_str());
		assert(false); 
	}
	
	return result;
}

void ToLower(std::string& _Str)
{
	std::transform(_Str.begin(), _Str.end(), _Str.begin(), ::tolower);
}

void ToUpper(std::string& _Str)
{
	std::transform(_Str.begin(), _Str.end(), _Str.begin(), ::toupper);
}

bool CompareIgnoreCase(std::string _Lhs, std::string _Rhs)
{
	ToLower(_Lhs);
	ToLower(_Rhs);
	return _Lhs == _Rhs;
}

void UnparseEnumData(std::string _EnumData, std::set<std::string>& _OutSet)
{
	// TODO
	// enum 배열에 대한 값 처리
}