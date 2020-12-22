/*
 * DmaManager.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_LOWLEVEL_DMALOWLEVEL_DMAMANAGER_HPP_
#define SRC_TEST_CPP_COMMANDS_LOWLEVEL_DMALOWLEVEL_DMAMANAGER_HPP_
#include "../../LowLevel/GeneralHalSetup.hpp"
#include "../DmaLowLevel/Dma.hpp"

class DmaManager {
private:
    friend DmaInterruptManager;
    static Dma dmas[GeneralHalSetup::dmaCount];
    DmaManager() {
    }
public:
    static void init();

    static Dma* getDmaById(DmaIdentifier id) {
        return dmas + id;
    }
    static Dma* getFreeDma() {
        for (int i = 0; i < GeneralHalSetup::dmaCount; ++i) {
            if (!dmas[i].isLocked()) {
                return dmas + i;
            }
        }
        return NULL;
    }
};

#endif /* SRC_TEST_CPP_COMMANDS_LOWLEVEL_DMALOWLEVEL_DMAMANAGER_HPP_ */
