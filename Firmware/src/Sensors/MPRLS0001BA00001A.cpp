#include<Arduino.h>
#include<SPI.h>
#include "MPRLS0001BA00001A.h"


MPRLS0001BA00001A:: MPRLS0001BA00001A(SPIClass& spi,Types::CoreTypes::SystemStatus_t& systemstatus,uint8_t cs):
_spi(spi),
_systemstatus(systemstatus),
_cs(cs), // update this with proper config value
_settings(200000, MSBFIRST, SPI_MODE0)
{}
void MPRLS0001BA00001A::setup(){
    //setup
    _spi.begin(); // begin SPI
    pinMode(_cs, OUTPUT); // pin 10 as SS
    digitalWrite(_cs, HIGH); // set SS High
}
void MPRLS0001BA00001A::update(){
    //update
    uint64_t data[7] = {0xFA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // holds output data
    uint64_t cmd[3] = {0xAA, 0x00, 0x00}; // command to be sent

    readRegister(cmd, 3);
    uint64_t prevtime = millis();
    if (millis() - prevtime > 10){
        readRegister(cmd, 3);
        prevtime = millis();
    }
    press_counts = data[3] + data[2] * 256 + data[1] * 65536; // calculate digital pressure counts
    temp_counts = data[6] + data[5] * 256 + data[4] * 65536; // calculate digital temperature counts
    temperature = (temp_counts * 200 / 16777215) - 50; // calculate temperature in deg c
    percentage = (press_counts / 16777215) * 100; // calculate pressure as percentage of full scale
    //calculation of pressure value according to equation 2 of datasheet
    pressure = ((press_counts - outputmin) * (pmax - pmin)) / (outputmax - outputmin) + pmin;
}

void MPRLS0001BA00001A::readRegister(uint8_t arr, int n)
{
    _spi.beginTransaction(_settings);
    digitalWrite(_cs, LOW);
    _spi.transfer(arr, n); 
    delay(1);
    digitalWrite(_cs, HIGH);
    _spi.endTransaction();
}

float MPRLS0001BA00001A::readPressure(){
    return pressure;
}
float MPRLS0001BA00001A::readTemperature(){
    return temperature;
}