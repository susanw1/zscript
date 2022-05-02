/*
 * ZcodeSequenceInStream.hpp
 *
 *  Created on: 8 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_ZCODE_INSTREAMS_ZCODESEQUENCEINSTREAM_HPP_
#define SRC_MAIN_CPP_ZCODE_INSTREAMS_ZCODESEQUENCEINSTREAM_HPP_

#include "../ZcodeIncludes.hpp"
#include "ZcodeCommandInStream.hpp"
#include "ZcodeMarkerInStream.hpp"

template<class ZP>
class ZcodeChannelInStream;

template<class ZP>
class ZcodeSequenceInStream {
    private:
        ZcodeChannelInStream<ZP> *channelIn;
        ZcodeCommandInStream<ZP> commandIn;
        ZcodeMarkerInStream<ZP> markerIn;

        char current = 0;
        bool opened = false;
        bool locked = false;

    public:
        ZcodeSequenceInStream(ZcodeChannelInStream<ZP> *channelIn) :
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
                while (current != Zchars::EOL_SYMBOL) {
                    readInternal();
                }
                opened = false;
            }
        }
        char read() {
            if (opened) {
                char prev = current;
                readInternal();
                if (opened && current == Zchars::EOL_SYMBOL) {
                    opened = false;
                }
                return prev;
            } else {
                return Zchars::EOL_SYMBOL;
            }
        }

        char peek() {
            if (opened) {
                return current;
            } else {
                return Zchars::EOL_SYMBOL;
            }
        }

        bool hasNext() {
            return opened;
        }

        void readInternal();

        void skipToError() {
            commandIn.close();
            while (commandIn.read() != Zchars::ORELSE_SYMBOL && opened) {
                commandIn.open();
                commandIn.close();
            }
        }

        ZcodeChannelInStream<ZP>* getChannelInStream() {
            return channelIn;
        }

        ZcodeCommandInStream<ZP>* getCommandInStream() {
            return &commandIn;
        }

        ZcodeMarkerInStream<ZP>* getMarkerInStream() {
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

template<class ZP>
void ZcodeSequenceInStream<ZP>::readInternal() {
    int next = channelIn->read();
    if (next == -1) {
        current = Zchars::EOL_SYMBOL;
        opened = false;
    } else {
        current = (char) next;
    }
}

#include "ZcodeChannelInStream.hpp"

#endif /* SRC_MAIN_CPP_ZCODE_INSTREAMS_ZCODESEQUENCEINSTREAM_HPP_ */
