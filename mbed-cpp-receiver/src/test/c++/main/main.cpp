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
//#include "UipEthernet.h"
//#include "Zcode.hpp"
//#include "commands/ZcodeActivateCommand.hpp"
//#include "commands/ZcodeSetDebugChannelCommand.hpp"
//#include "commands/ZcodeEchoCommand.hpp"
//#include "commands/ZcodeExecutionStateCommand.hpp"
//#include "commands/ZcodeExecutionCommand.hpp"
//#include "commands/ZcodeExecutionStoreCommand.hpp"
//#include "commands/ZcodeNotificationHostCommand.hpp"
//#include "commands/ZcodeCapabilitiesCommand.hpp"
//#include "executionspace/ZcodeExecutionSpaceChannel.hpp"
//#include "../commands/ZcodeIdentifyCommand.hpp"

//#include "UipUdpCommandChannel.hpp"

int main(void) {
    wait_us(5000000);
    uint8_t mac[] = { 0x1E, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    UipEthernet uip(mac, PA_7, PA_6, PA_5, PA_4);
    while (uip.connect(5))
        ;
    Zcode z(NULL, 0);
    UipUdpCommandChannel channel(&z, &uip, 4889);
    ZcodeExecutionSpaceChannel execCh = ZcodeExecutionSpaceChannel(&z, z.getSpace());
    ZcodeCommandChannel *chptr[2] = { &channel, &execCh };
    ZcodeExecutionSpaceChannel **execPtr = (ZcodeExecutionSpaceChannel**) chptr + 1;
    z.setChannels(chptr, 2);
    z.getSpace()->setChannels(execPtr, 1);
    ZcodeEchoCommand cmd0 = ZcodeEchoCommand();
    ZcodeActivateCommand cmd1 = ZcodeActivateCommand();
    ZcodeSetDebugChannelCommand cmd2 = ZcodeSetDebugChannelCommand(&z);
    ZcodeCapabilitiesCommand cmd3 = ZcodeCapabilitiesCommand(&z);
    ZcodeExecutionStateCommand cmd4 = ZcodeExecutionStateCommand(z.getSpace());
    ZcodeExecutionCommand cmd5 = ZcodeExecutionCommand(z.getSpace());
    ZcodeExecutionStoreCommand cmd6 = ZcodeExecutionStoreCommand(z.getSpace());
    ZcodeNotificationHostCommand cmd7 = ZcodeNotificationHostCommand(&z);
    ZcodeIdentifyCommand cmd8 = ZcodeIdentifyCommand();

    z.getCommandFinder()->registerCommand(&cmd8);
    z.getCommandFinder()->registerCommand(&cmd0);
    z.getCommandFinder()->registerCommand(&cmd3);
    z.getCommandFinder()->registerCommand(&cmd1);
    z.getCommandFinder()->registerCommand(&cmd7);
    z.getCommandFinder()->registerCommand(&cmd2);
    z.getCommandFinder()->registerCommand(&cmd4);
    z.getCommandFinder()->registerCommand(&cmd5);
    z.getCommandFinder()->registerCommand(&cmd6);

    while (true) {
        z.progressZcode();
        uip.tick();
        uip.dhcpClient.checkLease();
    }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
