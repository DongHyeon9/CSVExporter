#include "MapperGenerator.h"

bool MapperGenerator::Execute()
{
	CHECK(CreateMapper(), false, "맵퍼 생성 실패");
	CHECK(ExcuteRebuild(), false, "리빌드 실패");
	return true;
}

bool MapperGenerator::CreateMapper()
{
	std::string dir{ SystemManager::GetInstance()->GetDir(EDIR_FLAG::MAPPER | EDIR_FLAG::OUTPUT | EDIR_FLAG::CLIENT) };
	ExporterUtils::NormalizeDir(dir);

	std::vector<std::string> files{};
	ExporterUtils::ListHeaderFiles(SystemManager::GetInstance()->GetDir(EDIR_FLAG::HEADER | EDIR_FLAG::OUTPUT | EDIR_FLAG::CLIENT), files);

	CHECK(CreateHeader(dir), false, "헤더 생성 실패");
	CHECK(CreateCpp(dir, files), false, "헤더 생성 실패");
	return true;
}

bool MapperGenerator::ExcuteRebuild()
{
	std::string rebuilderPath{ SystemManager::GetInstance()->GetDir(EDIR_FLAG::REBUILD) };
	rebuilderPath.pop_back();
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

	CHECK(result != NULL, false, "실행 실패");

	return true;
}

bool MapperGenerator::CreateHeader(const std::string& _Dir)
{
	std::string output{ _Dir + HEADER_GEN::CLIENT_MAPPER_NAME + GLOBAL::HEADER_POST_FIX };
	std::string fileWriter{ SystemManager::GetInstance()->GetFormat(EUSES::CLIENT,EHEADER_FORMAT::MAPPER_HEADER) };
	ExporterUtils::ReplaceString(fileWriter, MARK::NAME, HEADER_GEN::CLIENT_MAPPER_NAME);

	std::filesystem::path fullPath(output);
	if (fullPath.has_parent_path())
		std::filesystem::create_directories(fullPath.parent_path());

	std::ofstream outFile{ output };
	outFile << fileWriter;
	outFile.close();

	return true;
}

bool MapperGenerator::CreateCpp(const std::string& _Dir, const std::vector<std::string>& _Members)
{
	std::string output{ _Dir + HEADER_GEN::CLIENT_MAPPER_NAME + GLOBAL::CPP_POST_FIX };
	std::string fileWriter{ SystemManager::GetInstance()->GetFormat(EUSES::CLIENT,EHEADER_FORMAT::MAPPER_CPP) };

	std::string members{};
	std::string headers{};

	for (const auto& member : _Members)
	{
		std::string memberFormat{ SystemManager::GetInstance()->GetDataTypeFormat(EUSES::CLIENT,EDATA_TYPE::MAPPER_MEMBER) };
		
		ExporterUtils::ReplaceString(memberFormat, MARK::NAME, member);

		members += memberFormat + "\n";
		headers += CreateMapperHeader(member) + "\n";
	}

	ExporterUtils::ReplaceString(fileWriter, { {MARK::NAME,HEADER_GEN::CLIENT_MAPPER_NAME} ,{ MARK::HEADERS,headers },{MARK::DATA_TYPE,members} });

	std::ofstream outFile{ output };
	outFile << fileWriter;
	outFile.close();

	return true;
}

std::string MapperGenerator::CreateMapperHeader(const std::string& _FileName)
{
	std::string result{ SystemManager::GetInstance()->GetDataTypeFormat(EUSES::CLIENT,EDATA_TYPE::MAPPER_HEADER) };
	std::string dir{ SystemManager::GetInstance()->GetDir(EDIR_FLAG::HEADER | EDIR_FLAG::OUTPUT | EDIR_FLAG::CLIENT) };

	const std::string key = "\\Source";
	size_t pos = dir.find(key);
	if (pos != std::string::npos)
	{
		dir = ".." + dir.substr(pos);
	}
	ExporterUtils::NormalizeDir(dir);

	ExporterUtils::ReplaceString(result, MARK::NAME, dir + _FileName);

	return result;
}
