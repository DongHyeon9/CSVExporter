#include "CSVExporter.h"
#include "HeaderGenerater.h"

int main() {

    if (!InitSystem())
    {
        LOG("Initialize Fail");
        END_OF_PROGRAM(-1);
    }

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
