#include "ExporterCore.h"

const std::string GLOBAL::SERVER_POST_FIX{ "_Server" };
const std::string GLOBAL::CLIENT_POST_FIX{ "_Client" };
const std::string GLOBAL::CSV_POST_FIX{ ".csv" };
const std::string GLOBAL::HEADER_POST_FIX{ ".h" };

const std::string GLOBAL::ERROR_NAME{ "Error" };
const std::string GLOBAL::COMMENT{ "#" };

const std::string GLOBAL::ENUM_END{ "END" };

const std::string INIT::EXPORTER_INI_FILE_NAME{ "exporter.ini" };

const std::string INIT::PROJECT_NAME_FLAG{ "[ProjectName]" };
const std::unordered_map<int32, std::string> INIT::OUTDIR_FLAG_MAP
{
	std::pair<int32,std::string>(EDIR_FLAG::CLIENT | EDIR_FLAG::OUTPUT | EDIR_FLAG::CSV,"[ClientAdditionalDir_CSV]"),
	std::pair<int32,std::string>(EDIR_FLAG::CLIENT | EDIR_FLAG::OUTPUT | EDIR_FLAG::HEADER,"[ClientAdditionalDir_Header]"),

	std::pair<int32,std::string>(EDIR_FLAG::CLIENT | EDIR_FLAG::FORMAT, "[ClientTypeFormat]"),
	std::pair<int32,std::string>(EDIR_FLAG::SERVER | EDIR_FLAG::FORMAT, "[ServerTypeFormat]"),

	std::pair<int32,std::string>(EDIR_FLAG::SERVER | EDIR_FLAG::OUTPUT | EDIR_FLAG::CSV,"[ServerAdditionalDir_CSV]"),
	std::pair<int32,std::string>(EDIR_FLAG::SERVER | EDIR_FLAG::OUTPUT | EDIR_FLAG::HEADER,"[ServerAdditionalDir_Header]"),
};

const std::string MARK::PROJECT_NAME{ "{ProjectName}" };
const std::string MARK::FILE_NAME{ "{FileName}" };
const std::string MARK::ENUM_NAME{ "{EnumName}" };
const std::string MARK::ENUM_MEMBER{ "{EnumMember}" };
const std::string MARK::ENUM_TYPES{ "{EnumTypes}" };
const std::string MARK::STRUCT_NAME{ "{StructName}" };
const std::string MARK::STRUCT_VARIABLES{ "{StructVariables}" };
const std::string MARK::DATA_TYPE{ "{DataType}" };
const std::string MARK::VAR_NAME{ "{VariableName}" };

const std::array<std::string, static_cast<int32>(EHEADER_FORMAT::END)> HEADER_GEN::FORMAT_FILE_NAMES
{
	"enum.fmt",			// ENUM
	"struct.fmt",		// STRUCT
	"pre_process.fmt",	// PRE_PROCESS
	"data_type.fmt",	// DATA_TYPE
};

const std::string USES::ALL{ "A" };
const std::string USES::CLIENT{ "C" };
const std::string USES::SERVER{ "S" };

const std::string DATA_TYPE::INT{ "int" };
const std::string DATA_TYPE::FLOAT{ "float" };
const std::string DATA_TYPE::ENUM{ "enum" };
const std::string DATA_TYPE::STRING{ "string" };
const std::string DATA_TYPE::ARRAY{ "[]" };
const std::string DATA_TYPE::META_DATA_FLAG{ ":" };

void ExporterUtils::NormalizeDir(std::string& _Path)
{
	if (_Path.back() != '\\')
		_Path.push_back('\\');
}

std::wstring ExporterUtils::UTF8ToWstring(const std::string& _UTF8)
{
	if (_UTF8.empty()) return {};
	int32 sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, _UTF8.data(), (int32)_UTF8.size(), nullptr, 0);
	if (sizeNeeded == 0)
		return {};

	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, _UTF8.data(), (int32)_UTF8.size(), &result[0], sizeNeeded);
	return result;
}

std::string ExporterUtils::WstringToUTF8(const std::wstring& _UTF16)
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

