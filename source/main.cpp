#include "hw/dhtxx.h"
#include "hw/hx711.h"
#include "io/gpio.h"

#include "logging.h"
#include "patterns.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <vector>
#include <thread>

namespace beewatch
{

    //==============================================================================
    class Manager : public singleton_t<Manager>
    {
    public:
        //==============================================================================
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

                g_logger.print(Logger::Info, "Humidity: " + numToStr(data.humidity) + " %");
                g_logger.print(Logger::Info, "Temperature: " + numToStr(data.temperature) + " deg Celsius");

                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }


    protected:
        //==============================================================================
        Manager()
        {
            // Populate GPIO list
            for (int i = 0; i < io::GPIO::NUM_GPIO; ++i)
            {
                gpioMap[i] = io::GPIO::claim(i);
            }

            // Initialise DHT11
            dht11 = std::make_unique<hw::DHTxx>(hw::DHTxx::Type::DHT22, std::move(gpioMap[16]));

            // Initialise HX711
            //hx711 = std::make_unique<hw::HX711>(std::move(gpioMap[11]), std::move(gpioMap[8]));
        }


    private:
        //==============================================================================
        std::map<int, io::GPIO::Ptr> gpioMap;

        hw::DHTxx::Ptr dht11;
        hw::HX711::Ptr hx711;
    };

    //==============================================================================
    Manager& g_manager = Manager::get();

} // namespace beewatch


//==============================================================================
int main(int, char*[])
{
    beewatch::g_manager.ctrlLoop();
    return 0;
}
