#pragma once
#include "SystemManager.h"

class MapperGenerator
{
private:
	std::string cppFormat{};
	std::string headerFormat{};

public:
	bool Init();
	bool Execute();

private:
	bool CreateMapper();
	bool ExcuteRebuild();
	bool RunFile(const std::string& _File);
};

