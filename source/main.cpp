#include "hw/dhtxx.h"
#include "hw/hx711.h"
#include "io/gpio.h"

#include "logging.h"
#include "patterns.h"
#include "timing.h"

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

                // Measure mass
                //auto mass = hx711->read();
                //
                //g_logger.print(Logger::Info, "Mass: " + numToStr(mass) + " kg");

                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }


    protected:
        //==============================================================================
        Manager()
        {
            // Initialise DHT11
            dht11 = std::make_unique<hw::DHTxx>(hw::DHTxx::Type::DHT22, io::GPIO::claim(16));

            // Initialise HX711
            //hx711 = std::make_unique<hw::HX711>(io::GPIO::claim(5), io::GPIO::claim(6));
        }


    private:
        //==============================================================================
        hw::DHTxx::Ptr dht11;
        hw::HX711::Ptr hx711;
    };

} // namespace beewatch


//==============================================================================
int main(int, char*[])
{
    auto& manager = beewatch::Manager::get();
    manager.ctrlLoop();
    return 0;
}
