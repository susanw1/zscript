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
    RCode r(NULL, 0);
    UipUdpCommandChannel channel(&r, &uip, 4889);
    RCodeExecutionSpaceChannel execCh = RCodeExecutionSpaceChannel(&r, r.getSpace());
    RCodeCommandChannel *chptr[2] = { &channel, &execCh };
    RCodeExecutionSpaceChannel **execPtr = (RCodeExecutionSpaceChannel**) chptr + 1;
    r.setChannels(chptr, 2);
    r.getSpace()->setChannels(execPtr, 1);
    RCodeEchoCommand cmd0 = RCodeEchoCommand();
    RCodeActivateCommand cmd1 = RCodeActivateCommand();
    RCodeSetDebugChannelCommand cmd2 = RCodeSetDebugChannelCommand(&r);
    RCodeCapabilitiesCommand cmd3 = RCodeCapabilitiesCommand(&r);
    RCodeExecutionStateCommand cmd4 = RCodeExecutionStateCommand(r.getSpace());
    RCodeExecutionCommand cmd5 = RCodeExecutionCommand(r.getSpace());
    RCodeExecutionStoreCommand cmd6 = RCodeExecutionStoreCommand(r.getSpace());
    RCodeNotificationHostCommand cmd7 = RCodeNotificationHostCommand(&r);
    RCodeIdentifyCommand cmd8 = RCodeIdentifyCommand();

    r.getCommandFinder()->registerCommand(&cmd8);
    r.getCommandFinder()->registerCommand(&cmd0);
    r.getCommandFinder()->registerCommand(&cmd3);
    r.getCommandFinder()->registerCommand(&cmd1);
    r.getCommandFinder()->registerCommand(&cmd7);
    r.getCommandFinder()->registerCommand(&cmd2);
    r.getCommandFinder()->registerCommand(&cmd4);
    r.getCommandFinder()->registerCommand(&cmd5);
    r.getCommandFinder()->registerCommand(&cmd6);

    while (true) {
        r.progressRCode();
        uip.tick();
        uip.dhcpClient.checkLease();
    }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
