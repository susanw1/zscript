/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_GENERALI2CACTION_HPP_
#define SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_GENERALI2CACTION_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <net/zscript/model/modules/base/I2cModule.hpp>
#include <Wire.h>

#define COMMAND_EXISTS_0052 EXISTENCE_MARKER_UTIL
#define COMMAND_EXISTS_0053 EXISTENCE_MARKER_UTIL
#define COMMAND_EXISTS_0054 EXISTENCE_MARKER_UTIL

/**
 * Handles the Send, Receive, and Send+Receive cases directly - they're pretty similar
 */
namespace Zscript {

namespace i2c_module {

template<class ZP>
class GeneralI2cAction: I2cSendReceive_CommandDefs {
public:
    static constexpr uint8_t SEND_CODE = I2cSend_CommandDefs::CODE;
    static constexpr uint8_t RECEIVE_CODE = I2cReceive_CommandDefs::CODE;
    static constexpr uint8_t SEND_RECEIVE_CODE = I2cSendReceive_CommandDefs::CODE;

    /** Note, SEND | RECEIVE == SEND_RECEIVE */
    enum class ActionType : uint8_t {
        SEND = 1, RECEIVE = 2, SEND_RECEIVE = 3
    };

    static void executeSendReceive(ZscriptCommandContext<ZP> ctx, ActionType action) {
        uint16_t address;
        if (!ctx.getField(ReqAddress__A, &address)) {
            ctx.status(ResponseStatus::MISSING_KEY);
            return;
        }
        if (ctx.hasField(ReqTenBitMode__T)) {
            ctx.status(ResponseStatus::EXECUTION_ERROR);
            return;
        }
        if (address >= 128) {
            ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
            return;
        }

        CommandOutStream<ZP> out = ctx.getOutStream();

        uint16_t requestedLength = ctx.getField(ReqLength__L, (uint16_t) 0);
        if (requestedLength > 32) {
            ctx.status(ResponseStatus::DATA_TOO_LONG);
            return;
        }

        // initial pass
        uint16_t interface;
        if (!ctx.getReqdFieldCheckLimit(ReqInterface__I, 1, &interface)) {
            return;
        }

        uint16_t retries = ctx.getField(ReqRetries__R, 5);
        if (retries > 255) {
            ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
            return;
        }
        if (retries == 0) {
            retries = 1;
        }
#ifdef WIRE_HAS_TIMEOUT
        Wire.setWireTimeout();
#endif
        uint16_t attemptsDone = 0;
        uint8_t infoValue;
        while (true) {
            attemptsDone++;
            uint8_t status = 0;

            uint8_t buffer[requestedLength];
            switch ((ActionType) action) {
                case ActionType::SEND:
                    Wire.beginTransmission((uint8_t) address);
                    if (ctx.getBigFieldSize() > 0) {
                        for (BigFieldBlockIterator<ZP> tbi = ctx.getBigField(); tbi.hasNext();) {
                            DataArrayWLeng16 data = tbi.nextContiguous();
                            Wire.write(data.data, data.length);
                        }
                    }
                    status = Wire.endTransmission();
                    break;
                case ActionType::RECEIVE: {
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
                case ActionType::SEND_RECEIVE:
                    Wire.beginTransmission((uint8_t) address);
                    if (ctx.getBigFieldSize() > 0) {
                        for (BigFieldBlockIterator<ZP> tbi = ctx.getBigField(); tbi.hasNext();) {
                            DataArrayWLeng16 data = tbi.nextContiguous();
                            Wire.write(data.data, data.length);
                        }
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
                infoValue = RespResultInfo_Values::ok_value;
                if (((uint8_t) action & (uint8_t) ActionType::RECEIVE) != 0) {
                    out.writeBigHex(buffer, requestedLength);
                }
                break;
            } else if (status == 3) {
                // abrupt failure during data send - don't retry, because its status is now unknown
                infoValue = RespResultInfo_Values::dataNack_value;
                ctx.status(ResponseStatus::COMMAND_DATA_NOT_TRANSMITTED);
                break;
            } else if (status == 5 || status == 4) {
                if (recoverBusJam()) {
                    infoValue = RespResultInfo_Values::other_value;
                    ctx.status(ResponseStatus::PERIPHERAL_JAM);
                } else {
                    infoValue = RespResultInfo_Values::other_value;
                    ctx.status(ResponseStatus::COMMAND_FAIL);
                }
                break;
            } else if (status == 6 && action == ActionType::SEND_RECEIVE) {
                infoValue = RespResultInfo_Values::addrNack_value;
                ctx.status(ResponseStatus::COMMAND_NO_TARGET);
                break;
            } else if (attemptsDone < retries) {
                // any other error, keep retrying. Beyond here, we must be at the end of all retries.
            } else if (status == 2) {
                infoValue = RespResultInfo_Values::addrNack_value;
                ctx.status(ResponseStatus::COMMAND_NO_TARGET);
                break;
            } else {
                infoValue = RespResultInfo_Values::other_value;
                ctx.status(ResponseStatus::COMMAND_FAIL);
                break;
            }

        }
        out.writeField(RespAttempts__R, attemptsDone);
        out.writeField(RespResultInfo__I, infoValue);
        return;
    }

#ifdef ZSCRIPT_SUPPORT_ADDRESSING
    static void executeAddressing(ZscriptAddressingContext<ZP> ctx) {
        uint16_t interface;
        uint16_t address;
        AddressOptIterator<ZP> iter = ctx.getAddressSegments();
        iter.next();
        OptInt16 seg = iter.next();
        if (!seg.isPresent) {
            ctx.status(ResponseStatus::ADDRESS_NOT_FOUND);
            return;
        }
        interface = seg.value;
        if (interface > 0) {
            ctx.status(ResponseStatus::ADDRESS_NOT_FOUND);
            return;
        }
        seg = iter.next();
        if (!seg.isPresent) {
            ctx.status(ResponseStatus::ADDRESS_NOT_FOUND);
            return;
        }
        address = seg.value;
        if (address >= 128) {
            ctx.status(ResponseStatus::ADDRESS_NOT_FOUND);
            return;
        }
        Wire.beginTransmission((uint8_t) address);
        if (ctx.getAddressedDataSize() > 0) {
            for (CombinedTokenBlockIterator<ZP> tbi = ctx.getAddressedData(); tbi.hasNext();) {
                DataArrayWLeng16 data = tbi.nextContiguous();
                Wire.write(data.data, data.length);
            }
        }
        Wire.write('\n');
        uint8_t status = Wire.endTransmission();
        if (status == 0) {
            //done
        } else if (status == 3) {
            ctx.status(ResponseStatus::COMMAND_DATA_NOT_TRANSMITTED);
        } else if (status == 5 || status == 4) {
            if (recoverBusJam()) {
                ctx.status(ResponseStatus::PERIPHERAL_JAM);
            } else {
                ctx.status(ResponseStatus::COMMAND_FAIL);
            }
        } else if (status == 2) {
            ctx.status(ResponseStatus::COMMAND_NO_TARGET);
        } else {
            ctx.status(ResponseStatus::COMMAND_FAIL);
        }
    }
#endif

    static bool recoverBusJam() {
        int attempts = 18;
        Wire.end();
        pinMode(SDA, INPUT_PULLUP);
        pinMode(SCL, INPUT_PULLUP);
        digitalWrite(SCL, LOW);
        digitalWrite(SDA, LOW);
        pinMode(SDA, INPUT_PULLUP);
        pinMode(SCL, INPUT_PULLUP);

        if (digitalRead(SDA)) {
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

}

}

#endif /* SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_GENERALI2CACTION_HPP_ */
