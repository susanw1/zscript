/*
 * GeneralI2cAction.hpp
 *
 *  Created on: 26 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ARDUINO_I2C_MODULE_COMMANDS_GENERALI2CACTION_HPP_
#define SRC_MAIN_C___ARDUINO_I2C_MODULE_COMMANDS_GENERALI2CACTION_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <Wire.h>

/**
 * Handles the Send, Receive, and Send+Receive cases directly - they're pretty similar
 */
template<class ZP>
class GeneralI2cAction {
public:
    static constexpr char CMD_PARAM_I2C_PORT_P = 'P';
    static constexpr char CMD_PARAM_I2C_ADDR_A = 'A';
    static constexpr char CMD_PARAM_TENBIT_ADDR_N = 'N';
    static constexpr char CMD_PARAM_RETRIES_T = 'T';
    static constexpr char CMD_PARAM_READ_LENGTH = 'L';

    static constexpr char CMD_RESP_I2C_ADDR_A = 'A';
    static constexpr char CMD_RESP_I2C_INFO_I = 'I';
    static constexpr char CMD_RESP_I2C_ATTEMPTS_T = 'T';
    static constexpr char CMD_RESP_I2C_INFO_VAL_OK = 0;
    static constexpr char CMD_RESP_I2C_INFO_VAL_ADDRNACK = 2;
    static constexpr char CMD_RESP_I2C_INFO_VAL_DATANACK = 3;
    static constexpr char CMD_RESP_I2C_INFO_VAL_OTHER = 4;

    /** Note, SEND | RECEIVE == SEND_RECEIVE */
    enum ActionType {
        SEND = 1, RECEIVE = 2, SEND_RECEIVE = 3
    };

