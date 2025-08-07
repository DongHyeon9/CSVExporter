#pragma once
#include "ExporterCore.h"

class CSVExporter
{
public:

protected:

private:
	std::vector<std::wstring> targetFiles{};
	std::string outputPath{};
	OpenXLSX::XLDocument document;

public:
	bool Init();
	bool Execute();

protected:

private:
	bool WriteCSV(std::string _Path);
	bool OpenFolderPicker();
};

