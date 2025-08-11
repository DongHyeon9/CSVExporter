#include "CSVExporter.h"
#include "HeaderGenerater.h"

bool CSVExporter::Init()
{
	CHECK(ReadXlsx(), false, "엑셀 읽기 실패");
	LOG("엑셀 읽기 성공!");

	return true;
}

bool CSVExporter::Execute()
{
	for (const auto& targetFile : targetFiles)
	{
		CHECK(MakeCSV(targetFile), false, "CSV 생성 실패");
	}

	return true;
}

bool CSVExporter::MakeCSV(std::string _Path)
{
	std::string targetPath{ std::move(_Path) };

	const size_t fileNameStart{ targetPath.find_last_of('\\') + 1 };
	const size_t fileNameEnd{ targetPath.find_last_of('.') };
	std::string baseFilename{ targetPath.begin() + fileNameStart ,targetPath.begin() + fileNameEnd };
	LOG("========== Parse : %s ==========", baseFilename.c_str());

	OpenXLSX::XLDocument document{};
	HeaderGenerater clientGenerator{};
	HeaderGenerater serverGenerator{};

	document.open(targetPath);

	auto workBook{ document.workbook() };
	const std::vector<std::string>& sheetNames{ workBook.sheetNames() };
	for (const auto& sheetName : sheetNames)
	{
		std::string outputFileName{ baseFilename + '_' + sheetName };
		std::string outputFileName_Server{ outputFileName + GLOBAL::SERVER_POST_FIX };
		std::string outputFileName_Client{ outputFileName + GLOBAL::CLIENT_POST_FIX };

		auto workSheet{ workBook.worksheet(sheetName) };
		auto sheetInfo{ UnparseSheet(workSheet) };

		CHECK(clientGenerator.Init<EUSES::CLIENT>(sheetInfo.metaData, outputFileName), false, "클라이언트 헤더 제너레이터 초기화 실패");
		CHECK(serverGenerator.Init<EUSES::SERVER>(sheetInfo.metaData, outputFileName), false, "서버 헤더 제너레이터 초기화 실패");

		clientGenerator.Execute();
		serverGenerator.Execute();

		WirteCSV<EUSES::CLIENT>(sheetInfo, outputFileName_Client);
		WirteCSV<EUSES::SERVER>(sheetInfo, outputFileName_Server);
	}

	document.close();

	return true;
}

bool CSVExporter::ReadXlsx()
{
	ComInit com;
	CComPtr<IFileOpenDialog> dlg;

	CHECK(SUCCEEDED(dlg.CoCreateInstance(CLSID_FileOpenDialog)), false, "폴더 피커 인스터스 생성 실패");

	// 파일 선택, 다중선택 허용
	DWORD opts = 0;
	dlg->GetOptions(&opts);
	dlg->SetOptions(opts | FOS_ALLOWMULTISELECT | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST | FOS_FORCEFILESYSTEM);

	dlg->SetTitle(_T("출력할 xlsx파일을 선택해 주세요"));

	// 파일 타입 필터: .xlsx 전용
	const COMDLG_FILTERSPEC filter{ L"Excel Files (*.xlsx)", L"*.xlsx" };
	dlg->SetFileTypes(1, &filter);
	dlg->SetFileTypeIndex(1);

	CHECK(SUCCEEDED(dlg->Show(nullptr)), false, "선택한 파일이 없습니다");

	CComPtr<IShellItemArray> itemArray{};
	dlg->GetResults(&itemArray);

	DWORD count{};
	itemArray->GetCount(&count);

	LOG("선택한 파일 수 : %d", count);

	for (DWORD i = 0; i < count; ++i)
	{
		CComPtr<IShellItem> item{};
		itemArray->GetItemAt(i, &item);
		if (!item) continue;

		LPWSTR path{};
		if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)))
		{
			targetFiles.emplace_back(ExporterUtils::ConvertString<std::string, std::wstring>(path));
		}
		CoTaskMemFree(path);
	}

	LOG("=====선택된 파일 목록=====");
	for (const auto& targetFile : targetFiles)
	{
		LOG("%s", targetFile.c_str());
	}

	return true;
}


