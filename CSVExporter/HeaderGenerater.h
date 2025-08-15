#pragma once
#include "SystemManager.h"

class HeaderGenerater
{
public:

protected:

private:
	EUSES uses{};
	std::string fileOutDir{};

public:
	HeaderGenerater(EUSES _Uses);
	void Execute(SheetMetaData _MetaData, std::string _FileName);

protected:

private:
	std::string CreatePreprocess(const std::string& _FileName);
	std::vector<std::string> CreateEnum(const SheetMetaData& _MetaData, const std::string& _FileName, std::unordered_map<DataType, std::string>& _OutEnumNames);
	std::string CreateStruct(const SheetMetaData& _MetaData, const std::string& _FileName, const std::unordered_map<DataType, std::string>& _EnumNames);
};