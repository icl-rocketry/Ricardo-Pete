/**
 * @file commands.cpp
 * @author Kiran de Silva (kd619@ic.ac.uk)
 * @brief Implementation of commands for system
 * @version 0.1
 * @date 2023-06-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "commands.h"

#include <memory>

#include <librnp/rnp_packet.h>
#include <librnp/rnp_interface.h>
#include <librnp/rnp_networkmanager.h>
#include <libriccore/commands/commandhandler.h>

#include "Commands/processedsensorpacket.h"

#include "system.h"

#include "States/idle.h"

#include "Config/services_config.h"

void Commands::FreeRamCommand(System& system, const RnpPacketSerialized& packet)
{	
	/// ESP_LOGI("ch", "%s", "deserialize");

	SimpleCommandPacket commandpacket(packet);

	uint32_t freeram = esp_get_free_heap_size();
	//avliable in all states
	//returning as simple string packet for ease
	//currently only returning free ram
	if (commandpacket.arg == 0){
	MessagePacket_Base<0,static_cast<uint8_t>(decltype(System::commandhandler)::PACKET_TYPES::MESSAGE_RESPONSE)> message("FreeRam: " + std::to_string(esp_get_free_heap_size()));
	// this is not great as it assumes a single command handler with the same service ID
	// would be better if we could pass some context through the function paramters so it has an idea who has called it
	// or make it much clearer that only a single command handler should exist in the system
		message.header.source_service = system.commandhandler.getServiceID(); 
		message.header.destination_service = packet.header.source_service;
		message.header.source = packet.header.destination;
		message.header.destination = packet.header.source;
		message.header.uid = packet.header.uid;
		system.networkmanager.sendPacket(message);
	}
	else if (commandpacket.arg == 1)
	{
		BasicDataPacket<uint32_t,0,105> responsePacket(freeram);
		responsePacket.header.source_service = system.commandhandler.getServiceID(); 
		responsePacket.header.destination_service = packet.header.source_service;
		responsePacket.header.source = packet.header.destination;
		responsePacket.header.destination = packet.header.source;
		responsePacket.header.uid = packet.header.uid;
		system.networkmanager.sendPacket(responsePacket);	
	}
	
}

void Commands::TelemetryCommand(System& sm, const RnpPacketSerialized& packet)
{
	SimpleCommandPacket commandpacket(packet);

	ProcessedSensorPacket processedSensorPacket;

	processedSensorPacket.header.type = 103;
	processedSensorPacket.header.source = sm.networkmanager.getAddress();
	processedSensorPacket.header.source_service = sm.commandhandler.getServiceID();
	processedSensorPacket.header.destination = commandpacket.header.source;
	processedSensorPacket.header.destination_service = commandpacket.header.source_service;
	processedSensorPacket.header.uid = commandpacket.header.uid;
	processedSensorPacket.system_time = millis();

	processedSensorPacket.pete1 = sm.pete1.getPressure();
	processedSensorPacket.pete2 = sm.pete2.getPressure();
	processedSensorPacket.pete3 = sm.pete3.getPressure();
	processedSensorPacket.pete4 = sm.pete4.getPressure();
	processedSensorPacket.pete5 = sm.pete5.getPressure();
	processedSensorPacket.baro_alt = sm.baro.getAltitude();
	processedSensorPacket.baro_temp = sm.baro.getTemperature();
	processedSensorPacket.baro_press = sm.baro.getPressure();

	processedSensorPacket.system_status = sm.systemstatus.getStatus();

	sm.networkmanager.sendPacket(processedSensorPacket);
}