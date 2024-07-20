#include "system.h"

#include <memory>

#include <ArduinoJson.h>

#include <libriccore/riccoresystem.h>
#include <libriccore/storage/wrappedfile.h>


#include "Config/systemflags_config.h"
#include "Config/commands_config.h"
#include "Config/pinmap_config.h"
#include "Config/general_config.h"
#include "Config/services_config.h"

#include "Commands/commands.h"

#include <libriccore/networkinterfaces/can/canbus.h>

#include "Storage/sdfat_store.h"
#include "Storage/sdfat_file.h"
#include "Loggers/TelemetryLogger/telemetrylogframe.h"

#include "States/idle.h"

#include "hal/usb_serial_jtag_ll.h"

System::System() : RicCoreSystem(Commands::command_map, Commands::defaultEnabledCommands, Serial),
                   vspi(0),
                   hspi(1),
                   canbus(systemstatus, PinMap::TxCan, PinMap::RxCan, 3),
                   baro(hspi, systemstatus, PinMap::BaroCs),
                   primarysd(vspi,PinMap::SD_Cs,SD_SCK_MHZ(20),false,&systemstatus),
                   pete1(hspi,systemstatus,PinMap::P1_Cs),
                   pete2(hspi,systemstatus,PinMap::P2_Cs),
                   pete3(hspi,systemstatus,PinMap::P3_Cs),
                   pete4(hspi,systemstatus,PinMap::P4_Cs),
                   pete5(hspi,systemstatus,PinMap::P5_Cs)
                   {};

void System::systemSetup()
{

    Serial.setRxBufferSize(GeneralConfig::SerialRxSize);
    Serial.begin(GeneralConfig::SerialBaud);
  
    statemachine.initalize(std::make_unique<Idle>(systemstatus,commandhandler));
    setupPins();

    // initalize spi interface
    setupSPI();

    primarysd.setup();

    initializeLoggers();    

    baro.setup();


    // network interfaces
    canbus.setup();

    // add interfaces to netmanager
    configureNetwork();

    loadConfig();


    pete1.setup();
    pete2.setup();
    pete3.setup();
    pete4.setup();
    pete5.setup();
};

void System::systemUpdate()
{
    // // Serial.println(baro.getPressure());
    pete1.update();
    pete2.update();
    pete3.update();
    pete4.update();
    pete5.update();
    baro.update();
    logReadings();
};

void System::setupSPI()
{   
    // sd card spi
    vspi.begin(PinMap::V_SCLK,PinMap::V_MISO,PinMap::V_MOSI);
    vspi.setFrequency(1000000);
    vspi.setBitOrder(MSBFIRST);
    vspi.setDataMode(SPI_MODE0);

    // sensor spi
    hspi.begin(PinMap::H_SCLK,PinMap::H_MISO,PinMap::H_MOSI);
    hspi.setFrequency(500000);
    hspi.setBitOrder(MSBFIRST);
    hspi.setDataMode(SPI_MODE0);
}


void System::setupPins()
{
    pinMode(PinMap::SD_Cs, OUTPUT);
    pinMode(PinMap::BaroCs, OUTPUT);
    pinMode(PinMap::P1_Cs, OUTPUT);
    pinMode(PinMap::P2_Cs, OUTPUT);
    pinMode(PinMap::P3_Cs, OUTPUT);
    pinMode(PinMap::P4_Cs, OUTPUT);
    pinMode(PinMap::P5_Cs, OUTPUT);
    pinMode(PinMap::SNSR_Reset, OUTPUT);


    digitalWrite(PinMap::SD_Cs, HIGH);
    digitalWrite(PinMap::BaroCs, HIGH);
    digitalWrite(PinMap::P1_Cs, HIGH);
    digitalWrite(PinMap::P2_Cs, HIGH);
    digitalWrite(PinMap::P3_Cs, HIGH);
    digitalWrite(PinMap::P4_Cs, HIGH);
    digitalWrite(PinMap::P5_Cs, HIGH);
    digitalWrite(PinMap::SNSR_Reset, HIGH);
}

