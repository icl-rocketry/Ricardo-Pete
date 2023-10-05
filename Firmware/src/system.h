#pragma once
#include <SPI.h>
#include <libriccore/riccoresystem.h>

#include "Config/systemflags_config.h"
#include "Config/commands_config.h"
#include "Config/pinmap_config.h"

#include <libriccore/networkinterfaces/can/canbus.h>

#include "Commands/commands.h"

#include "Sensors/MPRLS0001BA00001A.h"
#include "Sensors/DPS310.h"
#include "Storage/sdfat_store.h"

class System : public RicCoreSystem<System,SYSTEM_FLAG,Commands::ID>
{
    public:

        System();
        
        void systemSetup();

        void systemUpdate();

        SPIClass SDSPI;
        SPIClass SNSRSPI;

        CanBus<SYSTEM_FLAG> canbus;

        MPRLS0001BA00001A P1;
        MPRLS0001BA00001A P2;
        MPRLS0001BA00001A P3;
        MPRLS0001BA00001A P4;
        MPRLS0001BA00001A P5;
        DPS310 BAR;

        SdFat_Store SD;

    // private:


};