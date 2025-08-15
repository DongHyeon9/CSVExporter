#pragma once
#include "SystemManager.h"

class MapperGenerator
{
public:
	bool Execute();

private:
	bool CreateMapper();
	bool ExcuteRebuild();

	bool RunFile(const std::string& _File);
	bool CreateHeader(const std::string& _Dir);
	bool CreateCpp(const std::string& _Dir, const std::vector<std::string>& _Members);
	std::string CreateMapperHeader(const std::string& _FileName);
};