void System::loadConfig()
{
    DynamicJsonDocument configDoc(16384); //allocate 16kb for config doc MAXSIZE
    DeserializationError jsonError;
    // get wrapped file for config doc -> returns nullptr if cant open
    

    //only try load file if sd card is present
    if (primarysd.getState() == StoreBase::STATE::NOMINAL)
    {

        primarysd.mkdir("/Config"); // ensure config directory exists

        std::unique_ptr<WrappedFile> config_file_ptr = primarysd.open(config_path,FILE_MODE::READ);

        if (config_file_ptr != nullptr)
        {
            //cast non-owning wrapped file ptr to sdfat_wrappedfile ptr
            SdFat_WrappedFile* sdfat_wrapped_file_ptr = reinterpret_cast<SdFat_WrappedFile*>(config_file_ptr.get());
            //lock the file store device lock
            {
            RicCoreThread::ScopedLock sl(sdfat_wrapped_file_ptr->getDevLock());
            jsonError = deserializeJson(configDoc,sdfat_wrapped_file_ptr->IStream());

            }
        }
        else
        {
            RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Error opening config file!");
        }

        if (jsonError)
        {
            RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("Error deserializing JSON - " + std::string(jsonError.c_str()));
        }
    }
    
}

void System::initializeLoggers()
{   
    //check if sd card is mounted
    if (primarysd.getState() != StoreBase::STATE::NOMINAL)
    {
        
        loggerhandler.retrieve_logger<RicCoreLoggingConfig::LOGGERS::SYS>().initialize(nullptr,networkmanager);
        RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("SD Init Failed");
        return;
    }

    //open log files
    //get unique directory for logs
    std::string log_directory_path = primarysd.generateUniquePath(log_path,"");
    //make new directory
    primarysd.mkdir(log_directory_path);

    std::unique_ptr<WrappedFile> syslogfile = primarysd.open(log_directory_path + "/syslog.txt",static_cast<FILE_MODE>(O_WRITE | O_CREAT | O_AT_END));
    std::unique_ptr<WrappedFile> telemetrylogfile = primarysd.open(log_directory_path + "/telemetrylog.txt",static_cast<FILE_MODE>(O_WRITE | O_CREAT | O_AT_END),50); 
    
    // intialize sys logger
    loggerhandler.retrieve_logger<RicCoreLoggingConfig::LOGGERS::SYS>().initialize(std::move(syslogfile),networkmanager);
   
    //initialize telemetry logger
    loggerhandler.retrieve_logger<RicCoreLoggingConfig::LOGGERS::TELEMETRY>().initialize(std::move(telemetrylogfile));

    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("SD Init Complete");
}

void System::configureNetwork()
{   
    networkmanager.setNodeType(NODETYPE::HUB);
    networkmanager.addInterface(&canbus);
    networkmanager.enableAutoRouteGen(true);
    networkmanager.setNoRouteAction(NOROUTE_ACTION::BROADCAST, {1,3});    
    networkmanager.updateBaseTable(); // save the new base table

};
void System::logReadings()
{
    if (micros() - prev_telemetry_log_time > telemetry_log_delta)
    {
        // FS0.update();
        TelemetryLogframe logframe;

        logframe.pete1 = pete1.getPressure();
        logframe.pete2 = pete2.getPressure();
        logframe.pete3 = pete3.getPressure();
        logframe.pete4 = pete4.getPressure();
        logframe.pete5 = pete5.getPressure();
        logframe.baro_alt = baro.getAltitude();
        logframe.baro_temp = baro.getTemperature();
        logframe.baro_press = baro.getPressure();
        logframe.timestamp = esp_timer_get_time();
        prev_telemetry_log_time = micros();

        RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::TELEMETRY>(logframe);

        
    }
}