#include<Arduino.h>
#include<SPI.h>
#include "Config/types.h"


class MPRLS0001BA00001A {
    public: 
        MPRLS0001BA00001A(SPIClass& _spi,Types::CoreTypes::SystemStatus_t& systemstatus,uint8_t cs);
        void setup();
        void update();
        u32_t readRegister();
        void clearRegister();
        void resetSensor();
        void readPressure();
        float getPressure(){return _Pressure;};
    private:

        SPIClass & spi;
        Types::CoreTypes::SystemStatus_t& systemstatus;
        const uint8_t cs;
        float _min_cnt =  1677722; // minimum pressure reading [counts]
        float _max_cnt = 15099494; // maximum pressure reading [counts]
        float _max_bar = 1;
        float _min_bar = 0;
        float _Pressure;

};