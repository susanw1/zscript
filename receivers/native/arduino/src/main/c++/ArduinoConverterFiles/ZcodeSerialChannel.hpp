/*
 * ZcodeSerialChannel.hpp
 *
 *  Created on: 1 Jul 2022
 *      Author: robert
 */

#ifndef ARDUINO_SERIAL_CHANNEL_HPP_
#define ARDUINO_SERIAL_CHANNEL_HPP_

#ifndef SRC_TEST_CPP_ZCODE_ZCODE_HPP_
#error ZcodeSerialChannel.hpp needs to be included after Zcode.hpp
#endif

#ifdef ZCODE_HAVE_SERIAL_CHANNEL

#include "ZcodeChannelBuilder.hpp"

class ZcodeSerialChannelInStream: public ZcodeChannelInStream<ZcodeParams> {
private:

    uint8_t big[ZcodeParams::serialBigSize];
    uint8_t buffer = 0;
    bool usingBuffer = false;
    
public:
    ZcodeSerialChannelInStream(Zcode<ZcodeParams> *zcode, ZcodeCommandChannel<ZcodeParams> *channel) :
            ZcodeChannelInStream<ZcodeParams>(zcode, channel, big, ZcodeParams::serialBigSize) {
    }

    bool pushData() {
      if(usingBuffer){
        if(this->slot.acceptByte(buffer)){
          usingBuffer = false;
          return true;
        }
        return false;
      }
      if (Serial.available() > 0) {
        buffer = (uint8_t) Serial.read();
        if(!this->slot.acceptByte(buffer)){
          usingBuffer = true;
          return false;
        }
        return true;
      }else{
        return false;
      }
    }

};

class ZcodeSerialOutStream: public ZcodeOutStream<ZcodeParams> {
private:
    bool openB = false;

public:

    void open(ZcodeCommandChannel<ZcodeParams> *target, ZcodeOutStreamOpenType t) {
      (void) target;
      (void) t;
      openB = true;
    }

    bool isOpen() {
        return openB;
    }

    void close() {
        openB = false;
    }

    void writeByte(uint8_t value) {
        Serial.print((char) value);
    }

};

class ZcodeSerialChannel: public ZcodeCommandChannel<ZcodeParams> {
    ZcodeSerialChannelInStream seqin;
    ZcodeSerialOutStream out;

public:
    ZcodeSerialChannel(Zcode<ZcodeParams> *zcode) :
            ZcodeCommandChannel<ZcodeParams>(zcode, &seqin, &out, false), seqin(zcode, this) {
    }

};
ZcodeSerialChannel ZcodeSerialChannelI(&ZcodeI);
#endif
#endif /* ARDUINO_SERIAL_CHANNEL_HPP_ */
