/*
 * UsbInternal.cpp
 *
 *  Created on: 24 Jul 2021
 *      Author: robert
 */

#include "UsbInternal.hpp"

void USB_LP_IRQHandler() {
    UsbInternal::usbI->interrupt();
}
void USB_HP_IRQHandler() {
    UsbInternal::usbI->interrupt();
}
UsbInternal *UsbInternal::usbI = NULL;

void UsbDataTxOverflowCallback(SerialIdentifier id) {
    UsbInternal::usbI->checkBuffers();
}
