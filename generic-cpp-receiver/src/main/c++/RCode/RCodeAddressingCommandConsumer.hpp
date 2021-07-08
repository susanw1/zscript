/*
 * RCodeAddressingCommandConsumer.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_ADDRESSINGCOMMANDCONSUMER_HPP_
#define SRC_TEST_CPP_RCODE_ADDRESSINGCOMMANDCONSUMER_HPP_
#include "RCodeIncludes.hpp"
#include "instreams/RCodeSequenceInStream.hpp"

template<class RP>
class RCodeAddressingCommandConsumer {
public:
    virtual void consumeAddressedCommandSequence(RCodeSequenceInStream<RP> *in) = 0;
};

#endif /* SRC_TEST_CPP_RCODE_ADDRESSINGCOMMANDCONSUMER_HPP_ */
