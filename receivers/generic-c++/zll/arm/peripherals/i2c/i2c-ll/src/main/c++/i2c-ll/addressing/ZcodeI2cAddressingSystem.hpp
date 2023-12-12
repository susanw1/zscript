/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_ADDRESSING_ZCODEI2CADDRESSINGSYSTEM_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_ADDRESSING_ZCODEI2CADDRESSINGSYSTEM_HPP_

#include <zcode/ZcodeIncludes.hpp>
#include <zcode/modules/ZcodeCommand.hpp>
#include <zcode/addressing/ZcodeModuleAddressingSystem.hpp>
#include <i2c-ll/lowlevel/I2cManager.hpp>

#define ADDRESSING_SWITCH005 ADDRESSING_SWITCH_UTIL(ZcodeI2cAddressingSystem<ZP>::routeAddress)
#define ADDRESSING_RESP_SWITCH005 ADDRESSING_RESP_SWITCH_UTIL(ZcodeI2cAddressingSystem<ZP>::routeResponse)
#define ADDRESSING_LEVEL005 2

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeI2cAddressingSystem: public ZcodeModuleAddressingSystem<ZP> {
    typedef typename ZP::Strings::string_t string_t;

    typedef typename ZP::LL LL;
    typedef typename LL::HW HW;

    static constexpr char CMD_RESP_I2C_INFO_VAL_OK = 0;
    static constexpr char CMD_RESP_I2C_INFO_VAL_ADDRNACK = 2;
    static constexpr char CMD_RESP_I2C_INFO_VAL_DATANACK = 3;
    static constexpr char CMD_RESP_I2C_INFO_VAL_OTHER = 4;

public:

    static void routeAddress(ZcodeExecutionCommandSlot<ZP> slot, ZcodeAddressingInfo<ZP> *addressingInfo) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        // TODO: is this ok? We interpret 10 bit as a separate set of addresses...
        // My best alternative is doing the same thing on the port, with e.g. bit 8...

        bool tenBit = (addressingInfo->addr & 0x800) != 0;
        uint8_t port = addressingInfo->port;
        uint16_t address = (addressingInfo->addr & 0x7ff);
        if (port >= HW::i2cCount) {
            slot.fail(BAD_ADDRESSING, "Invalid I2C port");
            return;
        }
        I2c<LL> *i2c = I2cManager<LL>::getI2cById(port);

        if (address >= 1024 || (address >= 128 && !tenBit)) {
            slot.fail(BAD_ADDRESSING, (string_t) ZP::Strings::failAddressingInvalid);
            return;
        }

        if (!i2c->lock()) {
            slot.fail(CMD_FAIL, "Port locked");
            return;
        }
        I2cTerminationStatus status = i2c->transmit10(address, tenBit, slot.getBigField()->getData() + addressingInfo->start,
                slot.getBigField()->getLength() - addressingInfo->start);
        i2c->unlock();
        uint8_t infoValue;

        if (status == Complete) {
            return;
        } else if (status == DataNack) {
            infoValue = CMD_RESP_I2C_INFO_VAL_DATANACK;
            slot.fail(CMD_FAIL, "DataNack");
        } else if (status == BusJammed || status == MemoryError || status == OtherError) {
            infoValue = CMD_RESP_I2C_INFO_VAL_OTHER;
            slot.fail(CMD_ERROR, "Fatal I2C error");
        } else if (status == AddressNack) {
            infoValue = CMD_RESP_I2C_INFO_VAL_ADDRNACK;
            slot.fail(CMD_FAIL, "AddressNack");
        } else {
            infoValue = CMD_RESP_I2C_INFO_VAL_OTHER;
            slot.fail(CMD_FAIL, "I2C failure");
        }
        out->writeField8('I', infoValue);
        out->writeField16('A', address);
    }

    static void routeResponse(ZcodeBusInterrupt<ZP> *interrupt, ZcodeOutStream<ZP> *out) {
        bool tenBit = (interrupt->getFoundAddress() & 0x800) != 0;
        uint8_t port = interrupt->getNotificationPort();
        uint16_t address = (interrupt->getFoundAddress() & 0x7ff);
        if (port >= HW::i2cCount) {
            return;
        }
        I2c<LL> *i2c = I2cManager<LL>::getI2cById(port);

        uint8_t chunkSize = ZP::i2cAddressingResponseChunkSize;
        if (out->getReadBufferLength() < chunkSize) {
            return;
        }
        uint8_t *buffer = out->getReadBuffer();

        if (!i2c->lock()) {
            return;
        }
        bool isAtStart = true;

        I2cTerminationStatus status;
        bool done = false;
        while (!done) {
            status = i2c->receive10(address, tenBit, buffer, chunkSize);
            if (status != Complete) {
                break;
            }
            uint8_t i = 0;
            if (isAtStart && buffer[0] == ADDRESS_PREFIX) {
                out->markAddressingContinue();
                i++;
            }
            for (; i < chunkSize; ++i) {
                if (buffer[i] == EOL_SYMBOL) {
                    done = true;
                    break;
                }
                out->writeByte(buffer[i]);
            }
        }
        if (status != Complete) {
            out->writeCommandSequenceSeparator();
            out->markNotification(AddressingNotification);
            out->writeField16('M', interrupt->getNotificationModule());
            out->writeField8('P', interrupt->getNotificationPort());
            out->writeField16('A', address);
            uint8_t infoValue;
            if (status == DataNack) {
                infoValue = CMD_RESP_I2C_INFO_VAL_DATANACK;
                out->writeStatus(CMD_FAIL);
                out->writeBigStringField("DataNack");
            } else if (status == BusJammed || status == MemoryError || status == OtherError) {
                infoValue = CMD_RESP_I2C_INFO_VAL_OTHER;
                out->writeStatus(CMD_ERROR);
                out->writeBigStringField("Fatal I2C error");
            } else if (status == AddressNack) {
                infoValue = CMD_RESP_I2C_INFO_VAL_ADDRNACK;
                out->writeStatus(CMD_FAIL);
                out->writeBigStringField("AddressNack");
            } else {
                infoValue = CMD_RESP_I2C_INFO_VAL_OTHER;
                out->writeStatus(CMD_FAIL);
                out->writeBigStringField("I2C failure");
            }
            out->writeField8('I', infoValue);
        }
        i2c->unlock();
    }
};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_ADDRESSING_ZCODEI2CADDRESSINGSYSTEM_HPP_ */
