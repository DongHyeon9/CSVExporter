#pragma once
#include "ExporterCore.h"

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
		std::string finalOutPath{ GLOBAL::OUT_PUT_DIR + std::move(_FileName) };

		std::filesystem::path path(finalOutPath);
		if (path.has_parent_path())
			std::filesystem::create_directories(path.parent_path());

		std::ofstream outFile{ finalOutPath.c_str() };
		for (size_t i = 0; i < _SheetInfo.csv.size(); ++i)
		{
			for (size_t j = 0; j < _SheetInfo.csv[i].size(); ++j)
			{
				if (_SheetInfo.metaData.usesList[j] != EUSES::ALL && _SheetInfo.metaData.usesList[j] != _USES) continue;
				std::string front{ _SheetInfo.metaData.dataTypeList[j].bIsArray ? "\"(" : "\"" };
				std::string back{ _SheetInfo.metaData.dataTypeList[j].bIsArray ? ")\"" : "\"" };
				if (i != 0)
				{
					outFile << front << _SheetInfo.csv[i][j] << back;
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

