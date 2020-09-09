/*
 * RCodeParser.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODEPARSER_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODEPARSER_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeParameters.hpp"
#include "RCodeBigField.hpp"
#include "RCodeCommandSlot.hpp"

class RCode;
class RCodeCommandSequence;

class RCodeParser {
private:
    RCode *const rcode;
    RCodeCommandSequence *mostRecent = NULL;
    RCodeCommandSlot slots[RCodeParameters::slotNum];
    RCodeBigBigField bigBig = RCodeBigBigField();
    friend void RCodeParserSetupSlots(RCodeParser *parser);
public:

    RCodeParser(RCode *rcode) :
            rcode(rcode) {
        RCodeParserSetupSlots(this);
    }

    void parseNext();

    void parse(RCodeCommandSlot *slot, RCodeCommandSequence *sequence);
    void setupSlots();
};

void RCodeParserSetupSlots(RCodeParser *parser);

#include "../RCode.hpp"
#include "RCodeCommandChannel.hpp"
#include "RCodeCommandSequence.hpp"

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODEPARSER_HPP_ */
