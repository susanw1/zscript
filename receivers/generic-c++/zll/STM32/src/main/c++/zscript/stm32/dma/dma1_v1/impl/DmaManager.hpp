/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */


template<class LL>
class DmaManager {
private:
    static Dma<LL> dmas[DMA_COUNT];

    DmaManager() {
    }
    static void interrupt(uint8_t);

public:
    static void init();

    static Dma<LL>* getDmaById(DmaIdentifier id) {
        return dmas + id;
    }
    static Dma<LL>* getDmaForRequest(DmaRequest request) {

    }
};
