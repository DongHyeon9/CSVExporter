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
	std::array<std::string, static_cast<int32>(EHEADER_FORMAT::END)> formats{};
	//std::unordered_map<>

public:
	template<EUSES _USES>
	bool Init()
	{
		std::string dir{};
		if constexpr (_USES == EUSES::CLIENT)
		{
			dir = SystemManager::GetInstance()->GetDir(EDIR_FLAG::CLIENT | EDIR_FLAG::FORMAT);
		}
		else if constexpr (_USES == EUSES::SERVER)
		{
			dir = SystemManager::GetInstance()->GetDir(EDIR_FLAG::SERVER | EDIR_FLAG::FORMAT);
		}
		NormalizeDir(dir);

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
		return true;
	}
	void Execute(const SheetInfo& _SheetInfo);

protected:

private:

};
