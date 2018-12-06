#include "manager.h"

//==============================================================================
int main(int, char*[])
{
    auto& manager = beewatch::Manager::get();
    manager.ctrlLoop();
    return 0;
}
