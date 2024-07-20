#include<Arduino.h>
#include<SPI.h>
#include "MPRLS0001BA00001A.h"
#include "Config/pinmap_config.h"

MPRLS0001BA00001A::MPRLS0001BA00001A(SPIClass& spi,Types::CoreTypes::SystemStatus_t& systemstatus,uint8_t cs):
spi(spi),
systemstatus(systemstatus),
cs(cs) // update this with proper config value
{}
void MPRLS0001BA00001A::setup(){

    digitalWrite(cs, HIGH); // sensor is idle

    delay(10); // wait for sensor to power up

    clearRegister();

    delay(10); // wait for sensor to power up

    resetSensor();

    delay(10); // wait for sensor to power up

    clearRegister();

    delay(10); // wait for conversion

    readPressure();

    delay(10); // wait for conversion

    clearRegister();

    delay(10); // wait for conversion

    readPressure();

}

void MPRLS0001BA00001A::update(){

    delay(10);

    clearRegister();

    delay(10); 

    readPressure();

    delay(10);

    resetSensor();


}

void MPRLS0001BA00001A::clearRegister()
{
    digitalWrite(cs, LOW); // sensor is active
    spi.transfer(0xAA);
    spi.transfer(0x00);
    spi.transfer(0x00);
    digitalWrite(cs, HIGH); // sensor is idle

}

uint32_t MPRLS0001BA00001A::readRegister()
{
    spi.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
    digitalWrite(cs, LOW); // sensor is active
    uint8_t status2 = spi.transfer(0xF0);
    uint8_t data3 = spi.transfer(0x00);
    uint8_t data4 = spi.transfer(0x00);
    uint8_t data5 = spi.transfer(0x00);
    digitalWrite(cs, HIGH); // sensor is idle
    spi.endTransaction();

    // Serial.println(" Status2: " + String(status2) + " Data3: " + String(data3) + " Data4: " + String(data4) + " Data5: " + String(data5));
    return ((uint32_t)data3 << 16) | ((uint32_t)data4 << 8) | data5;
}

void MPRLS0001BA00001A::resetSensor() 
{
    digitalWrite(PinMap::SNSR_Reset, HIGH); 
    digitalWrite(PinMap::SNSR_Reset, LOW); 
    digitalWrite(PinMap::SNSR_Reset, HIGH); 
}

void MPRLS0001BA00001A::readPressure()
{
    uint32_t raw_data = readRegister();
    // Serial.println("Raw Data: " + String(raw_data));
    _Pressure = ((((float)raw_data - _min_cnt) * (_max_bar - _min_bar)) / (_max_cnt - _min_cnt)) + _min_bar; 
    // Serial.println("Pressure: " + String(_Pressure) + " bar " + String(cs));
}

