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
#include "RCodeActivateCommand.hpp"
#include "RCodeSetDebugChannelCommand.hpp"
#include "RCodeSendDebugCommand.hpp"
#include "UipUdpCommandChannel.hpp"

int main(void) {

    uint8_t mac[] = { 0x1E, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    UipEthernet uip(mac, PA_7, PA_6, PA_5, PA_8);

    while (uip.connect(5))
        ;
    RCode r;
    UipUdpCommandChannel channel(&r, &uip, 4889);
    RCodeCommandChannel *ch = &channel;
    r.setChannels(&ch, 1);
    RCodeEchoCommand cmd;
    RCodeActivateCommand cmd2;
    RCodeSetDebugChannelCommand cmd3(&r);
    RCodeSendDebugCommand cmd4(&r);
    r.getCommandFinder()->registerCommand(&cmd);
    r.getCommandFinder()->registerCommand(&cmd2);
    r.getCommandFinder()->registerCommand(&cmd3);
    r.getCommandFinder()->registerCommand(&cmd4);
    while (true) {
        r.progressRCode();
        uip.tick();
        uip.dhcpClient.checkLease();
    }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
