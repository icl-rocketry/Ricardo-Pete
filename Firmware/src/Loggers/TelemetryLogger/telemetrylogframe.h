#pragma once

#include <librnp/rnp_serializer.h>
#include <unistd.h>

class TelemetryLogframe{
private:  
    static constexpr auto getSerializer()
    {
        auto ret = RnpSerializer(
            &TelemetryLogframe::pete1,
            &TelemetryLogframe::pete2,
            &TelemetryLogframe::pete3,
            &TelemetryLogframe::pete4,
            &TelemetryLogframe::pete5,
            &TelemetryLogframe::baro_alt,
            &TelemetryLogframe::baro_temp,
            &TelemetryLogframe::baro_press,
            &TelemetryLogframe::timestamp
        );
        return ret;
    }

public:
    //baro
    float baro_alt,baro_temp,baro_press;
    //pete
    float pete1,pete2,pete3,pete4,pete5;
    uint64_t timestamp;

    std::string stringify()const{
        return getSerializer().stringify(*this) + "\n";
    };

};