    static void executeSendReceive(ZcodeExecutionCommandSlot<ZP> slot, uint8_t action) {
        uint16_t address;
        if (!slot.getFields()->get(CMD_PARAM_I2C_ADDR_A, &address)) {
            slot.fail(BAD_PARAM, "I2C address missing");
            return;
        }
        if (slot.getFields()->has(CMD_PARAM_TENBIT_ADDR_N)) {
            slot.fail(BAD_PARAM, "10 bit I2C not supported");
            return;
        }
        if (address >= 128) {
            slot.fail(BAD_PARAM, "I2C address out of range");
            return;
        }

        ZcodeOutStream<ZP> *out = slot.getOut();

        uint16_t requestedLength = slot.getFields()->get(CMD_PARAM_READ_LENGTH, (uint16_t) 0);
        if (requestedLength > 32) {
            slot.fail(TOO_BIG, "I2C requested length too large");
            return;
        }

        // initial pass
        uint16_t port;
        if (!slot.getFields()->get(CMD_PARAM_I2C_PORT_P, &port)) {
            slot.fail(BAD_PARAM, "Missing I2C port");
            return;
        } else if (port > 0) {
            slot.fail(BAD_PARAM, "Invalid I2C port");
            return;
        }

        uint16_t retries = slot.getFields()->get(CMD_PARAM_RETRIES_T, 5);
        if (retries > 255) {
            slot.fail(BAD_PARAM, "Retries Too Large");
            return;
        }
        if (retries == 0) {
            retries = 1;
        }
#ifdef WIRE_HAS_TIMEOUT
        Wire.setWireTimeout();
#endif
        const ZcodeBigField<ZP> *bigField = slot.getBigField();
        uint16_t attemptsDone = 0;
        uint8_t infoValue;
        while (true) {
            attemptsDone++;
            uint8_t status = 0;

            uint8_t buffer[requestedLength];
            switch ((ActionType) action) {
            case SEND:
                Wire.beginTransmission((uint8_t) address);
                if (bigField->getLength() > 0) {
                    Wire.write(bigField->getData(), bigField->getLength());
                }
                status = Wire.endTransmission();
                break;
            case RECEIVE:
                {
                uint8_t len = Wire.requestFrom((uint8_t) address, (uint8_t) requestedLength);
                if (len == 0) {
                    status = 2;
                } else if (len != requestedLength) {
                    status = 4;
                } else {
                    status = 0;
                    for (uint8_t i = 0; i < requestedLength; i++) {
                        buffer[i] = (uint8_t) Wire.read();
                    }
                }
            }
                break;
            case SEND_RECEIVE:
                Wire.beginTransmission((uint8_t) address);
                if (bigField->getLength() > 0) {
                    Wire.write(bigField->getData(), bigField->getLength());
                }
                status = Wire.endTransmission(false);
                if (status == 0) {
                    uint8_t len = Wire.requestFrom((uint8_t) address, (uint8_t) requestedLength);
                    if (len == 0) {
                        status = 6;
                    } else if (len != requestedLength) {
                        status = 4;
                    } else {
                        status = 0;
                        for (uint8_t i = 0; i < requestedLength; i++) {
                            buffer[i] = (uint8_t) Wire.read();
                        }
                    }
                    break;
                }
                break;
            }

            if (status == 0) {
                infoValue = CMD_RESP_I2C_INFO_VAL_OK;
                out->writeStatus(OK);
                if ((action & RECEIVE) != 0) {
                    out->writeBigHexField(buffer, requestedLength);
                }
                break;
            } else if (status == 3) {
                // abrupt failure during data send - don't retry, because its status is now unknown
                infoValue = CMD_RESP_I2C_INFO_VAL_DATANACK;
                slot.fail(CMD_FAIL, "DataNack");
                break;
            } else if (status == 5 || status == 4) {
                if (recoverBusJam()) {
                    infoValue = CMD_RESP_I2C_INFO_VAL_OTHER;
                    slot.fail(CMD_ERROR, "Fatal I2C error");
                } else {
                    infoValue = CMD_RESP_I2C_INFO_VAL_OTHER;
                    slot.fail(CMD_FAIL, "I2C failure");
                }
                break;
            } else if (status == 6 && action == ActionType::SEND_RECEIVE) {
                infoValue = CMD_RESP_I2C_INFO_VAL_ADDRNACK;
                slot.fail(CMD_FAIL, "Address2Nack");
                break;
            } else if (attemptsDone < retries) {
                // any other error, keep retrying. Beyond here, we must be at the end of all retries.
            } else if (status == 2) {
                infoValue = CMD_RESP_I2C_INFO_VAL_ADDRNACK;
                slot.fail(CMD_FAIL, "AddressNack");
                break;
            } else {
                infoValue = CMD_RESP_I2C_INFO_VAL_OTHER;
                slot.fail(CMD_FAIL, "I2C failure");
                break;
            }

        }
        out->writeField8(CMD_RESP_I2C_ATTEMPTS_T, attemptsDone);
        out->writeField8(CMD_RESP_I2C_INFO_I, infoValue);
        out->writeField16(CMD_RESP_I2C_ADDR_A, address);
        return;

    }

    static bool recoverBusJam() {
        int attempts = 18;
        Wire.end();
        pinMode(SDA, INPUT_PULLUP);
        pinMode(SCL, INPUT_PULLUP);
        digitalWrite(SCL, LOW);
        digitalWrite(SDA, LOW);
        pinMode(SDA, INPUT_PULLUP);
        pinMode(SCL, INPUT_PULLUP);

        if (digitalRead (SDA)) {
            if (!digitalRead(SCL)) {
                Wire.begin();
                return false;
            }
            Wire.begin();
            return true;
        }
        delay(10);
        while (!digitalRead(SDA) && attempts > 0) {
            pinMode(SCL, INPUT_PULLUP);
            delay(10);
            pinMode(SCL, OUTPUT);
            digitalWrite(SCL, LOW);
            delay(10);
            attempts--;
        }
        pinMode(SDA, OUTPUT);
        digitalWrite(SDA, LOW);

        pinMode(SCL, INPUT_PULLUP);
        pinMode(SDA, INPUT_PULLUP);
        Wire.begin();
        return digitalRead(SDA);

    }

};

#endif /* SRC_MAIN_C___ARDUINO_I2C_MODULE_COMMANDS_GENERALI2CACTION_HPP_ */
