#pragma once

#include <libriccore/riccoresystem.h>

#include <string_view>
#include <array>

//config includes
#include "Config/systemflags_config.h"
#include "Config/commands_config.h"
#include "Config/pinmap_config.h"
#include "Config/types.h"

#include <SPI.h>
#include <Wire.h>



#include "Commands/commands.h"

#include <libriccore/networkinterfaces/can/canbus.h>

#include "Sensors/dps310.h"
#include "Sensors/MPRLS0001BA00001A.h"


#include "Storage/sdfat_store.h"


class System : public RicCoreSystem<System,SYSTEM_FLAG,Commands::ID>
{
    public:

        System();
        
        void systemSetup();

        void systemUpdate();

        //board communication
        SPIClass vspi;
        SPIClass hspi;

        CanBus<SYSTEM_FLAG> canbus;

        DPS310 baro;

        SdFat_Store primarysd;

        MPRLS0001BA00001A pete1;
        MPRLS0001BA00001A pete2;
        MPRLS0001BA00001A pete3;
        MPRLS0001BA00001A pete4;
        MPRLS0001BA00001A pete5;
        

    private:

        void setupSPI();
        void setupPins();
        void loadConfig();
        void initializeLoggers();
        void configureNetwork();
        void logReadings();
        uint32_t telemetry_log_delta = 5000;
        uint32_t prev_telemetry_log_time;
        
        static constexpr std::string_view log_path = "/Logs";
        static constexpr std::string_view config_path = "/Config/rml.jsonc";
        

};