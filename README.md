# A winterised, monitored IoT enclosure for bees in cold climates

## Context

### Why?

Honey bees can be hard to manage in the winter in colder climates. This project is the
software component of an insulated enclosure that maintains an internal environment
favourable to the bees' well-being during the colder months.

The goal is to maintain an internal climate with temperatures between 0-5°C and relative
humidity between 50-75%. These conditions seem to be around the general consensus for
the ideal winter environment to keep the bees from waking up and finishing their food
reserves without dropping so low as to harm them.

### What is it?

The enclosure as designed consists of the following physical components:

  - An insulated box big enough to house 2 hives with 3 stacked supers (boxes)
  - A fan that vents the air outside the enclosure when it gets too warm or too humid
    (bees are really good at doing both of those!)
  - A thermostat for automatic fan control (currently an analog thermostat like you'd
    find in your home)
  - A Raspberry Pi Zero W board on which the software component runs

The software currently manages the following aspects of the system:

  - Temperature and humidity measurements via a DHT11 or DHT22 sensor
  - Data reporting via web page served on the device

I'd also like to implement a few major features down the road, including hive mass
monitoring and integrating the fan control into the software component via a PWM
interface. To track all the items on the project's wishlist, please see the issue board.

### Who is it for?

My dad, who lives in the countryside in eastern Ontario, Canada and is relatively new
to beekeeping. This is an open source project though, so anyone else with similar needs
should be able to build a similar system for themselves using the same parts and the software
provided here.


## Getting started

### Physical components

The following parts were used for this project:

- 1x Raspberry Pi Zero W
  - NB: The RPi Zero comes without GPIO pins, so you'll need to do some soldering to
        integrate everything
- 1x DHT22 (or DHT11) temperature & humidity sensor
  - GND and VCC pins connected to ground and +5V, respectively
  - DAT pin connected to GPIO16 (RPi pin 36)

The physical enclosure I'm using is currently a bit of a hack job using a house fan and
thermostat inside a shed built without any particular dimensions in mind. That said, the
design I'm targeting for development is the following:

- Plywood structure with a slanted roof for snow:
  - Base: 5ft/1.5m (length) by 3.6ft/1.1m (depth)
  - Tall wall: 4.6ft/1.4m (height)
  - Short wall: 3.6ft/1.1m (height)
  - Floor elevation: 1ft/0.3m (height)
- Polystyrene foam board insulation along the inner walls & ceiling and beneath the floor
- Cutouts on tall wall to accomodate 2-4 120mm fans
- Holes drilled in short wall to ease airflow

This should be large enough to accommodate two hives with 2 or 3 stacked supers. Of course,
the dimensions are a bit arbitrary and could be adapted so long as it fits the hives and
can house enough fans to ensure enough airflow to manage the internal environment.

### Software

The project is designed to run on any Raspberry Pi board since the model A+. Raspbian and
Ubuntu are both supported.

***Ubuntu Core:***

```
# Install cpprestsdk library
apt install libcpprest-dev

# Get repo
git clone git@gitlab.com:segwin/beewatch.git

cd beewatch/
git submodule update --init

# Build wiringpi library
cd vendor/wiringpi/
./build

# Build & install application
```

***Raspbian:***
```bash
# Install wiringpi library
apt install wiringpi

# Get repo
git clone git@gitlab.com:segwin/beewatch.git

cd beewatch/
git submodule update --init

# Build cpprestsdk library
sudo apt install g++ libboost-atomic-dev libboost-thread-dev libboost-system-dev \
                 libboost-date-time-dev libboost-regex-dev libboost-filesystem-dev \
                 libboost-random-dev libboost-chrono-dev libboost-serialization-dev \
                 libwebsocketpp-dev openssl libssl-dev make

cd vendor/cpprestsdk
mkdir build/
cd build/
cmake .. -DCMAKE_BUILD_TYPE=Release
make
sudo make install

# Build & install application
cd ../../
mkdir build
cmake ..

make
sudo make install
```
