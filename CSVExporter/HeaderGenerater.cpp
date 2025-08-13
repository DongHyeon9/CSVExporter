#include "HeaderGenerater.h"

void HeaderGenerater::Execute()
{
	std::string preprocessFile{ CreatePreprocess() };
	std::unordered_map<DataType, std::string> enumNames{};
	std::vector<std::string> enumTypesFiles{ CreateEnum(enumNames) };
	std::string structFile{ CreateStruct(enumNames) };

	if (!SystemManager::GetInstance()->IsRebuild() && !(std::filesystem::exists(fileOutPath) && std::filesystem::is_regular_file(fileOutPath)))
		SystemManager::GetInstance()->SetRebuild(true);

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
		ExporterUtils::ReplaceString(enumName, MARK::ENUM_NAME, ExporterUtils::ToScreamingSnake(fileName) + "_" + dataType.metaData);

		_OutEnumNames.emplace(std::pair<DataType, std::string>{ dataType ,enumName });

		std::string enumFile{ formats[static_cast<int32>(EHEADER_FORMAT::ENUM)] };
		std::string enumMembers{};

		for (const auto& em : dataType.enumSet)
		{
			std::string enumMember{ dataTypeFormats[EDATA_FORMAT::ENUM_MEMBER] };
			ExporterUtils::ReplaceString(enumMember, MARK::ENUM_MEMBER, em);
			enumMembers += enumMember + "\n\t";
		}

		enumMembers += GLOBAL::ENUM_END;
		ExporterUtils::ReplaceString(enumFile, { {MARK::ENUM_NAME, enumName}, { MARK::ENUM_TYPES,enumMembers } });
		result.emplace_back(enumFile);
	}

	return result;
}

std::string HeaderGenerater::CreateStruct(const std::unordered_map<DataType, std::string>& _EnumNames)
{
	std::string result{ formats[static_cast<int32>(EHEADER_FORMAT::STRUCT)] };
	ExporterUtils::ReplaceString(result, MARK::STRUCT_NAME, fileName);

	std::string structFile{};
	for (size_t i = 1; i < metaData.dataTypeList.size(); ++i)
	{
		std::string structVar{ dataTypeFormats[EDATA_FORMAT::STRUCT_VAR] };
		if (metaData.dataTypeList[i].bIsArray)
			ExporterUtils::ReplaceString(structVar, MARK::DATA_TYPE, dataTypeFormats[EDATA_FORMAT::ARRAY]);

		if (metaData.dataTypeList[i].dataType == EDATA_TYPE::ENUM)
		{
			ExporterUtils::ReplaceString(structVar, MARK::DATA_TYPE, _EnumNames.at(metaData.dataTypeList[i]));
		}
		else
		{
			ExporterUtils::ReplaceString(structVar, MARK::DATA_TYPE, dataTypeFormats[static_cast<int32>(metaData.dataTypeList[i].dataType)]);
		}

		ExporterUtils::ReplaceString(structVar, MARK::VAR_NAME, metaData.dataTypeList[i].variableName);

		structFile += structVar + "\n\t";
	}


	ExporterUtils::ReplaceString(result, MARK::STRUCT_VARIABLES, structFile);
	return result;
}

