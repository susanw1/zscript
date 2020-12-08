/*
 * RCodeLookaheadStream.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODELOOKAHEADSTREAM_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODELOOKAHEADSTREAM_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeParameters.hpp"

class RCodeLookaheadStream {
public:
    virtual char read() = 0;
    virtual ~RCodeLookaheadStream() {
    }
};

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODELOOKAHEADSTREAM_HPP_ */
