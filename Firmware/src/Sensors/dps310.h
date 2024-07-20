#pragma once

#include <SPI.h>
#include <Dps3xx.h>

#include "Config/types.h"


class DPS310 : private Dps3xx{

    public:
        DPS310(SPIClass& spi, Types::CoreTypes::SystemStatus_t& systemstatus,uint8_t cs);

        void setup();

        void update();

        void calibrateBaro();

        float getPressure() {return baro_pressure;}
        float getTemperature() {return baro_temperature;}
        float getAltitude() {return baro_altitude;}


    private:
        SPIClass & _spi;


        Types::CoreTypes::SystemStatus_t& _systemstatus;
        const uint8_t _cs;
        bool _initialized;

        float baro_pressure;
        float baro_temperature;
        float baro_altitude;

        /**
         * @brief Reference pressure and temp for altitude calculation
         * 
         */
        float refTemp{273.15 + 15};
        float refPress{101325};

        /**
         * @brief This reads the most recent pressure and temperautre result in the result registers.
         * 
         * @param pressure 
         * @param temp 
         */
        void readDPS();

        
        /**
         * @brief  Temperature Measurement rate 
         * 
         */
        static constexpr int temp_mr = 3;
        /**
         * @brief Temperature Over sampling rate
         * 
         */
        static constexpr int temp_osr = 1;
        /**
         * @brief Pressure Measurement Rate
         * 
         */
        static constexpr int press_mr = 6;
        /**
         * @brief Pressure Over sampling rate
         * 
         */
        static constexpr int press_osr = 3;

        float toAltitude(const float& pressure);

        void loadDPSCalibrationValues();
        void writeDPSCalibrationValues();

};