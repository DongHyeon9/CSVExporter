#pragma once
#include "ExporterCore.h"

class SystemManager
{
private:
	std::string projectName{};
	std::unordered_map<int32, std::string> dirMap{};
	bool bIsRebuild{};

public:
	static SystemManager* GetInstance();
	bool Init();
	int32 Run();
	void SetRebuild(bool _bIsRebuild) { bIsRebuild = _bIsRebuild; }
	bool IsRebuild()const { return bIsRebuild; }
	const std::string& GetDir(int32 _Flag)const { return dirMap.at(_Flag); }
	const std::string& GetCurrentDir()const { return dirMap.at(EDIR_FLAG::CURRENT); }

private:
	void UnparseInitFile(const std::string& _File, std::unordered_map<std::string, std::string>& _SystemInit);
	std::string CombineDir(std::string _Base, std::string _Additional);

private:
	SystemManager(){}
};

