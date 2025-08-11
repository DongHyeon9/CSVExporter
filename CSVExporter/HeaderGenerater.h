#pragma once
#include "SystemManager.h"

// TODO
// UENUM 생성
// USTRUCT 생성
// h파일 생성

class HeaderGenerater
{
public:

protected:

private:
	enum EDATA_FORMAT
	{
		//data_type.fmt와 순서를 맞춰줘야됨
		INT = 0,
		FLOAT,
		STRING,
		ENUM,
		ARRAY,
		STRUCT_VAR,
		ENUM_MEMBER,

		END,
	};

	SheetMetaData metaData{};
	std::string fileName{};
	std::string fileOutPath{};
	std::array<std::string, static_cast<int32>(EHEADER_FORMAT::END)> formats{};
	std::array<std::string, EDATA_FORMAT::END> dataTypeFormats{};

public:
	template<EUSES _USES>
	bool Init(SheetMetaData _MetaData, std::string _FileName)
	{
		metaData = std::move(_MetaData);
		fileName = std::move(_FileName);

		auto usesIter{ metaData.usesList.begin() };
		auto dtIter{ metaData.dataTypeList.begin() };

		while (usesIter != metaData.usesList.end() && dtIter != metaData.dataTypeList.end()) 
		{
			if (*usesIter != EUSES::ALL && *usesIter != _USES) 
			{
				usesIter = metaData.usesList.erase(usesIter);
				dtIter = metaData.dataTypeList.erase(dtIter);
			}
			else 
			{
				++usesIter;
				++dtIter;
			}
		}

		if constexpr (_USES == EUSES::SERVER)			fileOutPath = SystemManager::GetInstance()->GetDir(EDIR_FLAG::HEADER | EDIR_FLAG::OUTPUT | EDIR_FLAG::SERVER) + fileName;
		else if constexpr (_USES == EUSES::CLIENT)		fileOutPath = SystemManager::GetInstance()->GetDir(EDIR_FLAG::HEADER | EDIR_FLAG::OUTPUT | EDIR_FLAG::CLIENT) + fileName;
		else											fileOutPath = SystemManager::GetInstance()->GetCurrentDir() + fileName;

		fileOutPath += GLOBAL::HEADER_POST_FIX;

		std::filesystem::path path(fileOutPath);
		if (path.has_parent_path())
			std::filesystem::create_directories(path.parent_path());

		std::string dir{};
		if constexpr (_USES == EUSES::CLIENT)
		{
			dir = SystemManager::GetInstance()->GetDir(EDIR_FLAG::CLIENT | EDIR_FLAG::FORMAT);
		}
		else if constexpr (_USES == EUSES::SERVER)
		{
			dir = SystemManager::GetInstance()->GetDir(EDIR_FLAG::SERVER | EDIR_FLAG::FORMAT);
		}
		ExporterUtils::NormalizeDir(dir);

		for (int32 i = 0; i < static_cast<int32>(EHEADER_FORMAT::END); ++i)
		{
			std::string filePath{ dir + HEADER_GEN::FORMAT_FILE_NAMES.at(i) };
			std::ifstream formatFile{ filePath.c_str() };
			if (!formatFile.is_open())
			{
				LOG("포맷 파일 %s가 없음", HEADER_GEN::FORMAT_FILE_NAMES.at(i).c_str());
				return false;
			}
			formats[i] = { (std::istreambuf_iterator<char>(formatFile)), (std::istreambuf_iterator<char>()) };
		}

		std::istringstream iss{ formats[static_cast<int32>(EHEADER_FORMAT::DATA_TYPE)] };
		std::string line{};
		for (int32 i = 0; i < EDATA_FORMAT::END; ++i)
		{
			if (std::getline(iss, line))
			{
				size_t idx{ line.find(DATA_TYPE::META_DATA_FLAG) };
				dataTypeFormats[i] = line.substr(idx + 1);
			}
		}
		return true;
	}
	void Execute();

protected:

private:
	std::string CreatePreprocess();
	std::vector<std::string> CreateEnum(std::unordered_map<DataType, std::string>& _OutEnumNames);
	std::string CreateStruct(const std::unordered_map<DataType, std::string>& _EnumNames);
};