SheetInfo CSVExporter::UnparseSheet(OpenXLSX::XLWorksheet& _Sheet)
{
	SheetInfo result{};

	const uint32 rowCnt{ _Sheet.rowCount() };
	const uint32 colCnt{ _Sheet.columnCount() };

	// 1차 스캔
	// rightBottom, idRow, dataTypeRow, usesRow
	bool bIsFindID{};
	for (uint32 i = 1; i <= colCnt; ++i)
	{
		std::vector<std::string> row{};
		for (uint32 j = 1; j <= rowCnt; ++j)
		{
			std::string current{ _Sheet.cell(j, i).getString() };
			if (current.empty()) continue;

			result.rightBottom.y = max(result.rightBottom.y, j);
			row.emplace_back(current);
			if (bIsFindID || !ExporterUtils::CompareIgnoreCase(current, "ID")) continue;

			bIsFindID = true;
			result.usesRow.type = ELINE_TYPE::ROW;
			result.dataTypeRow.type = ELINE_TYPE::ROW;
			result.idRow.type = ELINE_TYPE::ROW;

			result.usesRow.lineIdx = j - 2;
			result.dataTypeRow.lineIdx = j - 1;
			result.idRow.lineIdx = j;

			result.usesRow.end = i;
			result.dataTypeRow.end = i;
			result.idRow.end = i;

			result.usesRow.start = i;
			result.dataTypeRow.start = i;
			result.idRow.start = i;

			uint32 idx{ i };

			while (!current.empty())
			{
				current = _Sheet.cell(j, ++idx).getString();
				result.idRow.end = max(result.idRow.end, idx);
				result.dataTypeRow.end = max(result.dataTypeRow.end, idx);
				result.usesRow.end = max(result.usesRow.end, idx);
			}
		}
		if (row.empty()) continue;

		result.rightBottom.x = max(result.rightBottom.x, i);
	}

	std::vector<std::string> idData{};
	idData.reserve(result.idRow.GetCount());
	result.metaData.dataTypeList.reserve(result.dataTypeRow.GetCount());
	result.metaData.usesList.reserve(result.usesRow.GetCount());
	result.csv.reserve(result.rightBottom.y - result.idRow.lineIdx + 1);
	std::set<uint32> ignoreColumn{};

	// 2차 데이터 파싱
	// metaData, ID 채우기
	for (uint32 i = result.idRow.start; i < result.idRow.end; ++i)
	{
		std::string currentType{ _Sheet.cell(result.dataTypeRow.lineIdx, i).getString() };
		std::string currentUses{ _Sheet.cell(result.usesRow.lineIdx, i).getString() };
		std::string currentID{ _Sheet.cell(result.idRow.lineIdx, i).getString() };

		if (currentType == GLOBAL::COMMENT || currentUses == GLOBAL::COMMENT || currentID == GLOBAL::COMMENT)
		{
			ignoreColumn.emplace(i);
			continue;
		}

		ExporterUtils::ToUpper(currentUses);

		DataType type{ ExporterUtils::StringToDataType(currentType) };
		type.variableName = currentID;
		EUSES uses{ ExporterUtils::StringToUses(currentUses) };

		result.metaData.dataTypeList.emplace_back(type);
		result.metaData.usesList.emplace_back(uses);
		idData.emplace_back(currentID);
	}
	result.csv.emplace_back(idData);

	// 3차 데이터 파싱
	// csv 채우기
	for (uint32 i = result.idRow.lineIdx + 1; i <= result.rightBottom.y; ++i)
	{
		std::vector<std::string> rowData{};
		rowData.reserve(result.rightBottom.x - result.idRow.start + 1);
		uint32 ignoreCnt{};
		for (uint32 j = result.idRow.start; j <= result.rightBottom.x; ++j)
		{
			if (ignoreColumn.find(j) != ignoreColumn.end())
			{
				++ignoreCnt;
				continue;
			}

			const uint32 colIdx{ j - result.idRow.start - ignoreCnt };
			std::string current{ _Sheet.cell(i, j).getString() };
			if (current == GLOBAL::COMMENT) break;

			if (result.metaData.dataTypeList[colIdx].dataType == EDATA_TYPE::INT)
			{
				const size_t decimalPointIdx{ current.find('.') };
				if (decimalPointIdx != std::string::npos)
					current.erase(decimalPointIdx);
			}
			else if (result.metaData.dataTypeList[colIdx].dataType == EDATA_TYPE::ENUM)
			{
				ExporterUtils::UnparseEnumData(current, result.metaData.dataTypeList[colIdx].enumSet);
			}
			rowData.emplace_back(current);
		}
		if (!rowData.empty())
			result.csv.emplace_back(rowData);
	}
	result.csv[0][0] = "---";

#pragma region LOG
#ifdef _DEBUG
	for (const auto& uses : result.metaData.usesList)
	{
		std::cout << "\t\t" << ExporterUtils::UsesToString(uses);
	}
	std::cout << std::endl;

	for (const auto& dataType : result.metaData.dataTypeList)
	{
		std::cout << "\t\t" << ExporterUtils::DataTypeToString(dataType);
	}
	std::cout << std::endl;

	for (const auto& row : result.csv)
	{
		for (const auto& col : row)
		{
			std::cout << col;
		}
		std::cout << std::endl;
	}

	for (const auto& dataType : result.metaData.dataTypeList)
	{
		LOG("==========Enum : %s==========", ExporterUtils::DataTypeToString(dataType).c_str());
		for (const auto& enumString : dataType.enumSet)
		{
			LOG("%s", enumString.c_str());
		}
	}
#endif
#pragma endregion LOG

	return result;
}

