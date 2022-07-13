#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEI2CSENDCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEI2CSENDCOMMAND_HPP_

#include <modules/ZcodeCommand.hpp>
#include "../../LowLevel/I2cLowLevel/I2cManager.hpp"

template<class ZP>
class ZcodeI2cSendCommand: public ZcodeCommand<ZP> {
public:
    static constexpr uint8_t CODE = 0x02;

    static constexpr char CMD_PARAM_I2C_PORT_P = 'P';
    static constexpr char CMD_PARAM_I2C_ADDR_A = 'A';
    static constexpr char CMD_PARAM_TENBIT_ADDR_N = 'N';
    static constexpr char CMD_PARAM_RETRIES_T = 'T';

    typedef struct {
        uint8_t port;
        uint8_t status;
        uint8_t attemptsLeft;
    } StoredI2cData;

    typedef typename ZP::LL LL;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        uint8_t **i2cStoredPointer = slot.getStoredPointerData();
        StoredI2cData *storedI2cData = slot.getStoredData();

        uint16_t address;
        if (!slot.getFields()->get(CMD_PARAM_I2C_ADDR_A, &address)) {
            slot.fail(BAD_PARAM, "I2C address missing");
            return;
        }
        bool tenBit = slot->getFields()->has(CMD_PARAM_TENBIT_ADDR_N);
        if (address >= 1024 || (!tenBit && address >= 128)) {
            slot.fail(BAD_PARAM, "I2C address out of range");
            return;
        }

        if (*i2cStoredPointer == NULL) {
            // initial pass
            uint16_t port;
            if (!slot.getFields()->get(CMD_PARAM_I2C_PORT_P, &port)) {
                slot.fail(BAD_PARAM, "Missing I2C port");
                return;
            } else if (port >= ZP::LL::i2cCount) {
                slot.fail(BAD_PARAM, "Invalid I2C port");
                return;
            }

            uint8_t retries = slot->getFields()->get(CMD_PARAM_RETRIES_T, 5);
            if (retries == 0) {
                retries = 1;
            }

            storedI2cData->port = port;
            storedI2cData->attemptsLeft = retries;
        } else {
            // we've returned after callback. What happened, then?
            switch ((I2cTerminationStatus) storedI2cData->status) {
            case I2cTerminationStatus::Complete:
                // TODO
                break;
            }
        }

        I2c<LL> *i2cPort = I2cManager<LL>::getI2cById(storedI2cData->port);

        if (storedI2cData->attemptsLeft > 1) {
            // FIXME!!
        }

        *i2cStoredPointer = i2cPort;

        ZcodeBigField<ZP> *bigField = slot.getBigField();
        if (slot.isParallel()) {
            i2cPort->asyncTransmit10(PeripheralOperationMode::DMA, address, tenBit, bigField->getData(), bigField->getLength(), asyncCallback);
        } else {
            i2cPort->transmit10(address, tenBit, bigField->getData(), bigField->getLength());
        }

        slot.getOut()->writeStatus(OK);
    }

    static void asyncCallback(I2c<LL> *i2c, I2cTerminationStatus status) {
        ZcodeRunningCommandSlot<ZP> *slot;
        uint8_t **i2cStoredPointer;
        for (uint8_t i = 0, n = Zcode<ZP>::zcode.getChannelCount(); i < n; i++) {
            if (Zcode<ZP>::zcode.getChannel(i)->runner->dataPointer == i2c) {
                slot = Zcode<ZP>::zcode.getChannel(i)->runner;
            }
        }

        ((StoredI2cData*) &slot->storedData)->status = (uint8_t) status;
        slot->setNeedsAction();
    }
};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEI2CSENDCOMMAND_HPP_ */
