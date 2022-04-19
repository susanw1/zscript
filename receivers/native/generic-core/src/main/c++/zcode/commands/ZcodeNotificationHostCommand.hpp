/*
 * ZcodeNotificationHostCommand.hpp
 *
 *  Created on: 30 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODENOTIFICATIONHOSTCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODENOTIFICATIONHOSTCOMMAND_HPP_

#include "../ZcodeIncludes.hpp"
#include "ZcodeCommand.hpp"

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeDebugOutput;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeNotificationHostCommand: public ZcodeCommand<ZP> {
    private:
        const uint8_t code = 0x08;
        Zcode<ZP> *const zcode;

    public:
        ZcodeNotificationHostCommand(Zcode<ZP> *const zcode) :
                zcode(zcode) {
        }

        void execute(ZcodeCommandSlot<ZP> *slot, ZcodeCommandSequence<ZP> *sequence, ZcodeOutStream<ZP> *out);

        void setLocks(ZcodeCommandSlot<ZP> *slot, ZcodeLockSet<ZP> *locks) const {
        }

        uint8_t getCode() const {
            return code;
        }

        bool matchesCode(uint8_t code[], uint8_t length) const {
            return length == 1 && code[0] == ZcodeNotificationHostCommand::code;
        }

        uint8_t getCodeLength() const {
            return 1;
        }

        uint8_t const* getFullCode() const {
            return &code;
        }
};

template<class ZP>
void ZcodeNotificationHostCommand<ZP>::execute(ZcodeCommandSlot<ZP> *slot, ZcodeCommandSequence<ZP> *sequence, ZcodeOutStream<ZP> *out) {
    zcode->getNotificationManager()->setNotificationChannel(sequence->getChannel());
    out->writeStatus(OK);
    slot->setComplete(true);
}

#include "../Zcode.hpp"
#include "../ZcodeOutStream.hpp"
#include "../ZcodeNotificationManager.hpp"
#include "../parsing/ZcodeCommandSlot.hpp"
#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODENOTIFICATIONHOSTCOMMAND_HPP_ */
