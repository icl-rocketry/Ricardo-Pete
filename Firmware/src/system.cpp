#include "system.h"

#include <memory>

#include <libriccore/riccoresystem.h>

#include <HardwareSerial.h>

#include "Config/systemflags_config.h"
#include "Config/commands_config.h"
#include "Config/pinmap_config.h"
#include "Config/general_config.h"

#include "Commands/commands.h"

#include "States/idle.h"


System::System():
RicCoreSystem(Commands::command_map,Commands::defaultEnabledCommands,Serial),
SNSRSPI(0),
SDSPI(1),
canbus(systemstatus, PinMap::TxCan, PinMap::RxCan, 3),
P1(SNSRSPI, systemstatus, PinMap::P1_SS),
P2(SNSRSPI, systemstatus, PinMap::P2_SS),
P3(SNSRSPI, systemstatus, PinMap::P3_SS),
P4(SNSRSPI, systemstatus, PinMap::P4_SS),
P5(SNSRSPI, systemstatus, PinMap::P5_SS),
BAR(SNSRSPI, systemstatus, PinMap::BARO_SS),
SD(SDSPI, PinMap::SD_SS,SD_SCK_MHZ(20),false,&systemstatus){}
{};


void System::systemSetup(){
    
    Serial.setRxBufferSize(GeneralConfig::SerialRxSize);
    Serial.begin(GeneralConfig::SerialBaud);
   
    //intialize rnp message logger
    loggerhandler.retrieve_logger<RicCoreLoggingConfig::LOGGERS::SYS>().initialize(networkmanager);

    //initialize statemachine with idle state
    statemachine.initalize(std::make_unique<Idle>(systemstatus,commandhandler));
    //any other setup goes here

    setupSPI();
    
    canbus.setup();
    networkmanager.setNodeType(NODETYPE::HUB)
    networkmanager.setNoRouteAction(NOROUTE_ACTION::BROADCAST, {1, 3});
    networkmanager.addInterface(&canbus);


    SD.setup();

    P1.setup();
    P2.setup();
    P3.setup();
    P4.setup();
    P4.setup();
    BAR.setup();
   

};


void System::initializeLoggers()
{
    // check if sd card is mounted
    if (primarysd.getState() != StoreBase::STATE::NOMINAL)
    {
        loggerhandler.retrieve_logger<RicCoreLoggingConfig::LOGGERS::SYS>().initialize(nullptr, networkmanager);

        return;
    }

    // open log files
    // get unique directory for logs
    std::string log_directory_path = primarysd.generateUniquePath(log_path, "");
    // make new directory
    primarysd.mkdir(log_directory_path);

    std::unique_ptr<WrappedFile> syslogfile = primarysd.open(log_directory_path + "/syslog.txt", static_cast<FILE_MODE>(O_WRITE | O_CREAT | O_AT_END));
    std::unique_ptr<WrappedFile> telemetrylogfile = primarysd.open(log_directory_path + "/telemetrylog.txt", static_cast<FILE_MODE>(O_WRITE | O_CREAT | O_AT_END),50);

    // intialize sys logger
    loggerhandler.retrieve_logger<RicCoreLoggingConfig::LOGGERS::SYS>().initialize(std::move(syslogfile), networkmanager);

    // initialize telemetry logger
    loggerhandler.retrieve_logger<RicCoreLoggingConfig::LOGGERS::TELEMETRY>().initialize(std::move(telemetrylogfile),[](std::string_view msg){RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>(msg);});
}

void System::logReadings()
{
    if (micros() - prev_telemetry_log_time > telemetry_log_delta)
    {
        TelemetryLogframe logframe;

        logframe.P1 = P1.readPressure();
        logframe.P2 = P2.readPressure();
        logframe.P3 = P3.readPressure();
        logframe.P4 = P4.readPressure();
        logframe.P5 = P5.readPressure();
        logframe.BAR = BAR.readPressure();

        logframe.timestamp = micros();

        RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::TELEMETRY>(logframe);

        prev_telemetry_log_time = micros();
    }
}


void System::systemUpdate()
{
    SD.update();
    P1.update();
    P2.update();
    P3.update();
    P4.update();
    P5.update();
    BAR.update();  

    logReadings();
};

void System::setupSPI()
{
    SNSRSPI.begin();
    SDSPI.begin();
};

