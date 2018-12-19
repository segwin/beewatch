#include "global/manager.h"

//==============================================================================
int main(int argc, char* argv[])
{
    try
    {
        // 
        auto& app = beewatch::Manager::get();

        app.init(argc, argv);
        app.start();
    }
    catch (const std::exception& e)
    {

    }

    return 0;
}
