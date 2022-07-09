/*
 * ZcodeExecutionCommandSlot.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_PARSING_ZCODEEXECUTIONCOMMANDSLOT_HPP_
#define SRC_TEST_CPP_ZCODE_PARSING_ZCODEEXECUTIONCOMMANDSLOT_HPP_
#include "../ZcodeIncludes.hpp"
#include "../parsing/ZcodeBigField.hpp"
#include "../parsing/ZcodeFieldMap.hpp"

template<class ZP>
class ZcodeRunningCommandSlot;

template<class ZP>
class ZcodeCommandChannel;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeExecutionCommandSlot {
    typedef typename ZP::Strings::string_t string_t;
    ZcodeRunningCommandSlot<ZP> *slot;

public:
    ZcodeExecutionCommandSlot(ZcodeRunningCommandSlot<ZP> *slot) :
            slot(slot) {
    }
    ZcodeCommandChannel<ZP>* getChannel() {
        return slot->commandSlot->channel;
    }
    uint32_t* getStoredData() {
        return &slot->storedData;
    }

    uint8_t** getStoredPointerData() {
        return &slot->dataPointer;
    }

    Zcode<ZP>* getZcode() {
        return slot->getZcode();
    }

    const ZcodeFieldMap<ZP>* getFields() {
        return slot->getFields();
    }

    const ZcodeBigField<ZP>* getBigField() {
        return slot->getBig();
    }

    void fail(ZcodeResponseStatus failStatus, string_t message) {
        slot->fail(failStatus, message);
    }
    void mildFail(ZcodeResponseStatus failStatus) {
        slot->mildFail(failStatus);
    }

    ZcodeOutStream<ZP>* getOut() {
        return slot->out;
    }

    void setComplete() {
        slot->setComplete();
    }

    void unsetComplete() {
        slot->unsetComplete();
    }
    void setNeedsAction() {
        slot->setNeedsAction();
    }
};
#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODEEXECUTIONCOMMANDSLOT_HPP_ */
