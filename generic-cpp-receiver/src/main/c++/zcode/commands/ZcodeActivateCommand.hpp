/*
 * ZcodeActivateCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEACTIVATECOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEACTIVATECOMMAND_HPP_

#include "../ZcodeIncludes.hpp"
#include "ZcodeCommand.hpp"

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeActivateCommand: public ZcodeCommand<ZP> {
    private:
        static bool activated;
        const uint8_t code = 0x03;

    public:
        static const int MAX_SYSTEM_CODE = 15;

        static bool isActivated() {
            return activated;
        }

        static void reset() {
            activated = false;
        }

        void execute(ZcodeCommandSlot<ZP> *slot, ZcodeCommandSequence<ZP> *sequence, ZcodeOutStream<ZP> *out) {
            if (!activated) {
                out->writeField('A', (uint8_t) 0);
                out->writeStatus(OK);
            } else {
                out->writeField('A', (uint8_t) 1);
                out->writeStatus(OK);
            }
            activated = true;
            slot->setComplete(true);
        }

        void setLocks(ZcodeCommandSlot<ZP> *slot, ZcodeLockSet<ZP> *locks) const {
        }

        uint8_t getCode() const {
            return code;
        }

        bool matchesCode(uint8_t code[], uint8_t length) const {
            return length == 1 && code[0] == ZcodeActivateCommand::code;
        }

        uint8_t getCodeLength() const {
            return 1;
        }

        uint8_t const* getFullCode() const {
            return &code;
        }
};

template<class ZP>
bool ZcodeActivateCommand<ZP>::activated = false;

#include "../ZcodeOutStream.hpp"
#include "../parsing/ZcodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEACTIVATECOMMAND_HPP_ */
