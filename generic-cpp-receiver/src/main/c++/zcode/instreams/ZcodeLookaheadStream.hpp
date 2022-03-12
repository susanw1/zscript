/*
 * ZcodeLookaheadStream.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_PARSING_ZCODELOOKAHEADSTREAM_HPP_
#define SRC_TEST_CPP_ZCODE_PARSING_ZCODELOOKAHEADSTREAM_HPP_
#include "../ZcodeIncludes.hpp"

template<class RP>
class ZcodeLookaheadStream {
public:
    virtual char read() = 0;

    virtual ~ZcodeLookaheadStream() {
    }
};


#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODELOOKAHEADSTREAM_HPP_ */
