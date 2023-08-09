/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_TOKENBUFFERFLAGS_HPP_
#define SRC_MAIN_C___ZSCRIPT_TOKENBUFFERFLAGS_HPP_
#include "../ZscriptIncludes.hpp"

namespace Zscript {
namespace GenericCore {

template<class ZP>
class TokenBufferFlags {
    volatile bool markerWritten = false;
    volatile bool seqMarkerWritten = false;
    volatile bool readerBlocked = true;

public:
    void setMarkerWritten() {
        this->markerWritten = true;
    }

    bool getAndClearMarkerWritten() {
        if (markerWritten) {
            markerWritten = false;
            return true;
        }
        return false;
    }

    void setSeqMarkerWritten() {
        this->seqMarkerWritten = true;
    }

    bool getAndClearSeqMarkerWritten() {
        if (seqMarkerWritten) {
            seqMarkerWritten = false;
            return true;
        }
        return false;
    }

    bool isReaderBlocked() {
        return readerBlocked;
    }

    void clearReaderBlocked() {
        this->readerBlocked = false;
    }

    void setReaderBlocked() {
        this->readerBlocked = true;
    }

};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_TOKENBUFFERFLAGS_HPP_ */
