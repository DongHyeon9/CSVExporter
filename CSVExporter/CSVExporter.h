#pragma once
#include "SystemManager.h"

class CSVExporter
{
public:

protected:

private:
	std::vector<std::string> targetFiles{};

public:
	bool Init();
	bool Execute();

protected:

private:
	// CSV를 Write함
	// @param[in] _Path CSV로 쓸 xlsx파일 경로
	bool MakeCSV(std::string _Path);
	bool ReadXlsx();
	SheetInfo UnparseSheet(OpenXLSX::XLWorksheet& _Sheet);

	template<EUSES _USES>
	void WirteCSV(const SheetInfo& _SheetInfo, std::string _FileName)
	{
		_FileName += GLOBAL::CSV_POST_FIX;
		std::string finalOutPath{};
		if constexpr (_USES == EUSES::SERVER)			finalOutPath = SystemManager::GetInstance()->GetDir(EDIR_FLAG::CSV | EDIR_FLAG::OUTPUT | EDIR_FLAG::SERVER) + std::move(_FileName);
		else if constexpr (_USES == EUSES::CLIENT)		finalOutPath = SystemManager::GetInstance()->GetDir(EDIR_FLAG::CSV | EDIR_FLAG::OUTPUT | EDIR_FLAG::CLIENT) + std::move(_FileName);
		else											finalOutPath = SystemManager::GetInstance()->GetCurrentDir() + std::move(_FileName);

		std::filesystem::path path(finalOutPath);
		if (path.has_parent_path())
			std::filesystem::create_directories(path.parent_path());

		std::ofstream outFile{ finalOutPath.c_str() };
		for (size_t i = 0; i < _SheetInfo.csv.size(); ++i)
		{
			for (size_t j = 0; j < _SheetInfo.csv[i].size(); ++j)
			{
				if (_SheetInfo.metaData.usesList[j] != EUSES::NONE && _SheetInfo.metaData.usesList[j] != _USES) continue;

				std::string prefix{};
				std::string postfix{};

				if constexpr (_USES == EUSES::SERVER)
				{
					if (_SheetInfo.metaData.dataTypeList[j].bIsArray)
					{
						prefix = "(";
						postfix = ")";
					}
				}
				else if constexpr (_USES == EUSES::CLIENT)
				{
					prefix = _SheetInfo.metaData.dataTypeList[j].bIsArray ? "\"(" : "\"";
					postfix = _SheetInfo.metaData.dataTypeList[j].bIsArray ? ")\"" : "\"";
				}

				if (i != 0)
				{
					outFile << prefix << _SheetInfo.csv[i][j] << postfix;
				}
				else
				{
					outFile << _SheetInfo.csv[i][j];
				}
				if (j < _SheetInfo.csv[i].size() - 1)
					outFile << ',';
			}
			if (i < _SheetInfo.csv.size() - 1)
				outFile << "\n";
		}
		outFile.close();

		LOG("파일 쓰기 성공 : %s", finalOutPath.c_str());
	}
};

