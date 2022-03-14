/*
 * UipUdpOutStream.cpp
 *
 *  Created on: 27 Sep 2020
 *      Author: robert
 */
#include "UipUdpOutStream.hpp"

void UipUdpOutStream::openResponse(ZcodeCommandChannel *target) {
    ((UipUdpCommandChannel*) target)->open(&write);
    open = true;
}

void UipUdpOutStream::openNotification(ZcodeCommandChannel *target) {
    ((UipUdpCommandChannel*) target)->open(&write);
    open = true;
}

void UipUdpOutStream::openDebug(ZcodeCommandChannel *target) {
    ((UipUdpCommandChannel*) target)->open(&write);
    open = true;
}
