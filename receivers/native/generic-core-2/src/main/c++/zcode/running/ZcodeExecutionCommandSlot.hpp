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
    void fail(ZcodeResponseStatus failStatus, const char *message) {
        slot->fail(failStatus, message);
    }
    /**
     * Much like fail, but allows the output stream to continue functioning - useful if you want to fail in an intelligent way. Doesn't write the failure status.
     */
    void mildFail(ZcodeResponseStatus failStatus) {
        slot->mildFail(failStatus);
    }

    ZcodeOutStream<ZP>* getOut() {
        return slot->out;
    }
    /**
     * Marks current command as being complete, so slot will move on to next command. Note: this is normally preset by default, so not normally needed.
     */
    void setComplete() {
        slot->setComplete();
    }

    /**
     * Marks current command as NOT being complete yet (eg has fired off an async activity). Will be executed again after setNeedsAction is called.
     */
    void unsetComplete() {
        slot->unsetComplete();
    }

    /**
     * Marks current command as having work available to do, so slot will call execute to progress it. This should be called by callback functions.
     */
    void setNeedsAction() {
        slot->setNeedsAction();
    }
};
#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODEEXECUTIONCOMMANDSLOT_HPP_ */
