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
	LOG("���� : �ѱ�");

    dirMap[EDIR_FLAG::CURRENT] = std::filesystem::current_path().string();
    LOG("���� ��� : %s", dirMap[EDIR_FLAG::CURRENT].c_str());

	std::ifstream initFile{ INIT::EXPORTER_INI_FILE_NAME.c_str() };
	if (!initFile.is_open())
	{
		LOG("�ý����� ���� �ϴ� %s������ ����", INIT::EXPORTER_INI_FILE_NAME.c_str());
		return false;
	}
    
    std::string fileString{ (std::istreambuf_iterator<char>(initFile)), (std::istreambuf_iterator<char>()) };
    std::unordered_map<std::string, std::string> systemInit{};

    UnparseInitFile(fileString, systemInit);

    dirMap.reserve(INIT::OUTDIR_FLAG_MAP.bucket_count() + dirMap.bucket_count());
    for (const auto& flagPair : INIT::OUTDIR_FLAG_MAP)
    {
        dirMap.emplace(flagPair.first, CombineDir(dirMap[EDIR_FLAG::CURRENT], systemInit[flagPair.second]));
    }

    initFile.close();

	LOG("�ý��� �ʱ�ȭ ����!");
    return true;
}

int32 SystemManager::Run()
{
    CSVExporter exporter{};

    if (!exporter.Init())
    {
        LOG("CSVExporter �ʱ�ȭ ����");
        END_OF_PROGRAM(-1);
    }

    if (!exporter.Execute())
    {
        LOG("CSV Export ����");
        END_OF_PROGRAM(-1);
    }

    LOG("CSV Export ����!");
    END_OF_PROGRAM(0);
}

void SystemManager::UnparseInitFile(const std::string& _File, std::unordered_map<std::string, std::string>& _SystemInit)
{
    std::string line{};
    std::istringstream issPrjName{ _File };
    while (std::getline(issPrjName, line))
    {
        if (ExporterUtils::CompareIgnoreCase(line, INIT::PROJECT_NAME_FLAG))
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
                ExporterUtils::ReplaceString(value, MARK::PROJECT_NAME, projectName);
                ExporterUtils::NormalizeDir(value);
                _SystemInit.emplace(std::move(key), std::move(value));
            }
        }
    }
}

std::string SystemManager::CombineDir(std::string _Base, std::string _Additional)
{
    std::filesystem::path basePath{ std::move(_Base) };
    std::filesystem::path addPath{ std::move(_Additional) };

    // Normalize �߰� ���: lexically_normal �� ���� ���ο� ������� ���ڿ� ���ؿ��� �����մϴ�
    addPath = addPath.lexically_normal();
    // �Ǵ� ������� ��� ������ �ʿ��ϸ� weakly_canonical ��� ����
    // addPath = fs::weakly_canonical(addPath);

    std::filesystem::path result{ basePath };

    // "../" ������ŭ ���� ���丮�� �̵�
    for (const auto& part : addPath) {
        if (part == "..") {
            result = result.parent_path();
        }
        else if (part == ".") {
            continue;
        }
        else {
            result /= part;
        }
    }

    return result.string();
}
