#include <climate/climatelib.h>
#include <health/healthlib.h>
#include <hw/hwlib.h>
#include <io/iolib.h>
#include <mass/masslib.h>
#include <reporting/reportinglib.h>

#include <iostream>
#include <vector>


//================================================================
namespace beewatch
{

    //================================================================
    class Manager
    {
    public:
        Manager()
        {
            // Populate GPIO list
            for (int i = 0; i < 40; ++i)
            {
                gpioMap[i] = io::GPIO::claim(i);
            }

            // Initialise HX711
            hx711.reset( new hw::HX711(std::move(gpioMap[23]),
                                       std::move(gpioMap[24])) );
        }

    private:
        std::map<int, io::GPIO::Ptr> gpioMap;

        hw::HX711::Ptr hx711;
    };


} // namespace beewatch


//================================================================
int main(int argc, char* argv[])
{
    beewatch::Manager mgr;

    while (true)
    {
        // TODO: run control loop
    }

    return 0;
}
