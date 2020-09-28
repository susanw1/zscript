/*
 * UipUdpOutStream.cpp
 *
 *  Created on: 27 Sep 2020
 *      Author: robert
 */
#include "UipUdpOutStream.hpp"

void UipUdpOutStream::openResponse(RCodeCommandChannel *target) {
    ((UipUdpCommandChannel*) target)->open(&write);
    open = true;
}

void UipUdpOutStream::openNotification(RCodeCommandChannel *target) {
    ((UipUdpCommandChannel*) target)->open(&write);
    open = true;
}

void UipUdpOutStream::openDebug(RCodeCommandChannel *target) {
    ((UipUdpCommandChannel*) target)->open(&write);
    open = true;
}
