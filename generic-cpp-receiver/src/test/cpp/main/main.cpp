/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    USB_Device/CDC_Standalone/Core/Src/main.c
 * @author  MCD Application Team
 * @brief   USB device CDC demo main file
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include <mbed.h>
#include "UipEthernet.h"
#include "RCode.hpp"
#include "RCodeEchoCommand.hpp"
#include "UipUdpChannelManager.hpp"

DigitalOut led(PB_0);
int main(void) {

    uint8_t mac[] = { 0x1E, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    UipEthernet uip(mac, PA_7, PA_6, PA_5, PA_8);

    while (uip.connect(5))
        ;
    UdpSocket socket(&uip);

    socket.begin(4889);

    RCode r;
    UipUdpChannelManager uipManager(&socket, &r);
    RCodeCommandChannel *channels[RCodeParameters::uipChannelNum];
    for (int i = 0; i < RCodeParameters::uipChannelNum; ++i) {
        channels[i] = uipManager.getChannels() + i;
    }
    r.setChannels(channels, RCodeParameters::uipChannelNum);
    RCodeEchoCommand cmd;
    r.getCommandFinder()->registerCommand(&cmd);
    while (true) {
        led = 0;
        r.progressRCode();
        led = 1;
        uip.tick();
        uip.dhcpClient.checkLease();
    }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
