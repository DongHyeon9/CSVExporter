#include "HeaderGenerater.h"

void HeaderGenerater::Execute()
{
	std::string preprocessFile{ CreatePreprocess() };
	std::unordered_map<DataType, std::string> enumNames{};
	std::vector<std::string> enumTypesFiles{ CreateEnum(enumNames) };
	std::string structFile{ CreateStruct(enumNames) };

	std::ofstream outFile{ fileOutPath.c_str() };
	outFile << preprocessFile;
	for (const auto& enumType : enumTypesFiles)
	{
		outFile << enumType;
	}
	outFile << structFile;
	outFile.close();
}

std::string HeaderGenerater::CreatePreprocess()
{
	std::string result{ formats[static_cast<int32>(EHEADER_FORMAT::PRE_PROCESS)] };
	ExporterUtils::ReplaceString(result, MARK::FILE_NAME, fileName);
	return result;
}

std::vector<std::string> HeaderGenerater::CreateEnum(std::unordered_map<DataType, std::string>& _OutEnumNames)
{
	std::vector<std::string> result{};

	for (const auto& dataType : metaData.dataTypeList)
	{
		if (dataType.dataType != EDATA_TYPE::ENUM) continue;

		std::string enumName{ dataTypeFormats[EDATA_FORMAT::ENUM] };
		ExporterUtils::ReplaceString(enumName, MARK::ENUM_NAME, dataType.metaData);

		_OutEnumNames.emplace(std::pair<DataType, std::string>{ dataType ,enumName });

		std::string enumFile{ formats[static_cast<int32>(EHEADER_FORMAT::ENUM)] };
		std::string enumMembers{};

		for (const auto& em : dataType.enumSet)
		{
			std::string enumMember{ dataTypeFormats[EDATA_FORMAT::ENUM_MEMBER] };
			ExporterUtils::ReplaceString(enumMember, MARK::ENUM_MEMBER, em);
			enumMembers += enumMember + "\n\t";
		}

		enumMembers.erase(enumMembers.find_last_of("\n"));
		ExporterUtils::ReplaceString(enumFile, { {MARK::ENUM_NAME, enumName}, { MARK::ENUM_TYPES,enumMembers } });
		result.emplace_back(enumFile);
	}

	return result;
}

std::string HeaderGenerater::CreateStruct(const std::unordered_map<DataType, std::string>& _EnumNames)
{
	return std::string();
}

