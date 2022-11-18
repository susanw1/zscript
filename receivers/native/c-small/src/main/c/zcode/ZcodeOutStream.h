/*
 * ZcodeOutStream.h
 *
 *  Created on: 18 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_CHANNEL_ZCODEOUTSTREAM_H_
#define SRC_MAIN_C_ZCODE_CHANNEL_ZCODEOUTSTREAM_H_

#include "ZcodeIncludes.h"

void ZcodeOutStream_WriteByte(uint8_t value);

void ZcodeOutStream_Open();

bool ZcodeOutStream_IsOpen();
// Is defined to do nothing if not open
void ZcodeOutStream_Close();

bool ZcodeOutStream_Lock();
void ZcodeOutStream_Unlock();
bool ZcodeOutStream_IsLocked();

void ZcodeOutStream_WriteBytes(uint8_t const *value, uint16_t l);

void ZcodeOutStream_MarkDebug();
void ZcodeOutStream_MarkNotification(ZcodeNotificationType type);
void ZcodeOutStream_MarkBroadcast();
void ZcodeOutStream_MarkAddressing();
void ZcodeOutStream_MarkAddressingContinue();

void ZcodeOutStream_SilentSucceed();
void ZcodeOutStream_WriteStatus(uint16_t st);

void ZcodeOutStream_WriteBigHexField(const uint8_t *value, ZcodeBigFieldAddress_t length);
void ZcodeOutStream_WriteBigStringField(const char *nullTerminated);
void ZcodeOutStream_WriteBigStringField_(const uint8_t *value, ZcodeBigFieldAddress_t length);

void ZcodeOutStream_WriteCommandSeparator();
void ZcodeOutStream_WriteSequenceErrorHandler();
void ZcodeOutStream_WriteCommandSequenceSeparator();

void ZcodeOutStream_WriteField8(char f, uint8_t v);
void ZcodeOutStream_WriteField8_(uint8_t v);
void ZcodeOutStream_ContinueField8(uint8_t v);

void ZcodeOutStream_WriteField16(char f, uint16_t value);
void ZcodeOutStream_WriteField16_(uint16_t value);
void continueField16(uint16_t value);

void ZcodeOutStream_WriteField32(char f, uint32_t value);
void ZcodeOutStream_WriteField32_(uint32_t value);
void ZcodeOutStream_ContinueField32(uint32_t value);

void ZcodeOutStream_WriteField64(char f, uint64_t value);
void ZcodeOutStream_WriteField64_(uint64_t value);
void ZcodeOutStream_ContinueField64(uint64_t value);

void ZcodeOutStream_WriteFieldL(char f, const uint8_t *value, uint8_t length);

#endif /* SRC_MAIN_C_ZCODE_CHANNEL_ZCODEOUTSTREAM_H_ */
