#ifndef SRC_MAIN_CPP_ZCODE_COMMANDS_ZCODEGENERALI2CACTION_HPP_
#define SRC_MAIN_CPP_ZCODE_COMMANDS_ZCODEGENERALI2CACTION_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <arm-no-os/i2c-module/lowlevel/I2cManager.hpp>

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

    typedef struct {
        uint8_t port;
        uint8_t status;
        uint8_t attemptsLeft;
        uint8_t initialRetries;
    } StoredI2cData;

    union I2cDataInPlace {
        uint32_t var;
        StoredI2cData data;
    };

    typedef typename ZP::LL LL;

    /** Note, SEND | RECEIVE == SEND_RECEIVE */
    enum ActionType {
        SEND = 1, RECEIVE = 2, SEND_RECEIVE = 3
    };

    static void executeSendReceive(ZcodeExecutionCommandSlot<ZP> slot, uint8_t action) {
        static_assert(sizeof(I2cDataInPlace) == sizeof(uint32_t), "StoredI2cData wouldn't fit into uint32_t");
        volatile uint8_t **i2cStoredPointer = slot.getStoredPointerData();
        volatile StoredI2cData *storedI2cData = &((I2cDataInPlace*) slot.getStoredData())->data;

        uint16_t address;
        if (!slot.getFields()->get(CMD_PARAM_I2C_ADDR_A, &address)) {
            slot.fail(BAD_PARAM, "I2C address missing");
            return;
        }
        bool tenBit = slot.getFields()->has(CMD_PARAM_TENBIT_ADDR_N);
        if (address >= 1024 || (!tenBit && address >= 128)) {
            slot.fail(BAD_PARAM, "I2C address out of range");
            return;
        }

        ZcodeOutStream<ZP> *out = slot.getOut();

        uint16_t requestedLength = slot.getFields()->get(CMD_PARAM_READ_LENGTH, (uint16_t) 0);
        if (requestedLength > out->getReadBufferLength()) {
            slot.fail(TOO_BIG, "I2C requested length too large");
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

            uint16_t retries = slot.getFields()->get(CMD_PARAM_RETRIES_T, 5);
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
            slot.unsetComplete();
        } else {
            // subsequent pass, we've (re)transmitted

            I2cTerminationStatus status = (I2cTerminationStatus) storedI2cData->status;
            bool terminating = true;
            uint8_t infoValue;

            if (status == Complete) {
                infoValue = CMD_RESP_I2C_INFO_VAL_OK;
                out->writeStatus(OK);
                if ((action & RECEIVE) != 0) {
                    out->writeBigHexField(out->getReadBuffer(), requestedLength);
                }
            } else if (status == DataNack) {
                // abrupt failure during data send - don't retry, because its status is now unknown
                infoValue = CMD_RESP_I2C_INFO_VAL_DATANACK;
                slot.fail(CMD_FAIL, "DataNack");
            } else if (status == BusJammed || status == MemoryError || status == OtherError) {
                // these are probably fatal, and should not be retried.
                infoValue = CMD_RESP_I2C_INFO_VAL_OTHER;
                slot.fail(CMD_ERROR, "Fatal I2C error");
            } else if (status == Address2Nack && action == ActionType::SEND_RECEIVE) {
                //TODO: This could be retried maybe, but it would need specific code, as otherwise it would redo the transmit...
                infoValue = CMD_RESP_I2C_INFO_VAL_ADDRNACK;
                slot.fail(CMD_FAIL, "Address2Nack");
            } else if (storedI2cData->attemptsLeft > 0) {
                // any other error, keep retrying. Beyond here, we must be at the end of all retries.
                slot.unsetComplete();
                terminating = false;
            } else if (status == AddressNack) {
                infoValue = CMD_RESP_I2C_INFO_VAL_ADDRNACK;
                slot.fail(CMD_FAIL, "AddressNack");
            } else {
                infoValue = CMD_RESP_I2C_INFO_VAL_OTHER;
                slot.fail(CMD_FAIL, "I2C failure");
            }

            if (terminating) {
                out->writeField8(CMD_RESP_I2C_ATTEMPTS_T, storedI2cData->initialRetries - storedI2cData->attemptsLeft);
                out->writeField8(CMD_RESP_I2C_INFO_I, infoValue);
                out->writeField16(CMD_RESP_I2C_ADDR_A, address);

                ((I2c<LL>*) *i2cStoredPointer)->unlock();
                return;
            }
        }

        I2c<LL> *i2cPort = I2cManager<LL>::getI2cById(storedI2cData->port);
        *i2cStoredPointer = (uint8_t*) i2cPort;

        storedI2cData->attemptsLeft--;

        const ZcodeBigField<ZP> *bigField = slot.getBigField();
        PeripheralOperationMode mode = slot.isParallel() ? PeripheralOperationMode::DMA : PeripheralOperationMode::SYNCHRONOUS;

        switch ((ActionType) action) {
        case SEND:
            i2cPort->asyncTransmit10(mode, address, tenBit, bigField->getData(), bigField->getLength(), asyncCallback);
            break;
        case RECEIVE:
            i2cPort->asyncReceive10(mode, address, tenBit, out->getReadBuffer(), requestedLength, asyncCallback);
            break;
        case SEND_RECEIVE:
            i2cPort->asyncTransmitReceive10(mode, address, tenBit, bigField->getData(), bigField->getLength(), out->getReadBuffer(), requestedLength, asyncCallback);
            break;
        }
    }

    static void asyncCallback(I2c<LL> *i2c, I2cTerminationStatus status) {
        static_assert(sizeof(I2cDataInPlace) == sizeof(uint32_t), "StoredI2cData wouldn't fit into uint32_t");
        for (uint8_t i = 0, n = Zcode<ZP>::zcode.getChannelCount(); i < n; i++) {
            if (Zcode<ZP>::zcode.getChannel(i)->runner.dataPointer == (uint8_t*) i2c) {

                ZcodeRunningCommandSlot<ZP> *slot = &Zcode<ZP>::zcode.getChannel(i)->runner;
                ((volatile I2cDataInPlace*) &slot->storedData)->data.status = (uint8_t) status;
                slot->setNeedsAction();
                return;

            }
        }
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_COMMANDS_ZCODEGENERALI2CACTION_HPP_ */
