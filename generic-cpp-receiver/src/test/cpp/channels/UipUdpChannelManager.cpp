/*
 * UipUdpChannelManager.cpp
 *
 *  Created on: 11 Sep 2020
 *      Author: robert
 */

#include "UipUdpChannelManager.hpp"

#include <mbed.h>

void UipUdpChannelManager::checkSequences() {
    if (!hasCheckedPackets && !read.isReading()) {
        hasCheckedPackets = true;
        for (int i = 0; i < RCodeParameters::uipChannelNum; i++) {
            channels[i].unsetHasSequence();
        }
        if (read.open()) {
            IpAddress ip = read.remoteIP();
            uint16_t port = read.remotePort();
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
                out.openResponse(match);
                out.writeBigStringField("hello");
                out.close();
                match->setHasSequence();
            }
        }
        hasCheckedPackets = false;
    }
}
