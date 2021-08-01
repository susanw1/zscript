/*
 * Usb.cpp
 *
 *  Created on: 24 Jul 2021
 *      Author: robert
 */
#include "../Usb.hpp"

Usb *Usb::usb = NULL;

void UsbTargetValueCallback() {
    Usb::usb->targetValueCallback(Usb::usb->id);
}

void Usb::setTargetValue(void (*volatile targetValueCallback)(UartIdentifier), uint8_t targetValue) {
    this->targetValueCallback = targetValueCallback;
    internal.setTargetValue(&UsbTargetValueCallback, targetValue);
}
