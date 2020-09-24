/*
 * UipUdpChannelManager.cpp
 *
 *  Created on: 11 Sep 2020
 *      Author: robert
 */

#include "UipUdpChannelManager.hpp"

#include <mbed.h>

DigitalOut led(PB_0);
void UipUdpChannelManager::checkSequences() {
    led = 1;
    if (!hasCheckedPackets && !reader.isReading()) {
        hasCheckedPackets = true;
        for (int i = 0; i < RCodeParameters::uipChannelNum; i++) {
            channels[i].unsetHasSequence();
        }
        if (reader.open()) {
            IpAddress ip = reader.remoteIP();
            uint16_t port = reader.remotePort();
            uint8_t *addr = ip.rawAddress();
            UipUdpCommandChannel *match = NULL;
            for (int i = 0; i < RCodeParameters::uipChannelNum; i++) {
                if (channels[i].matches(addr, port)) {
                    match = channels + i;
                    break;
                }
            }
            if (match == NULL) {
                for (int i = 0; i < RCodeParameters::uipChannelNum; i++) {
                    if (!channels[i].isInUse()) {
                        match = channels + i;
                        break;
                    }
                }
                if (match != NULL) {
                    match->setAddress(addr, port);
                }
            }
            if (match != NULL) {
                match->setHasSequence();
            }
        }
        hasCheckedPackets = false;
    }
    led = 0;
}
