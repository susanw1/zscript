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
class RCodeInStream;
class RCodeCommandSequence;

class RCodeParser;

class RCodeCommandSlot {
private:
    RCode *rcode = NULL;
    RCodeStandardBigField big;
    RCodeBigBigField *bigBig = NULL;
    RCodeFieldMap map;
    char end = 0;
    RCodeResponseStatus status = OK;
    char const *errorMessage = "";
    bool parsed = false;
    bool complete = false;
    bool started = false;
    bool usesBigBig = false;
    RCodeCommand *cmd = NULL;

    void eatWhitespace(RCodeInStream *in);

    uint8_t getHex(char c) {
        return (uint8_t) (c >= 'a' ? c - 'a' + 10 : c - '0');
    }

    bool isHex(char c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    }

    bool parseHexField(RCodeInStream *in, char field);

    void setup(RCode *rcode, RCodeBigBigField *bigBig) {
        this->rcode = rcode;
        this->bigBig = bigBig;
    }
    friend void RCodeParserSetupSlots(RCodeParser *parser);
public:
    RCodeCommandSlot *next = NULL;

    void reset() {
        next = NULL;
        map.reset();
        big.reset();
        status = OK;
        parsed = false;
        started = false;
        complete = false;
        if (usesBigBig) {
            bigBig->reset();
            usesBigBig = false;
        }
        cmd = NULL;
    }
    void start() {
        started = true;
    }
    bool isStarted() const {
        return started;
    }
    void terminate() {
        end = '\n';
    }
    bool isComplete() const {
        return complete;
    }
    void setComplete(bool complete) {
        this->complete = complete;
    }
    RCodeFieldMap* getFields() {
        return &map;
    }
    RCodeBigField* getBigField() {
        if (usesBigBig) {
            return bigBig;
        }
        return &big;
    }
    bool isParsed() const {
        return parsed;
    }

    char getEnd() const {
        return end;
    }

    RCodeResponseStatus getStatus() const {
        return status;
    }

    char const* getErrorMessage() const {
        return errorMessage;
    }

    RCodeCommand* getCommand();

    bool parseSingleCommand(RCodeInStream *in, RCodeCommandSequence *sequence);
};

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDSLOT_HPP_ */
