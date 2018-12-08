#include "manager.h"

//==============================================================================
int main(int argc, char* argv[])
{
    try
    {
        // 
        auto& app = beewatch::Manager::get();
        app.start();
    }
    catch (const std::exception& e)
    {

    }

    return 0;
}
