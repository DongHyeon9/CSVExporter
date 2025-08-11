#include "HeaderGenerater.h"

void HeaderGenerater::Execute()
{
	std::string preprocessFile{ CreatePreprocess() };
	std::vector<std::string> enumTypesFiles{ CreateEnum() };
	std::string structFile{ CreateStruct() };

	std::ofstream outFile{ fileOutPath.c_str() };
	for (const auto& file : formats)
	{
		outFile << file;
	}
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
	return std::string();
}

std::vector<std::string> HeaderGenerater::CreateEnum()
{
	return std::vector<std::string>();
}

std::string HeaderGenerater::CreateStruct()
{
	return std::string();
}

