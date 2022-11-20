/*
 * ZcodeSerialChannel.h
 *
 *  Created on: 20 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_TEST_C_ARDUINO_ZCODESERIALCHANNEL_H_
#define SRC_TEST_C_ARDUINO_ZCODESERIALCHANNEL_H_
#include <zcode/ZcodeAbstractOutStream.h>
#include <zcode/modules/ZcodeCommand.h>

void ZcodeChannelReadSetup(ZcodeCommandSlot *slot) {
    ZcodeOutStream_WriteField16('B', ZCODE_BIG_FIELD_SIZE);
    ZcodeOutStream_WriteField16('F', ZCODE_FIELD_COUNT);
    ZcodeOutStream_WriteField8('N', 0);
    ZcodeOutStream_WriteField8('M', 0x7);
    ZcodeOutStream_WriteStatus(OK);
}
void ZcodeChannelWriteInfo(ZcodeCommandSlot *slot) {
    ZcodeOutStream_WriteStatus(OK);
}

bool ZcodeOutStream_OpenBool = false;

void ZcodeOutStream_WriteByte(uint8_t value) {
    Serial.print((char) value);
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

#endif /* SRC_TEST_C_ARDUINO_ZCODESERIALCHANNEL_H_ */
