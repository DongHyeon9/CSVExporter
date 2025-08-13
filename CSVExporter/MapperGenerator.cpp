#include "MapperGenerator.h"

bool MapperGenerator::Init()
{
	int32 mapperIdx{ static_cast<int32>(EHEADER_FORMAT::MAPPER_HEADER) };
	std::string filePath{ SystemManager::GetInstance()->GetDir(EDIR_FLAG::CLIENT | EDIR_FLAG::FORMAT) + HEADER_GEN::FORMAT_FILE_NAMES.at(mapperIdx) };
	std::ifstream formatFile{ filePath.c_str() };
	if (!formatFile.is_open())
	{
		LOG("포맷 파일 %s가 없음", HEADER_GEN::FORMAT_FILE_NAMES.at(mapperIdx).c_str());
		return false;
	}
	headerFormat = { (std::istreambuf_iterator<char>(formatFile)), (std::istreambuf_iterator<char>()) };
	formatFile.close();

	mapperIdx = static_cast<int32>(EHEADER_FORMAT::MAPPER_CPP);
	filePath = SystemManager::GetInstance()->GetDir(EDIR_FLAG::CLIENT | EDIR_FLAG::FORMAT) + HEADER_GEN::FORMAT_FILE_NAMES.at(mapperIdx);
	formatFile.open(filePath);
	if (!formatFile.is_open())
	{
		LOG("포맷 파일 %s가 없음", HEADER_GEN::FORMAT_FILE_NAMES.at(mapperIdx).c_str());
		return false;
	}
	cppFormat = { (std::istreambuf_iterator<char>(formatFile)), (std::istreambuf_iterator<char>()) };
	formatFile.close();

	return true;
}

bool MapperGenerator::Execute()
{
	CHECK(CreateMapper(), false, "맵퍼 생성 실패");
	CHECK(ExcuteRebuild(), false, "리빌드 실패");
	return true;
}

bool MapperGenerator::CreateMapper()
{
	
	return true;
}

bool MapperGenerator::ExcuteRebuild()
{
	std::string rebuilderPath{ SystemManager::GetInstance()->GetDir(EDIR_FLAG::REBUILD) };
	bool bIsRebuildSuccess
	{
		std::filesystem::exists(rebuilderPath) &&
		std::filesystem::is_regular_file(rebuilderPath) &&
		RunFile(rebuilderPath)
	};
	return bIsRebuildSuccess;
}

bool MapperGenerator::RunFile(const std::string& _File)
{
	HINSTANCE result = ShellExecute(
		nullptr,														// 부모 윈도우 핸들
		L"open",														// 동작: open, print 등
		ExporterUtils::ConvertString<std::wstring>(_File).c_str(),		// 실행할 파일 경로
		nullptr,														// 인자 (필요하면 여기에)
		nullptr,														// 작업 디렉토리
		SW_SHOWNORMAL													// 창 표시 방법
	);

	CHECK(result == NULL, false, "실행 실패");

	return true;
}
