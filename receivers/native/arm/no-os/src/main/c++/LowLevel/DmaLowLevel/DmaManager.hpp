/*
 * DmaManager.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_LOWLEVEL_DMALOWLEVEL_DMAMANAGER_HPP_
#define SRC_TEST_CPP_COMMANDS_LOWLEVEL_DMALOWLEVEL_DMAMANAGER_HPP_

#include <llIncludes.hpp>
#include "Dma.hpp"

template<class LL>
class DmaManager {
private:
    static Dma<LL> dmas[LL::dmaCount];

    DmaManager() {
    }
    static void interrupt(uint8_t);

public:
    static void init();

    static Dma<LL>* getDmaById(DmaIdentifier id) {
        return dmas + id;
    }

    static Dma<LL>* getFreeDma() {
        for (int i = 0; i < LL::dmaCount; ++i) {
            if (!dmas[i].isLocked()) {
                return dmas + i;
            }
        }
        return NULL;
    }
};
#include "specific/DmaManagercpp.hpp" // these at the bottom are there to clean up include order issues.

#endif /* SRC_TEST_CPP_COMMANDS_LOWLEVEL_DMALOWLEVEL_DMAMANAGER_HPP_ */
