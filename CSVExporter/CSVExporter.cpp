#include "CSVExporter.h"

bool CSVExporter::Init()
{
	std::ifstream csvOutputDirFile{ GLOBAL::CSV_OUTPUT_DIR_FILE_NAME.c_str() };
	if (!csvOutputDirFile.is_open())
	{
		LOG("CSV 출력 경로알려주는 %s파일이 없음", ConvertString<std::wstring>(GLOBAL::CSV_OUTPUT_DIR_FILE_NAME).c_str());
		return false;
	}

	outputPath = { (std::istreambuf_iterator<char>(csvOutputDirFile)), (std::istreambuf_iterator<char>()) };
	NormalizeDir(outputPath);
	
	LOG("CSV 출력 경로 : %s", ConvertString<std::wstring>(outputPath).c_str());

	csvOutputDirFile.close();
	CHECK(OpenFolderPicker(), false, "폴더 선택 실패");
	LOG("폴더 선택 성공!");

	return true;
}

bool CSVExporter::Execute()
{
	for (const auto& targetFile : targetFiles)
	{
		WriteCSV(ConvertString<std::string>(targetFile));

	}

	return true;
}

bool CSVExporter::WriteCSV(std::string _Path)
{
	std::string targetPath{ std::move(_Path) };
	document.open(targetPath);

	const size_t fileNameStart{ targetPath.find_last_of('\\') + 1 };
	const size_t fileNameEnd{ targetPath.find_last_of('.') };
	std::string outputFileName{ targetPath.begin() + fileNameStart ,targetPath.begin() + fileNameEnd };
	outputFileName += ".csv";
	std::string finalFilePath{ GLOBAL::CURRENT_DIR + outputPath + outputFileName };
	LOG("출력 파일 이름 : %s", ConvertString<std::wstring>(finalFilePath).c_str());

	std::filesystem::path path(finalFilePath);
	if (path.has_parent_path())
		std::filesystem::create_directories(path.parent_path());

	std::ofstream outFile{ finalFilePath.c_str() };
	outFile << "bbb";
	outFile.close();

	document.close();
	return true;
}

bool CSVExporter::OpenFolderPicker()
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
			targetFiles.emplace_back(path);
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
