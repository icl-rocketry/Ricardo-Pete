#include<Arduino.h>
#include<SPI.h>


class MPRLS0001BA00001A {
    public: 
        MPRLS0001BA00001A:: MPRLS0001BA00001A();
        void MPRLS0001BA00001A::setup();
        void MPRLS0001BA00001A::update();
        void MPRLS0001BA00001A::readRegister();
        float MPRLS0001BA00001A::readPressure();
        float MPRLS0001BA00001A::readTemperature();
    private:
        float press_counts = 0; // digital pressure reading [counts]
        float temp_counts = 0; // digital temperature reading [counts]
        float pressure = 0; // pressure reading [bar, psi, kPa, etc.]
        float temperature = 0; // temperature reading in deg C
        float outputmax = 15099494; // output at maximum pressure [counts]
        float outputmin = 1677722; // output at minimum pressure [counts]
        float pmax = 1; // maximum value of pressure range [bar, psi, kPa, etc.]
        float pmin = 0; // minimum value of pressure range [bar, psi, kPa, etc.]
        float percentage = 0; // holds percentage of full scale data
}