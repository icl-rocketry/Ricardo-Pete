#pragma once

#include <librnp/rnp_packet.h>
#include <librnp/rnp_serializer.h>

#include <vector>

class ProcessedSensorPacket : public RnpPacket{
    private:
    //serializer framework
        static constexpr auto getSerializer()
        {
            auto ret = RnpSerializer(
                &ProcessedSensorPacket::pete1,
                &ProcessedSensorPacket::pete2,
                &ProcessedSensorPacket::pete3,
                &ProcessedSensorPacket::pete4,
                &ProcessedSensorPacket::pete5,
                &ProcessedSensorPacket::baro_alt,
                &ProcessedSensorPacket::baro_temp,
                &ProcessedSensorPacket::baro_press,
                &ProcessedSensorPacket::system_status,
                &ProcessedSensorPacket::system_time

            );
            return ret;
        }
        
    public:
        ~ProcessedSensorPacket();

        ProcessedSensorPacket();
        /**
         * @brief Deserialize Telemetry Packet
         * 
         * @param data 
         */
        ProcessedSensorPacket(const RnpPacketSerialized& packet);

        /**
         * @brief Serialize Telemetry Packet
         * 
         * @param buf 
         */
        void serialize(std::vector<uint8_t>& buf) override;

        float pete1;
        float pete2;
        float pete3;
        float pete4;
        float pete5;
        float baro_alt;
        float baro_temp;
        float baro_press;


        //system details
        uint32_t system_status;
        uint64_t system_time;


        static constexpr size_t size(){
            return getSerializer().member_size();
        }

};


