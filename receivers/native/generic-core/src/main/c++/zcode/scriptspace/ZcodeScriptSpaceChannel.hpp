/*
 * ZcodeExecutionSpaceChannel.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACECHANNEL_HPP_
#define SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACECHANNEL_HPP_

#include "../channels/ZcodeCommandChannel.hpp"
#include "../scriptspace/ZcodeScriptSpaceChannelIn.hpp"
#include "../scriptspace/ZcodeScriptSpaceOut.hpp"

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeScriptSpace;

template<class ZP>
class ZcodeScriptSpaceChannel: public ZcodeCommandChannel<ZP> {
private:
    typedef typename ZP::scriptSpaceAddress_t scriptSpaceAddress_t;
    typedef typename ZP::Strings::string_t string_t;

    ZcodeScriptSpace<ZP> *space;
    ZcodeScriptSpaceOut<ZP> outA;
    ZcodeScriptSpaceChannelIn<ZP> inA;

public:

    ZcodeScriptSpaceChannel() :
            ZcodeCommandChannel<ZP>(&inA, &outA, false), space(Zcode<ZP>::zcode.getSpace()), outA(space), inA(this) {
    }

    ZcodeScriptSpaceOut<ZP>* getOutStream() {
        return &outA;
    }

    void move(scriptSpaceAddress_t position) {
        inA.setPosition(position);
    }

    void giveInfo(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeField16('B', ZP::scriptBigSize);
        out->writeField16('F', ZP::fieldNum);
        out->writeField8('M', 2);

        out->writeField8('D', space->getDelay());
        out->writeField8('G', (uint8_t) space->isRunning());
        out->writeField16('L', space->getLength());
        out->writeBigStringField(ZP::Strings::scriptSpaceChannelDescriptor);
        out->writeStatus(OK);
    }

    void readSetup(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint16_t address = 0;
        uint16_t delay = 0;

        if (slot.getFields()->get('A', &address)) {
            if (address >= space->getLength()) {
                slot.fail(BAD_PARAM, (string_t) ZP::Strings::failScriptBadAddress);
                space->setFailed(true);
                return;
            } else {
                move(address);
            }
        }
        if (slot.getFields()->get('D', &delay)) {
            if (delay > 0xFF) {
                slot.fail(BAD_PARAM, (string_t) ZP::Strings::failScriptBadDelay);
                space->setFailed(true);
                return;
            } else {
                space->setDelay((uint8_t) delay);
            }
        }
        if (slot.getFields()->has('G')) {
            space->setRunning(true);
        } else {
            space->setRunning(false);
        }
        space->setFailed(false);
        out->writeStatus(OK);
    }
    bool reset() {
        space->setRunning(false);
        space->setFailed(false);
        outA.reset();
        return true;
    }

};

#endif /* SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACECHANNEL_HPP_ */
