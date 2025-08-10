#include "SystemManager.h"

int main() 
{
    if (!SystemManager::GetInstance()->Init())
    {
        LOG("Initialize Fail");
        END_OF_PROGRAM(-1);
    }

    return SystemManager::GetInstance()->Run();
}
