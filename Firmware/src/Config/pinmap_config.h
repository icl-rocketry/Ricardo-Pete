/*
**********************
* PINS               *
**********************
 */
#pragma once

namespace PinMap{
    static constexpr int H_SCLK = 11;
    static constexpr int H_MOSI = 12;
    static constexpr int H_MISO = 13;

    static constexpr int SdDet = 34; //sd card detected
    static constexpr int V_MISO = 35;
    static constexpr int V_SCLK = 36;
    static constexpr int V_MOSI = 37;
    static constexpr int SD_Cs = 38;


    static constexpr int BaroCs = 10;
    // pitot sensors
    static constexpr int P1_Cs = 7;
    static constexpr int P2_Cs = 8; // returns 0 ..
    static constexpr int P3_Cs = 9;
    static constexpr int P4_Cs = 6; // returns 128 0 0 128 ... on reset
    static constexpr int P5_Cs = 5;
    static constexpr int SNSR_Reset = 48;

    static constexpr int TxCan = 1;
    static constexpr int RxCan = 2;
};


