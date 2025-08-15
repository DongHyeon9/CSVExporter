#include "HeaderGenerater.h"

void HeaderGenerater::Execute(SheetMetaData _MetaData, std::string _FileName)
{
	std::string fileName{ std::move(_FileName) };
	SheetMetaData metaData{ std::move(_MetaData) };

	auto usesIter{ metaData.usesList.begin() };
	auto dtIter{ metaData.dataTypeList.begin() };

	while (usesIter != metaData.usesList.end() && dtIter != metaData.dataTypeList.end())
	{
		if (*usesIter != EUSES::NONE && *usesIter != uses)
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

	std::string preprocessFile{ CreatePreprocess(fileName) };
	std::unordered_map<DataType, std::string> enumNames{};
	std::vector<std::string> enumTypesFiles{ CreateEnum(metaData,fileName,enumNames) };
	std::string structFile{ CreateStruct(metaData,fileName,enumNames) };

	std::filesystem::path fullPath(fileOutDir + fileName + GLOBAL::HEADER_POST_FIX);

	if (!SystemManager::GetInstance()->IsRebuild() && !(std::filesystem::exists(fullPath) && std::filesystem::is_regular_file(fullPath)))
		SystemManager::GetInstance()->SetRebuild(true);

	if (fullPath.has_parent_path())
		std::filesystem::create_directories(fullPath.parent_path());

	std::ofstream outFile{ fullPath };
	outFile << preprocessFile;
	for (const auto& enumType : enumTypesFiles)
	{
		outFile << enumType;
	}
	outFile << structFile;
	outFile.close();
}

HeaderGenerater::HeaderGenerater(EUSES _Uses) : uses(_Uses)
{
	if (_Uses == EUSES::SERVER)				fileOutDir = SystemManager::GetInstance()->GetDir(EDIR_FLAG::HEADER | EDIR_FLAG::OUTPUT | EDIR_FLAG::SERVER);
	else if (_Uses == EUSES::CLIENT)		fileOutDir = SystemManager::GetInstance()->GetDir(EDIR_FLAG::HEADER | EDIR_FLAG::OUTPUT | EDIR_FLAG::CLIENT);
	else									fileOutDir = SystemManager::GetInstance()->GetCurrentDir();
}

std::string HeaderGenerater::CreatePreprocess(const std::string& _FileName)
{
	std::string result{ SystemManager::GetInstance()->GetFormat(uses, EHEADER_FORMAT::PRE_PROCESS) };
	ExporterUtils::ReplaceString(result, MARK::NAME, _FileName);
	return result;
}

std::vector<std::string> HeaderGenerater::CreateEnum(const SheetMetaData& _MetaData, const std::string& _FileName, std::unordered_map<DataType, std::string>& _OutEnumNames)
{
	std::vector<std::string> result{};

	for (const auto& dataType : _MetaData.dataTypeList)
	{
		if (dataType.dataType != EDATA_TYPE::ENUM) continue;

		std::string enumName{ SystemManager::GetInstance()->GetDataTypeFormat(uses, EDATA_TYPE::ENUM) };
		ExporterUtils::ReplaceString(enumName, MARK::NAME, ExporterUtils::ToScreamingSnake(_FileName) + "_" + dataType.metaData);

		_OutEnumNames.emplace(std::pair<DataType, std::string>{ dataType, enumName });

		std::string enumFile{ SystemManager::GetInstance()->GetFormat(uses, EHEADER_FORMAT::ENUM) };
		std::string enumMembers{};

		for (const auto& em : dataType.enumSet)
		{
			std::string enumMember{ SystemManager::GetInstance()->GetDataTypeFormat(uses, EDATA_TYPE::ENUM_MEMBER) };
			ExporterUtils::ReplaceString(enumMember, MARK::NAME, em);
			enumMembers += enumMember + "\n\t";
		}

		enumMembers += GLOBAL::ENUM_END;
		ExporterUtils::ReplaceString(enumFile, { {MARK::NAME, enumName}, { MARK::DATA_TYPE,enumMembers } });
		result.emplace_back(enumFile);
	}

	return result;
}

std::string HeaderGenerater::CreateStruct(const SheetMetaData& _MetaData, const std::string& _FileName, const std::unordered_map<DataType, std::string>& _EnumNames)
{

	std::string result{ SystemManager::GetInstance()->GetFormat(uses, EHEADER_FORMAT::STRUCT) };
	ExporterUtils::ReplaceString(result, MARK::NAME, _FileName);

	std::string structFile{};
	for (size_t i = 1; i < _MetaData.dataTypeList.size(); ++i)
	{
		std::string structVar{ SystemManager::GetInstance()->GetDataTypeFormat(uses, EDATA_TYPE::STRUCT_VAR) };
		if (_MetaData.dataTypeList[i].bIsArray)
			ExporterUtils::ReplaceString(structVar, MARK::DATA_TYPE, SystemManager::GetInstance()->GetDataTypeFormat(uses, EDATA_TYPE::ARRAY));

		if (_MetaData.dataTypeList[i].dataType == EDATA_TYPE::ENUM)
		{
			ExporterUtils::ReplaceString(structVar, MARK::DATA_TYPE, _EnumNames.at(_MetaData.dataTypeList[i]));
		}
		else
		{
			ExporterUtils::ReplaceString(structVar, MARK::DATA_TYPE, SystemManager::GetInstance()->GetDataTypeFormat(uses, _MetaData.dataTypeList[i].dataType));
		}

		ExporterUtils::ReplaceString(structVar, MARK::NAME, _MetaData.dataTypeList[i].variableName);

		structFile += structVar + "\n\t";
	}


	ExporterUtils::ReplaceString(result, MARK::DATA_TYPE, structFile);
	return result;
}

