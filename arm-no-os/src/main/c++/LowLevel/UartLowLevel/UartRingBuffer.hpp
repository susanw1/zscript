/*
 * UartRingBuffer.hpp
 *
 *  Created on: 8 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTRINGBUFFER_HPP_
#define SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTRINGBUFFER_HPP_
#include "../GeneralLLSetup.hpp"

template<uint16_t size>

class UartRingBuffer { // in theory, everything should work, but if the optimiser changes the order a bit,
    // there could be conditions (in extreme conditions) where the current read byte gets overwritten... would need to check the machine code to be sure

    // we don't fill in the last possible space, so we know the difference between full and empty...
    void (*overflowCallback)(UartIdentifier); // this is called from both write and attemptWrite - a bit weird but kind of useful...
    uint16_t writePos;
    uint16_t tempWritePos;
    uint16_t readPos;
    uint16_t peekPos;
    UartIdentifier id;
    uint8_t data[size];

public:
    UartRingBuffer() :
            overflowCallback(NULL), writePos(0), tempWritePos(0), readPos(0), peekPos(0), id(0) {
    }
    UartRingBuffer(void (*overflowCallback)(UartIdentifier), UartIdentifier id) :
            overflowCallback(overflowCallback), writePos(0), tempWritePos(0), readPos(0), peekPos(0), id(id) {
    }
    void setCallback(void (*overflowCallback)(UartIdentifier), UartIdentifier id) {
        this->overflowCallback = overflowCallback;
        this->id = id;
    }

    bool write(uint8_t datum) {
        if ((readPos == 0 && writePos == size - 1) || writePos == readPos - 1) {
            if (overflowCallback != NULL) {
                overflowCallback(id);
            }
            if ((readPos == 0 && writePos == size - 1) || writePos == readPos - 1) {
                return false;
            }
        }
        data[writePos++] = datum;
        if (writePos == size) {
            writePos = 0;
        }
        tempWritePos = writePos;
        return true;
    }

    bool write(const uint8_t *buffer, uint16_t length) {
        if (!canWrite(length)) {
            if (overflowCallback != NULL) {
                overflowCallback(id);
            }
            if (!canWrite(length)) {
                return false;
            }
        }
        for (int i = 0; i < length; i++) {
            data[writePos++] = buffer[i];
            if (writePos == size) {
                writePos = 0;
            }
        }
        tempWritePos = writePos;
        return true;
    }
    bool canWrite(uint16_t length) {
        uint16_t maxLength;
        if (writePos < readPos) {
            maxLength = readPos - writePos - 1; // -1 due to not filling last space...
        } else {
            maxLength = size - writePos + readPos - 1;
        }
        return length <= maxLength;
    }

    bool attemptWrite(const uint8_t *buffer, uint16_t length) { //starts writing without allowing read yet - used to discover if a message will fit
        if (!canWrite(length)) {
            if (overflowCallback != NULL) {
                overflowCallback(id);
            }
            if (!canWrite(length)) {
                return false;
            }
        }
        for (uint16_t i = 0; i < length; i++) {
            data[tempWritePos++] = buffer[i];
            if (tempWritePos == size) {
                tempWritePos = 0;
            }

        }
        return true;
    }
    bool attemptWrite(uint8_t datum) {
        if ((readPos == 0 && tempWritePos == size - 1) || tempWritePos == readPos - 1) {
            if (overflowCallback != NULL) {
                overflowCallback(id);
            }
            if ((readPos == 0 && tempWritePos == size - 1) || tempWritePos == readPos - 1) {
                return false;
            }
        }
        data[tempWritePos++] = datum;
        if (tempWritePos == size) {
            tempWritePos = 0;
        }
        return true;
    }

    bool canAttemptWrite(uint16_t length) {
        uint16_t maxLength;
        if (tempWritePos < readPos) {
            maxLength = readPos - tempWritePos - 1; // -1 due to not filling last space...
        } else {
            maxLength = size - tempWritePos + readPos - 1; // -1 due to not filling last space...
        }
        return length <= maxLength;
    }

    void cancelWrite() { //undoes an attempted write
        tempWritePos = writePos;
    }
    void completeWrite() { //allows reading of any attempted write
        writePos = tempWritePos;
    }

    uint16_t skip(uint16_t length) {
        if (length == 0) {
            peekPos = readPos;
            return 0;
        }
        uint16_t skipped = 0;
        if (writePos < readPos) {
            if (size - readPos > length) {
                readPos += length;
                peekPos = readPos;
                return length + skipped;
            } else {
                skipped = size - readPos;
                length = length - (size - readPos);
                readPos = 0;
            }
        }
        if (writePos - readPos >= length) {
            readPos += length;
            peekPos = readPos;
            return length + skipped;
        } else {
            uint16_t tmp = writePos - readPos;
            readPos = writePos;
            peekPos = readPos;
            return tmp + skipped;
        }
    }

    uint16_t available() {
        if (writePos < readPos) {
            return size - readPos + writePos;
        } else {
            return writePos - readPos;
        }
    }
    uint16_t read(uint8_t *buffer, uint16_t length) {
        uint16_t i = 0;
        while (i < length && readPos != writePos) {
            buffer[i++] = data[readPos];
            readPos++;
            if (readPos == size) {
                readPos = 0;
            }
        }
        peekPos = readPos;
        return i;
    }
    int16_t read() { //-1 if no data
        if (readPos == writePos) {
            return -1;
        }
        uint8_t datum = data[readPos];
        readPos++;
        if (readPos == size) {
            readPos = 0;
        }
        peekPos = readPos;
        return datum;
    }

    uint16_t availablePeek() {
        if (writePos < peekPos) {
            return size - peekPos + writePos;
        } else {
            return writePos - peekPos;
        }
    }
    uint16_t peek(uint8_t *buffer, uint16_t length) {
        uint16_t i = 0;
        while (i < length && peekPos != writePos) {
            buffer[i++] = data[peekPos];
            peekPos++;
            if (peekPos == size) {
                peekPos = 0;
            }
        }
        return i;
    }
    int16_t peek() { //-1 if no data
        if (peekPos == writePos) {
            return -1;
        }
        uint8_t datum = data[peekPos];
        peekPos++;
        if (peekPos == size) {
            peekPos = 0;
        }
        return datum;
    }
    void resetPeek() {
        peekPos = readPos;
    }
    void skipToPeek() {
        readPos = peekPos;
    }

    uint8_t* getCurrentLinearRead() {
        return data + readPos;
    }

    uint16_t getLinearReadLength() {
        if (writePos < readPos) {
            return size - readPos;
        } else {
            return writePos - readPos;
        }
    }
};

#endif /* SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTRINGBUFFER_HPP_ */
