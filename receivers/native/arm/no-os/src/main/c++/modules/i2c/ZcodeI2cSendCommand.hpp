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
    static constexpr char CMD_PARAM_CONT_ON_FAIL = 'C';

    static constexpr char CMD_RESP_I2C_ADDR_A = 'A';
    static constexpr char CMD_RESP_I2C_INFO_I = 'I';
    static constexpr char CMD_RESP_I2C_ATTEMPTS_T = 'T';
    static constexpr char CMD_RESP_I2C_INFO_VAL_OK = 0;
    static constexpr char CMD_RESP_I2C_INFO_VAL_ADDRNACK = 2;
    static constexpr char CMD_RESP_I2C_INFO_VAL_DATANACK = 3;
    static constexpr char CMD_RESP_I2C_INFO_VAL_OTHER = 4;

    typedef struct {
        uint8_t port;
        uint8_t status;
        uint8_t attemptsLeft;
        uint8_t initialRetries;
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

        ZcodeOutStream<ZP> *out = slot.getOut();
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
            if (retries > 255) {
                slot.fail(BAD_PARAM, "Retries Too Large");
                return;
            }
            if (retries == 0) {
                retries = 1;
            }

            if (!I2cManager<LL>::getI2cById(port)->lock()) {
                // TODO: actually, what's best here? Should sort-of fail, and be re-executed soon.
                slot.fail(CMD_FAIL, "Port locked");
                return;
            }

            storedI2cData->port = port;
            storedI2cData->attemptsLeft = retries;
            storedI2cData->initialRetries = retries;

            out->writeField16(CMD_RESP_I2C_ADDR_A, address);
        } else {
            // subsequent pass, we've transmitted
            bool failStatus = slot->getFields()->has(CMD_PARAM_CONT_ON_FAIL) ? OK : CMD_FAIL;

            I2cTerminationStatus status = (I2cTerminationStatus) storedI2cData->status;
            bool terminating = true;
            uint8_t infoValue;

            if (status == Complete) {
                infoValue = CMD_RESP_I2C_INFO_VAL_OK;
                out->writeStatus(OK);
            } else if (status == DataNack) {
                // abrupt failure during data send - don't retry, because its status is now unknown
                infoValue = CMD_RESP_I2C_INFO_VAL_DATANACK;
                slot.fail(failStatus, "DataNack");
            } else if (status == BusJammed || status == MemoryError || status == OtherError) {
                // these are probably fatal, and should not be retried.
                infoValue = CMD_RESP_I2C_INFO_VAL_OTHER;
                slot.fail(CMD_ERROR, "Fatal I2C error");
            } else if (storedI2cData->attemptsLeft-- > 0) {
                // any other error, keep retrying. Beyond this, we must be at the end of all retries.
                slot.unsetComplete();
                terminating = false;
            } else if (status == AddressNack) {
                infoValue = CMD_RESP_I2C_INFO_VAL_ADDRNACK;
                slot.fail(failStatus, "AddressNack");
            } else {
                infoValue = CMD_RESP_I2C_INFO_VAL_OTHER;
                slot.fail(CMD_FAIL, "I2C failure");
            }

            if (terminating) {
                out->writeField8(CMD_RESP_I2C_INFO_I, infoValue);
                ((I2c<LL>*) *i2cStoredPointer)->unlock();
                return;
            }
        }

        I2c<LL> *i2cPort = I2cManager<LL>::getI2cById(storedI2cData->port);
        *i2cStoredPointer = i2cPort;

        ZcodeBigField<ZP> *bigField = slot.getBigField();

        PeripheralOperationMode mode = slot.isParallel() ? PeripheralOperationMode::DMA : PeripheralOperationMode::SYNCHRONOUS;
        i2cPort->asyncTransmit10(PeripheralOperationMode::DMA, address, tenBit, bigField->getData(), bigField->getLength(), asyncCallback);
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
