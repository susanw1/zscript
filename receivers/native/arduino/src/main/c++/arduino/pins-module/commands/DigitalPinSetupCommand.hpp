/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_DIGITALPINSETUPCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_DIGITALPINSETUPCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include "../PinManager.hpp"

#define COMMAND_EXISTS_0041 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace pins_module {

template<class ZP>
class DigitalPinSetupCommand : public DigitalSetup_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t pin;
        if (!ctx.getReqdFieldCheckLimit(ReqPin__P, ZP::pinCount, &pin)) {
            return;
        }
        uint16_t mode;
        if (ctx.getField(ReqMode__M, &mode)) {
            switch (mode) {
                case ReqMode_Values::input_Value:
                    PinManager<ZP>::setState(pin, false, false, false);
                    pinMode(pin, INPUT);
                    break;
                case ReqMode_Values::inputPullUp_Value:
                    PinManager<ZP>::setState(pin, false, true, false);
                    pinMode(pin, INPUT_PULLUP);
                    break;
                case ReqMode_Values::output_Value:
                    PinManager<ZP>::setState(pin, false, false, false);
                    pinMode(pin, OUTPUT);
                    break;
                case ReqMode_Values::openDrain_Value:
                    PinManager<ZP>::setState(pin, true, false, false);
                    pinMode(pin, INPUT);
                    digitalWrite(pin, LOW);
                    break;
                case ReqMode_Values::openDrainPullUp_Value:
                    PinManager<ZP>::setState(pin, true, true, false);
                    pinMode(pin, INPUT_PULLUP);
                    digitalWrite(pin, LOW);
                    break;
                case ReqMode_Values::openCollector_Value:
                    PinManager<ZP>::setState(pin, true, false, true);
                    pinMode(pin, INPUT);
                    digitalWrite(pin, HIGH);
                    break;
                case ReqMode_Values::openCollectorPullUp_Value:
                    PinManager<ZP>::setState(pin, true, true, true);
                    pinMode(pin, INPUT_PULLUP);
                    digitalWrite(pin, HIGH);
                    break;
                default:
                    ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
                    return;
            }
        }

#ifdef ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS
        uint16_t notifMode;
        if (ctx.getField(ReqNotificationMode__N, &notifMode)) {
            if (!PinManager<ZP>::supportsInterruptNotifications(pin)) {
                if (notifMode != ReqNotificationMode_Values::none_Value
                        && notifMode != ReqNotificationMode_Values::onHigh_Value
                        && notifMode != ReqNotificationMode_Values::onLow_Value) {
                    ctx.status(ResponseStatus::VALUE_UNSUPPORTED);
                    return;
                }
            }
            switch (notifMode) {
                case ReqNotificationMode_Values::none_Value:
                    PinManager<ZP>::setNotificationType(pin, PinNotificationType::NONE);
                    break;
                case ReqNotificationMode_Values::onHigh_Value:
                    PinManager<ZP>::setNotificationType(pin, PinNotificationType::ON_HIGH);
                    break;
                case ReqNotificationMode_Values::onLow_Value:
                    PinManager<ZP>::setNotificationType(pin, PinNotificationType::ON_LOW);
                    break;
                case ReqNotificationMode_Values::onRising_Value:
                    PinManager<ZP>::setNotificationType(pin, PinNotificationType::ON_RISING);
                    break;
                case ReqNotificationMode_Values::onFalling_Value:
                    PinManager<ZP>::setNotificationType(pin, PinNotificationType::ON_FALLING);
                    break;
                case ReqNotificationMode_Values::onChange_Value:
                    PinManager<ZP>::setNotificationType(pin, PinNotificationType::ON_CHANGE);
                    break;
                default:
                    ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
                    return;
            }
        }
#endif
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespSupportedModes__M, RespSupportedModes_Values::input_field
                                              | RespSupportedModes_Values::inputPullUp_field
                                              | RespSupportedModes_Values::output_field
                                              | RespSupportedModes_Values::openDrain_field
                                              | RespSupportedModes_Values::openDrainPullUp_field
                                              | RespSupportedModes_Values::openCollector_field
                                              | RespSupportedModes_Values::openCollectorPullUp_field);

#ifdef ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS
        if (PinManager<ZP>::supportsInterruptNotifications(pin)) {
            out.writeField(RespSupportedNotifications__N,
                           RespSupportedNotifications_Values::onHigh_field
                           | RespSupportedNotifications_Values::onLow_field
                           | RespSupportedNotifications_Values::onRising_field
                           | RespSupportedNotifications_Values::onFalling_field
                           | RespSupportedNotifications_Values::onChange_field);
        } else {
            out.writeField(RespSupportedNotifications__N,
                           RespSupportedNotifications_Values::onHigh_field
                           | RespSupportedNotifications_Values::onLow_field);
        }
#endif
    }
};

}

}

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_DIGITALPINSETUPCOMMAND_HPP_ */
