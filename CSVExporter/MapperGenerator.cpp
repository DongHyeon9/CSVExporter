#include "MapperGenerator.h"

bool MapperGenerator::Init()
{
	int32 mapperIdx{ static_cast<int32>(EHEADER_FORMAT::MAPPER_HEADER) };
	std::string filePath{ SystemManager::GetInstance()->GetDir(EDIR_FLAG::CLIENT | EDIR_FLAG::FORMAT) + HEADER_GEN::FORMAT_FILE_NAMES.at(mapperIdx) };
	std::ifstream formatFile{ filePath.c_str() };
	if (!formatFile.is_open())
	{
		LOG("���� ���� %s�� ����", HEADER_GEN::FORMAT_FILE_NAMES.at(mapperIdx).c_str());
		return false;
	}
	headerFormat = { (std::istreambuf_iterator<char>(formatFile)), (std::istreambuf_iterator<char>()) };
	formatFile.close();

	mapperIdx = static_cast<int32>(EHEADER_FORMAT::MAPPER_CPP);
	filePath = SystemManager::GetInstance()->GetDir(EDIR_FLAG::CLIENT | EDIR_FLAG::FORMAT) + HEADER_GEN::FORMAT_FILE_NAMES.at(mapperIdx);
	formatFile.open(filePath);
	if (!formatFile.is_open())
	{
		LOG("���� ���� %s�� ����", HEADER_GEN::FORMAT_FILE_NAMES.at(mapperIdx).c_str());
		return false;
	}
	cppFormat = { (std::istreambuf_iterator<char>(formatFile)), (std::istreambuf_iterator<char>()) };
	formatFile.close();

	return true;
}

bool MapperGenerator::Execute()
{
	CHECK(CreateMapper(), false, "���� ���� ����");
	CHECK(ExcuteRebuild(), false, "������ ����");
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
		nullptr,														// �θ� ������ �ڵ�
		L"open",														// ����: open, print ��
		ExporterUtils::ConvertString<std::wstring>(_File).c_str(),		// ������ ���� ���
		nullptr,														// ���� (�ʿ��ϸ� ���⿡)
		nullptr,														// �۾� ���丮
		SW_SHOWNORMAL													// â ǥ�� ���
	);

	CHECK(result == NULL, false, "���� ����");

	return true;
}
