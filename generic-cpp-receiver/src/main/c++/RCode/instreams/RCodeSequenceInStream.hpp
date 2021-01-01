/*
 * RCodeSequenceInStream.hpp
 *
 *  Created on: 8 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_RCODE_INSTREAMS_RCODESEQUENCEINSTREAM_HPP_
#define SRC_MAIN_CPP_RCODE_INSTREAMS_RCODESEQUENCEINSTREAM_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "RCodeCommandInStream.hpp"
#include "RCodeMarkerInStream.hpp"

class RCodeChannelInStream;

class RCodeSequenceInStream {
private:
    RCodeChannelInStream *channelIn;
    RCodeCommandInStream commandIn = RCodeCommandInStream(this);
    RCodeMarkerInStream markerIn = RCodeMarkerInStream(this);
    char current = 0;
    bool opened = false;
    bool locked = false;
public:
    RCodeSequenceInStream(RCodeChannelInStream *channelIn) :
            channelIn(channelIn) {
    }

    void open() {
        if (!opened) {
            commandIn.reset();
            opened = true;
            readInternal();
        }
    }

    void close() {
        commandIn.reset();
        if (opened) {
            while (current != '\n') {
                readInternal();
            }
            opened = false;
        }
    }
    char read() {
        if (opened) {
            char prev = current;
            readInternal();
            if (opened && current == '\n') {
                opened = false;
            }
            return prev;
        } else {
            return '\n';
        }
    }

    char peek() {
        if (opened) {
            return current;
        } else {
            return '\n';
        }
    }

    bool hasNext() {
        return opened;
    }

    void readInternal();

    void skipToError() {
        commandIn.close();
        while (commandIn.read() != '|' && opened) {
            commandIn.open();
            commandIn.close();
        }
    }

    RCodeChannelInStream* getChannelInStream() {
        return channelIn;
    }

    RCodeCommandInStream* getCommandInStream() {
        return &commandIn;
    }

    RCodeMarkerInStream* getMarkerInStream() {
        markerIn.reset();
        return &markerIn;
    }

    void unlock() {
        locked = false;
    }

    bool lock() {
        if (locked) {
            return false;
        } else {
            locked = true;
            return true;
        }
    }

    bool isLocked() {
        return locked;
    }
};

#include "RCodeChannelInStream.hpp"

#endif /* SRC_MAIN_CPP_RCODE_INSTREAMS_RCODESEQUENCEINSTREAM_HPP_ */
