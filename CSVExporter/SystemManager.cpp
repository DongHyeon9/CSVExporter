#include "SystemManager.h"
#include "CSVExporter.h"
#include "HeaderGenerater.h"

SystemManager* SystemManager::GetInstance()
{
    static SystemManager instance{};
    return &instance;
}

bool SystemManager::Init()
{
	_wsetlocale(LC_ALL, TEXT("korean"));
	LOG("언어설정 : 한국");

    dirMap[EDIR_FLAG::CURRENT] = std::filesystem::current_path().string();
    LOG("현재 경로 : %s", dirMap[EDIR_FLAG::CURRENT].c_str());

	std::ifstream initFile{ INIT::EXPORTER_INI_FILE_NAME.c_str() };
	if (!initFile.is_open())
	{
		LOG("시스템을 설정 하는 %s파일이 없음", INIT::EXPORTER_INI_FILE_NAME.c_str());
		return false;
	}
    
    std::string fileString{ (std::istreambuf_iterator<char>(initFile)), (std::istreambuf_iterator<char>()) };
    std::unordered_map<std::string, std::string> systemInit{};

    UnparseInitFile(fileString, systemInit);

    dirMap.reserve(INIT::OUTDIR_FLAG_MAP.bucket_count() + dirMap.bucket_count());
    for (const auto& flagPair : INIT::OUTDIR_FLAG_MAP)
    {
        std::string baseDir{ dirMap[EDIR_FLAG::CURRENT] };
        if (flagPair.first & EDIR_FLAG::CLIENT &&
            !(flagPair.first & EDIR_FLAG::FORMAT))
        {
            baseDir.erase(baseDir.find_last_of("\\"));
        }
        NormalizeDir(baseDir);

        dirMap.emplace(flagPair.first, baseDir + systemInit[flagPair.second]);
    }

    initFile.close();

	LOG("시스템 초기화 성공!");
    return true;
}

int32 SystemManager::Run()
{
    CSVExporter exporter{};

    if (!exporter.Init())
    {
        LOG("CSVExporter 초기화 실패");
        END_OF_PROGRAM(-1);
    }

    if (!exporter.Execute())
    {
        LOG("CSV Export 실패");
        END_OF_PROGRAM(-1);
    }

    LOG("CSV Export 성공!");
    END_OF_PROGRAM(0);
}

void SystemManager::UnparseInitFile(const std::string& _File, std::unordered_map<std::string, std::string>& _SystemInit)
{
    std::string line{};
    std::istringstream issPrjName{ _File };
    while (std::getline(issPrjName, line))
    {
        if (CompareIgnoreCase(line, INIT::PROJECT_NAME_FLAG))
        {
            if (std::getline(issPrjName, line))
            {
                projectName = line;
                break;
            }
        }
    }

    std::istringstream iss{ _File };

    while (std::getline(iss, line)) 
    {
        if (line.size() >= 2 && line.front() == '[' && line.back() == ']') 
        {
            std::string key{ line };
            if (std::getline(iss, line))
            {
                std::string value{ line };
                ReplaceString(value, MARK::PROJECT_NAME, projectName);
                NormalizeDir(value);
                _SystemInit.emplace(std::move(key), std::move(value));
            }
        }
    }
}
