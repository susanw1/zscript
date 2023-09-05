/*
 * EthernetSystem.hpp
 *
 *  Created on: 16 Aug 2023
 *      Author: alicia
 */

#ifndef SRC_MAIN_C___ARDUINO_ETHERNET_MODULE_CHANNELS_ETHERNETSYSTEM_HPP_
#define SRC_MAIN_C___ARDUINO_ETHERNET_MODULE_CHANNELS_ETHERNETSYSTEM_HPP_

#include <Ethernet.h>
#include <ZscriptChannelBuilder.hpp>
#include "../arduino-core-module/persistence/PersistenceSystem.hpp"

namespace Zscript {

template<class ZP>
class EthernetSystem {
    static uint8_t macAddress[6];
    static uint8_t macAddrOffset;
    static bool hasLink;

public:

    static void setup() {
        macAddrOffset = PersistenceSystem<ZP>::reserveSection(6);
        PersistenceSystem<ZP>::readSection(macAddrOffset, 6, macAddress);
    }

    static void setMacAddr(uint8_t *mac) {
        for (uint8_t i = 0; i < 6; ++i) {
            macAddress[i] = mac[i];
        }
        PersistenceSystem<ZP>::writeSection(macAddrOffset, 6, macAddress);
    }

    static void setMacAddr(BigFieldBlockIterator<ZP> mac) {
        for (uint8_t i = 0; i < 6; ++i) {
            macAddress[i] = mac.next();
        }
        PersistenceSystem<ZP>::writeSection(macAddrOffset, 6, macAddress);
    }

    static uint8_t *getMacAddr() {
        return macAddress;
    }

    static bool resetLink() {
        hasLink = false;
        PersistenceSystem<ZP>::readSection(macAddrOffset, 6, macAddress);
        return ensureLink();
    }

    static bool ensureLink() {
        if (hasLink) {
            uint8_t result = Ethernet.maintain();
            if (result == 1 || result == 3) {
                hasLink = false;
                return false;
            }
            return true;
        }
//        if (Ethernet.hardwareStatus() == EthernetNoHardware) {
//            return false;
//        }
//        if (Ethernet.linkStatus() == LinkOFF) {
//            return false;
//        }
        if (Ethernet.begin(macAddress, 20000, 4000)) {
            hasLink = true;
        }
        return hasLink;
    }
};

template<class ZP>
bool EthernetSystem<ZP>::hasLink = false;
template<class ZP>
uint8_t EthernetSystem<ZP>::macAddrOffset = 0;
template<class ZP>
uint8_t EthernetSystem<ZP>::macAddress[6] = {0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed};
}

#endif /* SRC_MAIN_C___ARDUINO_ETHERNET_MODULE_CHANNELS_ETHERNETSYSTEM_HPP_ */
