/*
 * ZcodeAddressingCommandConsumer.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ADDRESSINGCOMMANDCONSUMER_HPP_
#define SRC_TEST_CPP_ZCODE_ADDRESSINGCOMMANDCONSUMER_HPP_
#include "ZcodeIncludes.hpp"
#include "instreams/ZcodeSequenceInStream.hpp"

template<class RP>
class ZcodeAddressingCommandConsumer {
public:
    virtual void consumeAddressedCommandSequence(ZcodeSequenceInStream<RP> *in) = 0;
};

#endif /* SRC_TEST_CPP_ZCODE_ADDRESSINGCOMMANDCONSUMER_HPP_ */
