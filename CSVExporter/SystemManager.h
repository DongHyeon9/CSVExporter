#pragma once
#include "ExporterCore.h"

class SystemManager
{
private:
	std::string projectName{};
	std::unordered_map<int32, std::string> dirMap{};
	std::array<std::array<std::string, static_cast<int32>(EHEADER_FORMAT::END)>,static_cast<int32>(EUSES::NONE)> formats{};
	std::array<std::array<std::string, static_cast<int32>(EDATA_TYPE::END)>,static_cast<int32>(EUSES::NONE)> dataTypeFormats{};
	bool bIsRebuild{};

public:
	static SystemManager* GetInstance();
	bool Init();
	int32 Run();
	void SetRebuild(bool _bIsRebuild) { bIsRebuild = _bIsRebuild; }
	bool IsRebuild()const { return bIsRebuild; }
	const std::string& GetDir(int32 _Flag)const { return dirMap.at(_Flag); }
	const std::string& GetCurrentDir()const { return dirMap.at(EDIR_FLAG::CURRENT); }
	const std::string& GetFormat(EUSES _Uses, EHEADER_FORMAT _Format)const { return formats[static_cast<int32>(_Uses)][static_cast<int32>(_Format)]; }
	const std::string& GetDataTypeFormat(EUSES _Uses, EDATA_TYPE _Format)const { return dataTypeFormats[static_cast<int32>(_Uses)][static_cast<int32>(_Format)]; }

private:
	void UnparseInitFile(const std::string& _File, std::unordered_map<std::string, std::string>& _SystemInit);
	template<EUSES _USES>
	bool CreateFormat()
	{
		std::string dir{};
		if constexpr (_USES == EUSES::CLIENT)			dir = SystemManager::GetInstance()->GetDir(EDIR_FLAG::CLIENT | EDIR_FLAG::FORMAT);
		else if constexpr (_USES == EUSES::SERVER)		dir = SystemManager::GetInstance()->GetDir(EDIR_FLAG::SERVER | EDIR_FLAG::FORMAT);
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
			formats[static_cast<int32>(_USES)][i] = { (std::istreambuf_iterator<char>(formatFile)), (std::istreambuf_iterator<char>()) };
			formatFile.close();
		}

		std::istringstream iss{ formats[static_cast<int32>(_USES)][static_cast<int32>(EHEADER_FORMAT::DATA_TYPE)] };
		std::string line{};
		for (int32 i = 0; i < static_cast<int32>(EDATA_TYPE::END); ++i)
		{
			if (std::getline(iss, line))
			{
				size_t idx{ line.find(DATA_TYPE::META_DATA_FLAG) };
				dataTypeFormats[static_cast<int32>(_USES)][i] = line.substr(idx + 1);
			}
		}

		return true;
	}
	std::string CombineDir(std::string _Base, std::string _Additional);

private:
	SystemManager(){}
};

