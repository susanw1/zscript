/*
 * ZCodeLocalChannel.h
 *
 *  Created on: 18 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_TEST_C_SUPPORT_ZCODELOCALCHANNEL_H_
#define SRC_TEST_C_SUPPORT_ZCODELOCALCHANNEL_H_

#include <stdio.h>
#include <zcode/ZcodeAbstractOutStream.h>
#include <zcode/modules/ZcodeCommand.h>

void ZcodeChannelReadSetup() {
    ZcodeOutStream_WriteStatus(OK);
}
void ZcodeChannelWriteInfo() {
    ZcodeOutStream_WriteStatus(OK);
}

bool ZcodeOutStream_OpenBool = false;

void ZcodeOutStream_WriteByte(uint8_t value) {
    printf("%c", value);
}

void ZcodeOutStream_Open() {
    ZcodeOutStream_OpenBool = true;
}

bool ZcodeOutStream_IsOpen() {
    return ZcodeOutStream_OpenBool;
}

void ZcodeOutStream_Close() {
    ZcodeOutStream_OpenBool = false;
}

#endif /* SRC_TEST_C_SUPPORT_ZCODELOCALCHANNEL_H_ */
