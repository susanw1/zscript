/*
 * RCodeSequenceInStream.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODESEQUENCEINSTREAM_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODESEQUENCEINSTREAM_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeParameters.hpp"
#include "../instreams/RCodeLookaheadStream.hpp"
bbbbbbbbbbbbbbbbbbb
class OldRCodeSequenceInStream {
public:
    virtual char next() = 0;

    virtual bool hasNext() = 0;

    virtual RCodeLookaheadStream* getLookahead() = 0;

    virtual void openCommandSequence() = 0;

    virtual void closeCommandSequence() = 0;

    virtual bool isCommandSequenceOpen() const = 0;
    virtual ~OldRCodeSequenceInStream() {

    }
};
#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODESEQUENCEINSTREAM_HPP_ */
