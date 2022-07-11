#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEI2CSENDCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEI2CSENDCOMMAND_HPP_

#include <modules/ZcodeCommand.hpp>

template<class ZP>
class ZcodeI2cSendCommand: public ZcodeCommand<ZP> {
private:

public:
    static constexpr uint8_t CODE = 0x02;

    static constexpr char CMD_PARAM_I2C_PORT_P = 'P';
    static constexpr char CMD_PARAM_I2C_ADDR_A = 'A';
    static constexpr char CMD_PARAM_TENBIT_ADDR_N = 'N';
    static constexpr char CMD_PARAM_RETRIES_T = 'T';

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
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

        uint8_t retries = getFields()->get(CMD_PARAM_RETRIES_T, 5);
        if (retries == 0) {
            retries = 1;
        }

        uint16_t port;
        if (!slot.getFields()->get(CMD_PARAM_I2C_PORT_P, &port)) {
            slot.fail(BAD_PARAM, "Missing I2C port");
            return;
        } else if (port >= ZP::LL::i2cCount) {
            slot.fail(BAD_PARAM, "Invalid I2C port");
            return;
        }

        I2c<LL> *i2cPort = I2cManager<ZP::LL>::getI2cById(port);

        if (slot.isParallel()) {
            i2cPort->asyncTransmit10(PeripheralOperationMode::DMA, address, tenBit, slot.getBigField()->getData(), slot.getBigField()->getLength(), asyncCallback);
        } else {
            i2cPort->transmit10(address, tenBit, slot.getBigField()->getData(), slot.getBigField()->getLength());
        }

        slot.getOut()->writeStatus(OK);
    }

    static void asyncCallback(I2c *i2c, I2cTerminationStatus status) {

    }
};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEI2CSENDCOMMAND_HPP_ */
