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
//#include "RCode.hpp"
//#include "commands/RCodeActivateCommand.hpp"
//#include "commands/RCodeSetDebugChannelCommand.hpp"
//#include "commands/RCodeEchoCommand.hpp"
//#include "commands/RCodeExecutionStateCommand.hpp"
//#include "commands/RCodeExecutionCommand.hpp"
//#include "commands/RCodeExecutionStoreCommand.hpp"
//#include "commands/RCodeNotificationHostCommand.hpp"
//#include "commands/RCodeCapabilitiesCommand.hpp"
//#include "executionspace/RCodeExecutionSpaceChannel.hpp"
//#include "../commands/RCodeIdentifyCommand.hpp"

//#include "UipUdpCommandChannel.hpp"

int main(void) {
    wait_us(5000000);
    uint8_t mac[] = { 0x1E, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    UipEthernet uip(mac, PA_7, PA_6, PA_5, PA_4);
    while (uip.connect(5))
        ;
    RCode z(NULL, 0);
    UipUdpCommandChannel channel(&z, &uip, 4889);
    RCodeExecutionSpaceChannel execCh = RCodeExecutionSpaceChannel(&z, z.getSpace());
    RCodeCommandChannel *chptr[2] = { &channel, &execCh };
    RCodeExecutionSpaceChannel **execPtr = (RCodeExecutionSpaceChannel**) chptr + 1;
    z.setChannels(chptr, 2);
    z.getSpace()->setChannels(execPtr, 1);
    RCodeEchoCommand cmd0 = RCodeEchoCommand();
    RCodeActivateCommand cmd1 = RCodeActivateCommand();
    RCodeSetDebugChannelCommand cmd2 = RCodeSetDebugChannelCommand(&z);
    RCodeCapabilitiesCommand cmd3 = RCodeCapabilitiesCommand(&z);
    RCodeExecutionStateCommand cmd4 = RCodeExecutionStateCommand(z.getSpace());
    RCodeExecutionCommand cmd5 = RCodeExecutionCommand(z.getSpace());
    RCodeExecutionStoreCommand cmd6 = RCodeExecutionStoreCommand(z.getSpace());
    RCodeNotificationHostCommand cmd7 = RCodeNotificationHostCommand(&z);
    RCodeIdentifyCommand cmd8 = RCodeIdentifyCommand();

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
        z.progressRCode();
        uip.tick();
        uip.dhcpClient.checkLease();
    }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