std::string ExporterUtils::UsesToString(EUSES _Uses)
{
	switch (_Uses)
	{
	case EUSES::ALL:		return USES::ALL;
	case EUSES::CLIENT:		return USES::CLIENT;
	case EUSES::SERVER:		return USES::SERVER;
	}
	assert(false);
}

std::string ExporterUtils::DataTypeToString(const DataType& _DataType)
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

EUSES ExporterUtils::StringToUses(const std::string& _Uses)
{
	if (ExporterUtils::CompareIgnoreCase(_Uses, USES::CLIENT))				return EUSES::CLIENT;
	else if (ExporterUtils::CompareIgnoreCase(_Uses, USES::SERVER))			return EUSES::SERVER;
	else if (ExporterUtils::CompareIgnoreCase(_Uses, USES::ALL))			return EUSES::ALL;
	else
	{
		LOG("알 수 없는 사용처 : %s", _Uses.c_str());
		assert(false);
	}
}

DataType ExporterUtils::StringToDataType(std::string _DataType)
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
		result.metaData = ExporterUtils::ToScreamingSnake(_DataType.substr(idx + 1));
		_DataType.erase(idx);
	}

	if (ExporterUtils::CompareIgnoreCase(_DataType, DATA_TYPE::STRING))			result.dataType = EDATA_TYPE::STRING;
	else if (ExporterUtils::CompareIgnoreCase(_DataType, DATA_TYPE::INT))		result.dataType = EDATA_TYPE::INT;
	else if (ExporterUtils::CompareIgnoreCase(_DataType, DATA_TYPE::FLOAT))		result.dataType = EDATA_TYPE::FLOAT;
	else if (ExporterUtils::CompareIgnoreCase(_DataType, DATA_TYPE::ENUM))		result.dataType = EDATA_TYPE::ENUM;
	else
	{
		LOG("알 수 없는 데이터 타입 : %s", _DataType.c_str());
		assert(false);
	}

	return result;
}

void ExporterUtils::ToLower(std::string& _Str)
{
	std::transform(_Str.begin(), _Str.end(), _Str.begin(), ::tolower);
}

void ExporterUtils::ToUpper(std::string& _Str)
{
	std::transform(_Str.begin(), _Str.end(), _Str.begin(), ::toupper);
}

bool ExporterUtils::CompareIgnoreCase(std::string _Lhs, std::string _Rhs)
{
	ExporterUtils::ToLower(_Lhs);
	ExporterUtils::ToLower(_Rhs);
	return _Lhs == _Rhs;
}

void ExporterUtils::UnparseEnumData(std::string _EnumData, std::set<std::string>& _OutSet)
{
	std::stringstream ss{ _EnumData };
	std::string token;

	while (std::getline(ss, token, ','))
		_OutSet.emplace(token);
}

std::string ExporterUtils::ToScreamingSnake(const std::string& _Input)
{
	//특수문자 제거
	std::string filtered{};
	filtered.reserve(_Input.size());
	for (unsigned char c : _Input) {
		if (std::isalnum(c)) {
			filtered.push_back(c);
		}
	}

	//SCREAMING_SNAKE_CASE 변환
	std::string result{};
	result.reserve(filtered.size() * 2);

	for (char c : filtered) {
		if (std::isupper(static_cast<unsigned char>(c))) {
			if (!result.empty()) {
				result.push_back('_');
			}
			result.push_back(c);
		}
		else {
			result.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
		}
	}

	return result;
}

void ExporterUtils::ReplaceString(std::string& _Target, const std::string& _From, const std::string& _To)
{
	size_t pos{};
	while ((pos = _Target.find(_From, pos)) != std::string::npos)
	{
		_Target.replace(pos, _From.length(), _To);
		pos += _To.length();
	}
}

void ExporterUtils::ReplaceString(std::string& _Target, const std::unordered_map<std::string, std::string>& _FromToMap)
{
	for (const auto& fromTo : _FromToMap)
	{
		ExporterUtils::ReplaceString(_Target, fromTo.first, fromTo.second);
	}
}
