/*
 * ZcodeI2cBusInterruptSource.hpp
 *
 *  Created on: 24 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ARM_NO_OS_I2C_MODULE_ADDRESSING_ZCODEI2CBUSINTERRUPTSOURCE_HPP_
#define SRC_MAIN_C___ARM_NO_OS_I2C_MODULE_ADDRESSING_ZCODEI2CBUSINTERRUPTSOURCE_HPP_

#include <zcode/ZcodeIncludes.hpp>
#include <zcode/ZcodeBusInterruptSource.hpp>
#include <arm-no-os/pins-module/lowlevel/Gpio.hpp>
#include <arm-no-os/i2c-module/lowlevel/I2cManager.hpp>

template<class ZP>
class ZcodeI2cBusInterruptHandler {
    typedef typename ZP::LL LL;
    typedef typename LL::HW HW;
    struct HandlerStatus {
        bool notificationFlag :1;
        I2cIdentifier id :7;
    };
    GpioPin<LL> interruptPin;
    HandlerStatus status;

public:
    ZcodeI2cBusInterruptHandler(GpioPin<LL> interruptPin, I2cIdentifier id) :
            interruptPin(interruptPin), status( { false, id }) {
        interruptPin.init();
        interruptPin.setOutputMode(OpenDrain);
        interruptPin.setOutputSpeed(HighSpeed);
        interruptPin.setPullMode(PullUp);
        interruptPin.setMode(Input);
    }
    bool hasNotification() {
        if (status.notificationFlag) {
            return false;
        }
        return !interruptPin.read();
    }
    void setNotificationFlag() {
        status.notificationFlag = true;
    }
    void clearNotificationFlag() {
        status.notificationFlag = false;
    }
    ZcodeNotificationAddressInfo getAddress() {
        ZcodeNotificationAddressInfo info;
        info.valid = true;
        info.isAddressed = true;
        info.hasFound = false;
        I2c<LL> *i2c = I2cManager<LL>::getI2cById(status.id);
        if (!i2c->lock()) {
            return info;
        }
        uint8_t addrRead[2];
        I2cTerminationStatus status = i2c->receive(0x0C, addrRead, 2);
        i2c->unlock();
        if (status != Complete) {
            return info;
        }
        info.hasFound = true;
        if ((addrRead[0] & 0xF9) == 0xF0) {
            info.address = addrRead[1] | ((addrRead[0] & 0x06) << 7);
        } else {
            info.address = addrRead[0] >> 1;
        }
        return info;
    }
};

template<class ZP>
class ZcodeI2cBusInterruptSource: public ZcodeBusInterruptSource<ZP> {
    ZcodeI2cBusInterruptHandler<ZP> handlers[ZP::LL::HW::i2cCount];

public:

    ZcodeI2cBusInterruptSource() :
            handlers { ZcodeI2cBusInterruptHandler<ZP>(I2C_1_ALERT, 0), ZcodeI2cBusInterruptHandler<ZP>(I2C_2_ALERT, 1),
                    ZcodeI2cBusInterruptHandler<ZP>(I2C_3_ALERT, 2), ZcodeI2cBusInterruptHandler<ZP>(I2C_4_ALERT, 3) } {
    }

    ZcodeNotificationInfo takeUncheckedNotification() {
        for (I2cIdentifier i = 0; i < ZP::LL::HW::i2cCount; ++i) {
            if (handlers[i].hasNotification()) {
                handlers[i].setNotificationFlag();
                ZcodeNotificationInfo info;
                info.id = i;
                info.port = i;
                info.module = 0x5;
                info.valid = true;
                return info;
            }
        }
        ZcodeNotificationInfo invalid;
        invalid.valid = false;
        return invalid;
    }

    void clearNotification(uint8_t id) {
        handlers[id].clearNotificationFlag();
    }

    ZcodeNotificationAddressInfo getAddressInfo(uint8_t id) {
        return handlers[id].getAddress();
    }

};

#endif /* SRC_MAIN_C___ARM_NO_OS_I2C_MODULE_ADDRESSING_ZCODEI2CBUSINTERRUPTSOURCE_HPP_ */
