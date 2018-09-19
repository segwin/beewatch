#include <health/healthlib.h>
#include <hw/hwlib.h>
#include <io/iolib.h>
#include <mass/masslib.h>
#include <reporting/reportinglib.h>
#include <util/utillib.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>
#include <thread>


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

            // Initialise DHT11
            dht11.reset( new hw::DHTxx(hw::DHTxx::Type::DHT22, std::move(gpioMap[36])) );

            // Initialise HX711
            //hx711.reset( new hw::HX711(std::move(gpioMap[23]),
            //                           std::move(gpioMap[24])) );
        }

        template <typename T>
        static std::string numToStr(T num, int decimalPlaces = 2)
        {
            int wholePart = static_cast<int>(num);

            double decimalMultiplier = std::pow(10, decimalPlaces);
            int decimalPart = static_cast<int>(decimalMultiplier * (num - wholePart));

            return std::to_string(wholePart) + "." + std::to_string(decimalPart);
        }

        void ctrlLoop()
        {
            while (1)
            {
                auto data = dht11->read();

                logger.print(Logger::Info, "Humidity: " + numToStr(data.humidity) + " %");
                logger.print(Logger::Info, "Temperature: " + numToStr(data.temperature) + " deg Celsius");

                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }

    private:
        std::map<int, io::GPIO::Ptr> gpioMap;

        hw::DHTxx::Ptr dht11;
        hw::HX711::Ptr hx711;
    };


} // namespace beewatch


//================================================================
int main(int argc, char* argv[])
{
    beewatch::Manager mgr;
    mgr.ctrlLoop();

    return 0;
}
