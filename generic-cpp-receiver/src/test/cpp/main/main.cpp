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
DigitalOut myled(PB_0);

#define REGFIELD(r,m,v) (r)=((r)&~(m))|(v)
#define REGFINDVAL(p,v) (v)<<(p)

int main(void) {
    uint8_t mac[] = { 0x1E, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    UipEthernet uip(mac, PA_7, PA_6, PA_5, PA_8);
    int connected = -1;
    connected = uip.connect(5);
    while (1) {
        if (connected == -1) {
            uip.disconnect();
            myled = 1;
            connected = uip.connect(5);
            myled = 0;
        } else if (connected == 0) {
            myled = 1;
            wait_us(1000000);
            myled = 0;
			connected = 1;
        } else if (connected == 1) {
            myled = 1;
            wait_us(10);
            myled = 0;
            uip.tick();
            uip.dhcpClient.checkLease();
        }
        wait_us(10);
    }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
