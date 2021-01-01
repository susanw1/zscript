/*
 * RCodeSequenceInStream.hpp
 *
 *  Created on: 8 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_RCODE_INSTREAMS_RCODESEQUENCEINSTREAM_HPP_
#define SRC_MAIN_CPP_RCODE_INSTREAMS_RCODESEQUENCEINSTREAM_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeCommandInStream.hpp"
#include "RCodeMarkerInStream.hpp"

template<class RP>
class RCodeChannelInStream;

template<class RP>
class RCodeSequenceInStream {
private:
    RCodeChannelInStream<RP> *channelIn;
    RCodeCommandInStream<RP> commandIn;
    RCodeMarkerInStream<RP> markerIn;
    char current = 0;
    bool opened = false;
    bool locked = false;
public:
    RCodeSequenceInStream(RCodeChannelInStream<RP> *channelIn) :
            channelIn(channelIn), commandIn(this), markerIn(this) {
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

    RCodeChannelInStream<RP>* getChannelInStream() {
        return channelIn;
    }

    RCodeCommandInStream<RP>* getCommandInStream() {
        return &commandIn;
    }

    RCodeMarkerInStream<RP>* getMarkerInStream() {
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

template<class RP>
void RCodeSequenceInStream<RP>::readInternal() {
    int next = channelIn->read();
    if (next == -1) {
        current = '\n';
        opened = false;
    } else {
        current = (char) next;
    }
}

#include "RCodeChannelInStream.hpp"

#endif /* SRC_MAIN_CPP_RCODE_INSTREAMS_RCODESEQUENCEINSTREAM_HPP_ */
