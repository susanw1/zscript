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
#include "commands/RCodeSendDebugCommand.hpp"
#include "commands/RCodeExecutionStateCommand.hpp"
#include "commands/RCodeExecutionCommand.hpp"
#include "commands/RCodeExecutionStoreCommand.hpp"
#include "commands/RCodeNotificationHostCommand.hpp"
#include "executionspace/RCodeExecutionSpaceChannel.hpp"
#include "UipUdpCommandChannel.hpp"

DigitalOut led(PB_0);

int main(void) {
    led = 1;
    uint8_t mac[] = { 0x1E, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    UipEthernet uip(mac, PA_7, PA_6, PA_5, PA_8);

    while (uip.connect(5))
        ;
    RCode r(NULL, 0);
    UipUdpCommandChannel channel(&r, &uip, 4889);
    RCodeExecutionSpaceChannel execCh = RCodeExecutionSpaceChannel(&r,
            r.getSpace());
    RCodeCommandChannel *chptr[2] = { &channel, &execCh };
    RCodeExecutionSpaceChannel **execPtr = (RCodeExecutionSpaceChannel**) chptr
            + 1;
    r.setChannels(chptr, 1);
    r.getSpace()->setChannels(execPtr, 1);
    RCodeEchoCommand cmd0 = RCodeEchoCommand();
    RCodeActivateCommand cmd1 = RCodeActivateCommand();
    RCodeSetDebugChannelCommand cmd2 = RCodeSetDebugChannelCommand(&r);
    RCodeSendDebugCommand cmd3 = RCodeSendDebugCommand(&r);
    RCodeExecutionStateCommand cmd4 = RCodeExecutionStateCommand(r.getSpace());
    RCodeExecutionCommand cmd5 = RCodeExecutionCommand(r.getSpace());
    RCodeExecutionStoreCommand cmd6 = RCodeExecutionStoreCommand(r.getSpace());
    RCodeNotificationHostCommand cmd7 = RCodeNotificationHostCommand(&r);
    r.getCommandFinder()->registerCommand(&cmd0);
    r.getCommandFinder()->registerCommand(&cmd2);
    r.getCommandFinder()->registerCommand(&cmd3);
    r.getCommandFinder()->registerCommand(&cmd4);
    r.getCommandFinder()->registerCommand(&cmd5);
    r.getCommandFinder()->registerCommand(&cmd6);
    r.getCommandFinder()->registerCommand(&cmd7);
    while (true) {
        r.progressRCode();
        uip.tick();
        uip.dhcpClient.checkLease();
    }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
