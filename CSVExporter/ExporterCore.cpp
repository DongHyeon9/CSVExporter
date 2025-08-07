#include "ExporterCore.h"

std::string GLOBAL::CURRENT_DIR{};
const std::string GLOBAL::SERVER_POST_FIX_NAME{ "_Server" };
const std::string GLOBAL::CLIENT_POST_FIX_NAME{ "_Client" };
const std::string GLOBAL::CSV_OUTPUT_DIR_FILE_NAME{ "CsvOutputDir.txt" };

bool InitSystem()
{
    int32 LogNumber{ _setmode(_fileno(stdout), _O_U16TEXT) };
    LOG("Log Mode : %d", LogNumber);
    _wsetlocale(LC_ALL, TEXT("korean"));
    GLOBAL::CURRENT_DIR = std::filesystem::current_path().string();
    NormalizeDir(GLOBAL::CURRENT_DIR);
    LOG("현재 경로 : %s", ConvertString<std::wstring>(GLOBAL::CURRENT_DIR).c_str());
    LOG("언어설정 : 한국");
    LOG("시스템 초기화 성공!");
    return true;
}

void NormalizeDir(std::string& _Path)
{
    if (_Path.back() != '\\')
        _Path.push_back('\\');
}

std::wstring UTF8ToWstring(const std::string& _UTF8)
{
    if (_UTF8.empty()) return {};
    int32 sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, _UTF8.data(), (int32)_UTF8.size(), nullptr, 0);
    if (sizeNeeded == 0)
        throw std::runtime_error("UTF‑8 to UTF‑16 conversion failed");

    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, _UTF8.data(), (int32)_UTF8.size(), &result[0], sizeNeeded);
    return result;
}

std::string WstringToUTF8(const std::wstring& _UTF16)
{
    if (_UTF16.empty()) return {};
    int32 requiredBytes = WideCharToMultiByte(
        CP_UTF8, 0,
        _UTF16.data(), (int32)_UTF16.size(),
        nullptr, 0,
        nullptr, nullptr
    );
    if (requiredBytes == 0) {
        throw std::system_error(GetLastError(), std::system_category(), "WideCharToMultiByte failed");
    }
    std::string utf8(requiredBytes, '\0');
    WideCharToMultiByte(
        CP_UTF8, 0,
        _UTF16.data(), (int32)_UTF16.size(),
        &utf8[0], requiredBytes,
        nullptr, nullptr
    );
    return utf8;
}
