/*
 * DmaManager.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_LOWLEVEL_DMALOWLEVEL_DMAMANAGER_HPP_
#define SRC_TEST_CPP_COMMANDS_LOWLEVEL_DMALOWLEVEL_DMAMANAGER_HPP_

#include <arm-no-os/llIncludes.hpp>
#include "Dma.hpp"

template<class LL>
class DmaManager {
private:
    typedef typename LL::HW HW;
    static Dma<LL> dmas[LL::HW::dmaCount];

    DmaManager() {
    }
    static void interrupt(uint8_t);

public:
    static void init();

    static Dma<LL>* getDmaById(DmaIdentifier id) {
        return dmas + id;
    }
};
#include "specific/DmaManagercpp.hpp" // these at the bottom are there to clean up include order issues.

#endif /* SRC_TEST_CPP_COMMANDS_LOWLEVEL_DMALOWLEVEL_DMAMANAGER_HPP_ */
