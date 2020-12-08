/*
 * RCodeCommandSlot.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDSLOT_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDSLOT_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeParameters.hpp"
#include "RCodeBigField.hpp"
#include "RCodeFieldMap.hpp"

class RCode;
class RCodeCommand;
class RCodeCommandInStream;
class RCodeCommandSequence;

class RCodeParser;
struct RCodeCommandSlotStatus {
    bool parsed :1;
    bool complete :1;
    bool started :1;
    bool usesBigBig :1;
    bool hasCheckedCommand :1;
};
class RCodeCommandSlot {
public:
    RCodeCommandSlot *next = NULL;
private:
    RCodeBigBigField *bigBig = NULL;
    RCodeCommand *cmd = NULL;
    char const *errorMessage = "";
    RCodeStandardBigField big;
    RCodeFieldMap map;
    RCodeResponseStatus status = OK;
    char end = 0;
    RCodeCommandSlotStatus slotStatus;

    uint8_t getHex(char c) {
        return (uint8_t) (c >= 'a' ? c - 'a' + 10 : c - '0');
    }

    bool isHex(char c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    }

    bool parseHexField(RCodeCommandInStream *in, char field);

    void setup(RCodeBigBigField *bigBig) {
        this->bigBig = bigBig;
    }
    friend void RCodeParserSetupSlots(RCodeParser *parser);

    void failParse(RCodeCommandInStream *in, RCodeCommandSequence *sequence,
            RCodeResponseStatus errorStatus, char const *errorMessage);

public:
    RCodeCommandSlot() {
        slotStatus.parsed = false;
        slotStatus.started = false;
        slotStatus.complete = false;
        slotStatus.hasCheckedCommand = false;
        slotStatus.usesBigBig = false;
    }
    void reset() {
        next = NULL;
        map.reset();
        big.reset();
        status = OK;
        slotStatus.parsed = false;
        slotStatus.started = false;
        slotStatus.complete = false;
        slotStatus.hasCheckedCommand = false;
        if (RCodeParameters::bigBigFieldLength > 0 && slotStatus.usesBigBig) {
            bigBig->reset();
        }
        slotStatus.usesBigBig = false;
        cmd = NULL;
    }
    void start() {
        slotStatus.started = true;
    }
    bool isStarted() const {
        return slotStatus.started;
    }
    void terminate() {
        end = '\n';
    }
    bool isComplete() const {
        return slotStatus.complete;
    }
    void setComplete(bool complete) {
        this->slotStatus.complete = complete;
    }
    RCodeFieldMap* getFields() {
        return &map;
    }
    RCodeBigField* getBigField() {
        if (RCodeParameters::bigBigFieldLength > 0 && slotStatus.usesBigBig) {
            return bigBig;
        }
        return &big;
    }
    bool isParsed() const {
        return slotStatus.parsed;
    }

    char getEnd() const {
        return end;
    }
    bool isEndOfSequence() const {
        return end == '\n';
    }

    RCodeResponseStatus getStatus() const {
        return status;
    }

    char const* getErrorMessage() const {
        return errorMessage;
    }

    RCodeCommand* getCommand(RCode *rcode);

    bool parseSingleCommand(RCodeCommandInStream *in,
            RCodeCommandSequence *sequence);

    void fail(const char *errorMessage, RCodeResponseStatus status) {
        this->errorMessage = errorMessage;
        this->status = status;
        this->end = '\n';
    }
};

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDSLOT_HPP_ */
