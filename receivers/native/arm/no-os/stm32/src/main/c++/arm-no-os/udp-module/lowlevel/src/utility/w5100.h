/*
 * Copyright 2018 Paul Stoffregen
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

// w5100.h contains private W5x00 hardware "driver" level definitions
// which are not meant to be exposed to other libraries or Arduino users
#ifndef	W5100_H_INCLUDED
#define	W5100_H_INCLUDED

#include "../../SPI.h"
#include <arm-no-os/pins-module/lowlevel/GpioManager.hpp>

// Require Ethernet.h, because we need MAX_SOCK_NUM
#ifndef ethernet_h_
#error "Ethernet.h must be included before w5100.h"
#endif

typedef uint8_t SOCKET;

class SnMR {
public:
    static const uint8_t CLOSE = 0x00;
    static const uint8_t TCP = 0x21;
    static const uint8_t UDP = 0x02;
    static const uint8_t IPRAW = 0x03;
    static const uint8_t MACRAW = 0x04;
    static const uint8_t PPPOE = 0x05;
    static const uint8_t ND = 0x20;
    static const uint8_t MULTI = 0x80;
};

enum SockCMD {
    Sock_OPEN = 0x01,
    Sock_LISTEN = 0x02,
    Sock_CONNECT = 0x04,
    Sock_DISCON = 0x08,
    Sock_CLOSE = 0x10,
    Sock_SEND = 0x20,
    Sock_SEND_MAC = 0x21,
    Sock_SEND_KEEP = 0x22,
    Sock_RECV = 0x40
};

class SnIR {
public:
    static const uint8_t SEND_OK = 0x10;
    static const uint8_t TIMEOUT = 0x08;
    static const uint8_t RECV = 0x04;
    static const uint8_t DISCON = 0x02;
    static const uint8_t CON = 0x01;
};

class SnSR {
public:
    static const uint8_t CLOSED = 0x00;
    static const uint8_t INIT = 0x13;
    static const uint8_t LISTEN = 0x14;
    static const uint8_t SYNSENT = 0x15;
    static const uint8_t SYNRECV = 0x16;
    static const uint8_t ESTABLISHED = 0x17;
    static const uint8_t FIN_WAIT = 0x18;
    static const uint8_t CLOSING = 0x1A;
    static const uint8_t TIME_WAIT = 0x1B;
    static const uint8_t CLOSE_WAIT = 0x1C;
    static const uint8_t LAST_ACK = 0x1D;
    static const uint8_t UDP = 0x22;
    static const uint8_t IPRAW = 0x32;
    static const uint8_t MACRAW = 0x42;
    static const uint8_t PPPOE = 0x5F;
};

class IPPROTO {
public:
    static const uint8_t IP = 0;
    static const uint8_t ICMP = 1;
    static const uint8_t IGMP = 2;
    static const uint8_t GGP = 3;
    static const uint8_t TCP = 6;
    static const uint8_t PUP = 12;
    static const uint8_t UDP = 17;
    static const uint8_t IDP = 22;
    static const uint8_t ND = 77;
    static const uint8_t RAW = 255;
};

enum W5100Linkstatus {
    UNKNOWN,
    LINK_ON,
    LINK_OFF
};

template<class LL>
class W5100Class {

public:
    static uint8_t init(void);

    inline void setGatewayIp(const uint8_t *addr) {
        writeGAR(addr);
    }
    inline void getGatewayIp(uint8_t *addr) {
        readGAR(addr);
    }

    inline void setSubnetMask(const uint8_t *addr) {
        writeSUBR(addr);
    }
    inline void getSubnetMask(uint8_t *addr) {
        readSUBR(addr);
    }

    inline void setMACAddress(const uint8_t *addr) {
        writeSHAR(addr);
    }
    inline void getMACAddress(uint8_t *addr) {
        readSHAR(addr);
    }

    inline void setIPAddress(const uint8_t *addr) {
        writeSIPR(addr);
    }
    inline void getIPAddress(uint8_t *addr) {
        readSIPR(addr);
    }

    inline void setRetransmissionTime(uint16_t timeout) {
        writeRTR(timeout);
    }
    inline void setRetransmissionCount(uint8_t retry) {
        writeRCR(retry);
    }

    static void execCmdSn(SOCKET s, SockCMD _cmd);

    // W5100 Registers
    // ---------------
//private:
public:
    static uint16_t write(uint16_t addr, const uint8_t *buf, uint16_t len);
    static uint8_t write(uint16_t addr, uint8_t data) {
        return write(addr, &data, 1);
    }
    static uint16_t read(uint16_t addr, uint8_t *buf, uint16_t len);
    static uint8_t read(uint16_t addr) {
        uint8_t data;
        read(addr, &data, 1);
        return data;
    }

#define __GP_REGISTER8(name, address)             \
  static inline void write##name(uint8_t _data) { \
    write(address, _data);                        \
  }                                               \
  static inline uint8_t read##name() {            \
    return read(address);                         \
  }
#define __GP_REGISTER16(name, address)            \
  static void write##name(uint16_t _data) {       \
    uint8_t buf[2];                               \
    buf[0] = _data >> 8;                          \
    buf[1] = _data & 0xFF;                        \
    write(address, buf, 2);                       \
  }                                               \
  static uint16_t read##name() {                  \
    uint8_t buf[2];                               \
    read(address, buf, 2);                        \
    return (buf[0] << 8) | buf[1];                \
  }
#define __GP_REGISTER_N(name, address, size)      \
  static uint16_t write##name(const uint8_t *_buff) {   \
    return write(address, _buff, size);           \
  }                                               \
  static uint16_t read##name(uint8_t *_buff) {    \
    return read(address, _buff, size);            \
  }
    static W5100Linkstatus getLinkStatus();

public:
    __GP_REGISTER8 (MR, 0x0000)
        ;    // Mode
    __GP_REGISTER_N(GAR, 0x0001, 4)
        ; // Gateway IP address
    __GP_REGISTER_N(SUBR, 0x0005, 4)
        ; // Subnet mask address
    __GP_REGISTER_N(SHAR, 0x0009, 6)
        ; // Source MAC address
    __GP_REGISTER_N(SIPR, 0x000F, 4)
        ; // Source IP address
    __GP_REGISTER8 (IR, 0x0015)
        ;    // Interrupt
    __GP_REGISTER8 (IMR, 0x0016)
        ;    // Interrupt Mask
    __GP_REGISTER16(RTR, 0x0017)
        ;    // Timeout address
    __GP_REGISTER8 (RCR, 0x0019)
        ;    // Retry count
    __GP_REGISTER8 (RMSR, 0x001A)
        ;    // Receive memory size (W5100 only)
    __GP_REGISTER8 (TMSR, 0x001B)
        ;    // Transmit memory size (W5100 only)
    __GP_REGISTER8 (PATR, 0x001C)
        ;    // Authentication type address in PPPoE mode
    __GP_REGISTER8 (PTIMER, 0x0028)
        ;    // PPP LCP Request Timer
    __GP_REGISTER8 (PMAGIC, 0x0029)
        ;    // PPP LCP Magic Number
    __GP_REGISTER_N(UIPR, 0x002A, 4)
        ; // Unreachable IP address in UDP mode (W5100 only)
    __GP_REGISTER16(UPORT, 0x002E)
        ;    // Unreachable Port address in UDP mode (W5100 only)
    __GP_REGISTER8 (VERSIONR_W5200,0x001F)
        ;   // Chip Version Register (W5200 only)
    __GP_REGISTER8 (VERSIONR_W5500,0x0039)
        ;   // Chip Version Register (W5500 only)
    __GP_REGISTER8 (PSTATUS_W5200, 0x0035)
        ;    // PHY Status
    __GP_REGISTER8 (PHYCFGR_W5500, 0x002E)
        ;    // PHY Configuration register, default: 10111xxx

#undef __GP_REGISTER8
#undef __GP_REGISTER16
#undef __GP_REGISTER_N

    // W5100 Socket registers
    // ----------------------
private:
    static uint16_t CH_BASE(void) {
        //if (chip == 55) return 0x1000;
        //if (chip == 52) return 0x4000;
        //return 0x0400;
        return CH_BASE_MSB << 8;
    }
    static uint8_t CH_BASE_MSB; // 1 redundant byte, saves ~80 bytes code on AVR
    static const uint16_t CH_SIZE = 0x0100;

    static inline uint8_t readSn(SOCKET s, uint16_t addr) {
        return read(CH_BASE() + s * CH_SIZE + addr);
    }
    static inline uint8_t writeSn(SOCKET s, uint16_t addr, uint8_t data) {
        return write(CH_BASE() + s * CH_SIZE + addr, data);
    }
    static inline uint16_t readSn(SOCKET s, uint16_t addr, uint8_t *buf, uint16_t len) {
        return read(CH_BASE() + s * CH_SIZE + addr, buf, len);
    }
    static inline uint16_t writeSn(SOCKET s, uint16_t addr, uint8_t *buf, uint16_t len) {
        return write(CH_BASE() + s * CH_SIZE + addr, buf, len);
    }

#define __SOCKET_REGISTER8(name, address)                    \
  static inline void write##name(SOCKET _s, uint8_t _data) { \
    writeSn(_s, address, _data);                             \
  }                                                          \
  static inline uint8_t read##name(SOCKET _s) {              \
    return readSn(_s, address);                              \
  }
#define __SOCKET_REGISTER16(name, address)                   \
  static void write##name(SOCKET _s, uint16_t _data) {       \
    uint8_t buf[2];                                          \
    buf[0] = _data >> 8;                                     \
    buf[1] = _data & 0xFF;                                   \
    writeSn(_s, address, buf, 2);                            \
  }                                                          \
  static uint16_t read##name(SOCKET _s) {                    \
    uint8_t buf[2];                                          \
    readSn(_s, address, buf, 2);                             \
    return (buf[0] << 8) | buf[1];                           \
  }
#define __SOCKET_REGISTER_N(name, address, size)             \
  static uint16_t write##name(SOCKET _s, uint8_t *_buff) {   \
    return writeSn(_s, address, _buff, size);                \
  }                                                          \
  static uint16_t read##name(SOCKET _s, uint8_t *_buff) {    \
    return readSn(_s, address, _buff, size);                 \
  }

public:
    __SOCKET_REGISTER8(SnMR, 0x0000)        // Mode
    __SOCKET_REGISTER8(SnCR, 0x0001)        // Command
    __SOCKET_REGISTER8(SnIR, 0x0002)        // Interrupt
    __SOCKET_REGISTER8(SnSR, 0x0003)        // Status
    __SOCKET_REGISTER16(SnPORT, 0x0004)        // Source Port
    __SOCKET_REGISTER_N(SnDHAR, 0x0006, 6)     // Destination Hardw Addr
    __SOCKET_REGISTER_N(SnDIPR, 0x000C, 4)     // Destination IP Addr
    __SOCKET_REGISTER16(SnDPORT, 0x0010)        // Destination Port
    __SOCKET_REGISTER16(SnMSSR, 0x0012)        // Max Segment Size
    __SOCKET_REGISTER8(SnPROTO, 0x0014)        // Protocol in IP RAW Mode
    __SOCKET_REGISTER8(SnTOS, 0x0015)        // IP TOS
    __SOCKET_REGISTER8(SnTTL, 0x0016)        // IP TTL
    __SOCKET_REGISTER8(SnRX_SIZE, 0x001E)        // RX Memory Size (W5200 only)
    __SOCKET_REGISTER8(SnTX_SIZE, 0x001F)        // RX Memory Size (W5200 only)
    __SOCKET_REGISTER16(SnTX_FSR, 0x0020)        // TX Free Size
    __SOCKET_REGISTER16(SnTX_RD, 0x0022)        // TX Read Pointer
    __SOCKET_REGISTER16(SnTX_WR, 0x0024)        // TX Write Pointer
    __SOCKET_REGISTER16(SnRX_RSR, 0x0026)        // RX Free Size
    __SOCKET_REGISTER16(SnRX_RD, 0x0028)        // RX Read Pointer
    __SOCKET_REGISTER16(SnRX_WR, 0x002A)        // RX Write Pointer (supported?)

#undef __SOCKET_REGISTER8
#undef __SOCKET_REGISTER16
#undef __SOCKET_REGISTER_N

private:
    static uint8_t chip;
    static uint8_t ss_pin;
    static uint8_t softReset(void);
    static uint8_t isW5100(void);
    static uint8_t isW5200(void);
    static uint8_t isW5500(void);

public:
    static uint8_t getChip(void) {
        return chip;
    }
#ifdef ETHERNET_LARGE_BUFFERS
  static uint16_t SSIZE;
  static uint16_t SMASK;
#else
    static const uint16_t SSIZE = 2048;
    static const uint16_t SMASK = 0x07FF;
    #endif
    static uint16_t SBASE(uint8_t socknum) {
        if (chip == 51) {
            return socknum * SSIZE + 0x4000;
        } else {
            return socknum * SSIZE + 0x8000;
        }
    }
    static uint16_t RBASE(uint8_t socknum) {
        if (chip == 51) {
            return socknum * SSIZE + 0x6000;
        } else {
            return socknum * SSIZE + 0xC000;
        }
    }

    static bool hasOffsetAddressMapping(void) {
        if (chip == 55)
            return true;
        return false;
    }

private:
    #if defined(__AVR__)
	static volatile uint8_t *ss_pin_reg;
	static uint8_t ss_pin_mask;
	inline static void initSS() {
		ss_pin_reg = portOutputRegister(digitalPinToPort(ss_pin));
		ss_pin_mask = digitalPinToBitMask(ss_pin);
		pinMode(ss_pin, OUTPUT);
	}
	inline static void setSS() {
		*(ss_pin_reg) &= ~ss_pin_mask;
	}
	inline static void resetSS() {
		*(ss_pin_reg) |= ss_pin_mask;
	}
#elif defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK66FX1M0__) || defined(__MK64FX512__)
	static volatile uint8_t *ss_pin_reg;
	inline static void initSS() {
		ss_pin_reg = portOutputRegister(ss_pin);
		pinMode(ss_pin, OUTPUT);
	}
	inline static void setSS() {
		*(ss_pin_reg+256) = 1;
	}
	inline static void resetSS() {
		*(ss_pin_reg+128) = 1;
	}
#elif defined(__MKL26Z64__)
	static volatile uint8_t *ss_pin_reg;
	static uint8_t ss_pin_mask;
	inline static void initSS() {
		ss_pin_reg = portOutputRegister(digitalPinToPort(ss_pin));
		ss_pin_mask = digitalPinToBitMask(ss_pin);
		pinMode(ss_pin, OUTPUT);
	}
	inline static void setSS() {
		*(ss_pin_reg+8) = ss_pin_mask;
	}
	inline static void resetSS() {
		*(ss_pin_reg+4) = ss_pin_mask;
	}
#elif defined(__SAM3X8E__) || defined(__SAM3A8C__) || defined(__SAM3A4C__)
	static volatile uint32_t *ss_pin_reg;
	static uint32_t ss_pin_mask;
	inline static void initSS() {
		ss_pin_reg = &(digitalPinToPort(ss_pin)->PIO_PER);
		ss_pin_mask = digitalPinToBitMask(ss_pin);
		pinMode(ss_pin, OUTPUT);
	}
	inline static void setSS() {
		*(ss_pin_reg+13) = ss_pin_mask;
	}
	inline static void resetSS() {
		*(ss_pin_reg+12) = ss_pin_mask;
	}
#elif defined(__PIC32MX__)
	static volatile uint32_t *ss_pin_reg;
	static uint32_t ss_pin_mask;
	inline static void initSS() {
		ss_pin_reg = portModeRegister(digitalPinToPort(ss_pin));
		ss_pin_mask = digitalPinToBitMask(ss_pin);
		pinMode(ss_pin, OUTPUT);
	}
	inline static void setSS() {
		*(ss_pin_reg+8+1) = ss_pin_mask;
	}
	inline static void resetSS() {
		*(ss_pin_reg+8+2) = ss_pin_mask;
	}

#elif defined(ARDUINO_ARCH_ESP8266)
	static volatile uint32_t *ss_pin_reg;
	static uint32_t ss_pin_mask;
	inline static void initSS() {
		ss_pin_reg = (volatile uint32_t*)GPO;
		ss_pin_mask = 1 << ss_pin;
		pinMode(ss_pin, OUTPUT);
	}
	inline static void setSS() {
		GPOC = ss_pin_mask;
	}
	inline static void resetSS() {
		GPOS = ss_pin_mask;
	}

#elif defined(__SAMD21G18A__)
	static volatile uint32_t *ss_pin_reg;
	static uint32_t ss_pin_mask;
	inline static void initSS() {
		ss_pin_reg = portModeRegister(digitalPinToPort(ss_pin));
		ss_pin_mask = digitalPinToBitMask(ss_pin);
		pinMode(ss_pin, OUTPUT);
	}
	inline static void setSS() {
		*(ss_pin_reg+5) = ss_pin_mask;
	}
	inline static void resetSS() {
		*(ss_pin_reg+6) = ss_pin_mask;
	}
#else
    inline static void initSS() {
        GpioPin<LL> nss = GpioManager<LL>::getPin(SPI_1_UDP_SS);
        nss.init();
        nss.set();
        nss.setOutputSpeed(VeryHighSpeed);
        nss.setOutputMode(PushPull);
        nss.setPullMode(NoPull);
        nss.setMode(Output);
    }
    inline static void setSS() {
        GpioManager<LL>::getPin(SPI_1_UDP_SS).reset();
    }
    inline static void resetSS() {
        GpioManager<LL>::getPin(SPI_1_UDP_SS).set();
    }
#endif
};

#endif

#ifndef UTIL_H
#define UTIL_H

#define htons(x) ( (((x)<<8)&0xFF00) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)

#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
                   ((x)<< 8 & 0x00FF0000UL) | \
                   ((x)>> 8 & 0x0000FF00UL) | \
                   ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)

/*
 * Copyright 2018 Paul Stoffregen
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "../Ethernet.h"
#include <arm-no-os/system/clock/SystemMilliClock.hpp>
#include "w5100.h"
#include <string.h>

/***************************************************/
/**            Default SS pin setting             **/
/***************************************************/

// If variant.h or other headers specifically define the
// default SS pin for ethernet, use it.
#if defined(PIN_SPI_SS_ETHERNET_LIB)
#define SS_PIN_DEFAULT  PIN_SPI_SS_ETHERNET_LIB

// MKR boards default to pin 5 for MKR ETH
// Pins 8-10 are MOSI/SCK/MISO on MRK, so don't use pin 10
#elif defined(USE_ARDUINO_MKR_PIN_LAYOUT) || defined(ARDUINO_SAMD_MKRZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_MKRFox1200) || defined(ARDUINO_SAMD_MKRGSM1400) || defined(ARDUINO_SAMD_MKRWAN1300)
#define SS_PIN_DEFAULT  5

// For boards using AVR, assume shields with SS on pin 10
// will be used.  This allows for Arduino Mega (where
// SS is pin 53) and Arduino Leonardo (where SS is pin 17)
// to work by default with Arduino Ethernet Shield R2 & R3.
#elif defined(__AVR__)
#define SS_PIN_DEFAULT  10

// If variant.h or other headers define these names
// use them if none of the other cases match
#elif defined(PIN_SPI_SS)
#define SS_PIN_DEFAULT  PIN_SPI_SS
#elif defined(CORE_SS0_PIN)
#define SS_PIN_DEFAULT  CORE_SS0_PIN

// As a final fallback, use pin 10
#else
#define SS_PIN_DEFAULT  10
#endif

// W5100 controller instance
template<class LL>
uint8_t W5100Class<LL>::chip = 0;
template<class LL>
uint8_t W5100Class<LL>::CH_BASE_MSB;
template<class LL>
uint8_t W5100Class<LL>::ss_pin = SS_PIN_DEFAULT;
#ifdef ETHERNET_LARGE_BUFFERS
template<class LL>
uint16_t W5100Class<LL>::SSIZE = 2048;
template<class LL>
uint16_t W5100Class<LL>::SMASK = 0x07FF;
#endif
template<class LL>
W5100Class<LL> W5100;

// pointers and bitmasks for optimized SS pin
#if defined(__AVR__)
  volatile uint8_t * W5100Class::ss_pin_reg;
  uint8_t W5100Class::ss_pin_mask;
#elif defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK66FX1M0__) || defined(__MK64FX512__)
  volatile uint8_t * W5100Class::ss_pin_reg;
#elif defined(__MKL26Z64__)
  volatile uint8_t * W5100Class::ss_pin_reg;
  uint8_t W5100Class::ss_pin_mask;
#elif defined(__SAM3X8E__) || defined(__SAM3A8C__) || defined(__SAM3A4C__)
  volatile uint32_t * W5100Class::ss_pin_reg;
  uint32_t W5100Class::ss_pin_mask;
#elif defined(__PIC32MX__)
  volatile uint32_t * W5100Class::ss_pin_reg;
  uint32_t W5100Class::ss_pin_mask;
#elif defined(ARDUINO_ARCH_ESP8266)
  volatile uint32_t * W5100Class::ss_pin_reg;
  uint32_t W5100Class::ss_pin_mask;
#elif defined(__SAMD21G18A__)
  volatile uint32_t * W5100Class::ss_pin_reg;
  uint32_t W5100Class::ss_pin_mask;
#endif

template<class LL>
uint8_t W5100Class<LL>::init(void)
        {
    static bool initialized = false;
    uint8_t i;

    if (initialized)
        return 1;

    // Many Ethernet shields have a CAT811 or similar reset chip
    // connected to W5100 or W5200 chips.  The W5200 will not work at
    // all, and may even drive its MISO pin, until given an active low
    // reset pulse!  The CAT811 has a 240 ms typical pulse length, and
    // a 400 ms worst case maximum pulse length.  MAX811 has a worst
    // case maximum 560 ms pulse length.  This delay is meant to wait
    // until the reset pulse is ended.  If your hardware has a shorter
    // reset time, this can be edited or removed.
    SystemMilliClock < LL > ::blockDelayMillis(560);
    //Serial.println("w5100 init");

    SPI<LL> .begin();
    initSS();
    resetSS();
    SPI<LL> .beginTransaction();

    // Attempt W5200 detection first, because W5200 does not properly
    // reset its SPI state when CS goes high (inactive).  Communication
    // from detecting the other chips can leave the W5200 in a state
    // where it won't recover, unless given a reset pulse.
    if (isW5200()) {
        CH_BASE_MSB = 0x40;
#ifdef ETHERNET_LARGE_BUFFERS
#if MAX_SOCK_NUM <= 1
        SSIZE = 16384;
#elif MAX_SOCK_NUM <= 2
        SSIZE = 8192;
#elif MAX_SOCK_NUM <= 4
        SSIZE = 4096;
#else
        SSIZE = 2048;
#endif
        SMASK = SSIZE - 1;
#endif
        for (i = 0; i < MAX_SOCK_NUM; i++) {
            writeSnRX_SIZE(i, SSIZE >> 10);
            writeSnTX_SIZE(i, SSIZE >> 10);
        }
        for (; i < 8; i++) {
            writeSnRX_SIZE(i, 0);
            writeSnTX_SIZE(i, 0);
        }
        // Try W5500 next.  Wiznet finally seems to have implemented
        // SPI well with this chip.  It appears to be very resilient,
        // so try it after the fragile W5200
    } else if (isW5500()) {
        CH_BASE_MSB = 0x10;
#ifdef ETHERNET_LARGE_BUFFERS
#if MAX_SOCK_NUM <= 1
        SSIZE = 16384;
#elif MAX_SOCK_NUM <= 2
        SSIZE = 8192;
#elif MAX_SOCK_NUM <= 4
        SSIZE = 4096;
#else
        SSIZE = 2048;
#endif
        SMASK = SSIZE - 1;
        for (i=0; i<MAX_SOCK_NUM; i++) {
            writeSnRX_SIZE(i, SSIZE >> 10);
            writeSnTX_SIZE(i, SSIZE >> 10);
        }
        for (; i<8; i++) {
            writeSnRX_SIZE(i, 0);
            writeSnTX_SIZE(i, 0);
        }
#endif
        // Try W5100 last.  This simple chip uses fixed 4 byte frames
        // for every 8 bit access.  Terribly inefficient, but so simple
        // it recovers from "hearing" unsuccessful W5100 or W5200
        // communication.  W5100 is also the only chip without a VERSIONR
        // register for identification, so we check this last.
    } else if (isW5100()) {
        CH_BASE_MSB = 0x04;
#ifdef ETHERNET_LARGE_BUFFERS
#if MAX_SOCK_NUM <= 1
        SSIZE = 8192;
        writeTMSR(0x03);
        writeRMSR(0x03);
#elif MAX_SOCK_NUM <= 2
        SSIZE = 4096;
        writeTMSR(0x0A);
        writeRMSR(0x0A);
#else
        SSIZE = 2048;
        writeTMSR(0x55);
        writeRMSR(0x55);
#endif
        SMASK = SSIZE - 1;
#else
        writeTMSR(0x55);
        writeRMSR(0x55);
#endif
        // No hardware seems to be present.  Or it could be a W5200
        // that's heard other SPI communication if its chip select
        // pin wasn't high when a SD card or other SPI chip was used.
    } else {
        //Serial.println("no chip :-(");
        chip = 0;
        SPI<LL> .endTransaction();
        return 0; // no known chip is responding :-(
    }
    SPI<LL> .endTransaction();
    initialized = true;
    return 1; // successful init
}

// Soft reset the Wiznet chip, by writing to its MR register reset bit

template<class LL>
uint8_t W5100Class<LL>::softReset(void)
        {
    uint16_t count = 0;

    //Serial.println("Wiznet soft reset");
    // write to reset bit
    writeMR(0x80);
    // then wait for soft reset to complete
    do {
        uint8_t mr = readMR();
        //Serial.print("mr=");
        //Serial.println(mr, HEX);
        if (mr == 0)
            return 1;
        SystemMilliClock < LL > ::blockDelayMillis(1);
    } while (++count < 20);
    return 0;
}

template<class LL>
uint8_t W5100Class<LL>::isW5100(void)
        {
    chip = 51;
    //Serial.println("w5100.cpp: detect W5100 chip");
    if (!softReset())
        return 0;
    writeMR(0x10);
    if (readMR() != 0x10)
        return 0;
    writeMR(0x12);
    if (readMR() != 0x12)
        return 0;
    writeMR(0x00);
    if (readMR() != 0x00)
        return 0;
    //Serial.println("chip is W5100");
    return 1;
}

template<class LL>
uint8_t W5100Class<LL>::isW5200(void)
        {
    chip = 52;
    //Serial.println("w5100.cpp: detect W5200 chip");
    if (!softReset())
        return 0;
    writeMR(0x08);
    if (readMR() != 0x08)
        return 0;
    writeMR(0x10);
    if (readMR() != 0x10)
        return 0;
    writeMR(0x00);
    if (readMR() != 0x00)
        return 0;
    int ver = readVERSIONR_W5200();
    //Serial.print("version=");
    //Serial.println(ver);
    if (ver != 3)
        return 0;
    //Serial.println("chip is W5200");
    return 1;
}

template<class LL>
uint8_t W5100Class<LL>::isW5500(void)
        {
    chip = 55;
    //Serial.println("w5100.cpp: detect W5500 chip");
    if (!softReset())
        return 0;
    writeMR(0x08);
    if (readMR() != 0x08)
        return 0;
    writeMR(0x10);
    if (readMR() != 0x10)
        return 0;
    writeMR(0x00);
    if (readMR() != 0x00)
        return 0;
    int ver = readVERSIONR_W5500();
    //Serial.print("version=");
    //Serial.println(ver);
    if (ver != 4)
        return 0;
    //Serial.println("chip is W5500");
    return 1;
}

template<class LL>
W5100Linkstatus W5100Class<LL>::getLinkStatus()
{
    uint8_t phystatus;

    if (!init())
        return UNKNOWN;
    switch (chip) {
    case 52:
        SPI<LL> .beginTransaction();
        phystatus = readPSTATUS_W5200();
        SPI<LL> .endTransaction();
        if (phystatus & 0x20)
            return LINK_ON;
        return LINK_OFF;
    case 55:
        SPI<LL> .beginTransaction();
        phystatus = readPHYCFGR_W5500();
        SPI<LL> .endTransaction();
        if (phystatus & 0x01)
            return LINK_ON;
        return LINK_OFF;
    default:
        return UNKNOWN;
    }
}

template<class LL>
uint16_t W5100Class<LL>::write(uint16_t addr, const uint8_t *buf, uint16_t len)
        {
    uint8_t cmd[8];

    if (chip == 51) {
        for (uint16_t i = 0; i < len; i++) {
            setSS();
            SPI<LL> .transfer(0xF0);
            SPI<LL> .transfer(addr >> 8);
            SPI<LL> .transfer(addr & 0xFF);
            addr++;
            SPI<LL> .transfer(buf[i]);
            resetSS();
        }
    } else if (chip == 52) {
        setSS();
        cmd[0] = addr >> 8;
        cmd[1] = addr & 0xFF;
        cmd[2] = ((len >> 8) & 0x7F) | 0x80;
        cmd[3] = len & 0xFF;
        SPI<LL> .transfer(cmd, 4);
#ifdef SPI_HAS_TRANSFER_BUF
        SPI<LL>.transfer(buf, NULL, len);
#else
        // TODO: copy 8 bytes at a time to cmd[] and block transfer
        for (uint16_t i = 0; i < len; i++) {
            SPI<LL> .transfer(buf[i]);
        }
#endif
        resetSS();
    } else { // chip == 55
        setSS();
        if (addr < 0x100) {
            // common registers 00nn
            cmd[0] = 0;
            cmd[1] = addr & 0xFF;
            cmd[2] = 0x04;
        } else if (addr < 0x8000) {
            // socket registers  10nn, 11nn, 12nn, 13nn, etc
            cmd[0] = 0;
            cmd[1] = addr & 0xFF;
            cmd[2] = ((addr >> 3) & 0xE0) | 0x0C;
        } else if (addr < 0xC000) {
            // transmit buffers  8000-87FF, 8800-8FFF, 9000-97FF, etc
            //  10## #nnn nnnn nnnn
            cmd[0] = addr >> 8;
            cmd[1] = addr & 0xFF;
#if defined(ETHERNET_LARGE_BUFFERS) && MAX_SOCK_NUM <= 1
            cmd[2] = 0x14;                       // 16K buffers
            #elif defined(ETHERNET_LARGE_BUFFERS) && MAX_SOCK_NUM <= 2
            cmd[2] = ((addr >> 8) & 0x20) | 0x14; // 8K buffers
            #elif defined(ETHERNET_LARGE_BUFFERS) && MAX_SOCK_NUM <= 4
            cmd[2] = ((addr >> 7) & 0x60) | 0x14; // 4K buffers
            #else
            cmd[2] = ((addr >> 6) & 0xE0) | 0x14; // 2K buffers
#endif
        } else {
            // receive buffers
            cmd[0] = addr >> 8;
            cmd[1] = addr & 0xFF;
#if defined(ETHERNET_LARGE_BUFFERS) && MAX_SOCK_NUM <= 1
            cmd[2] = 0x1C;                       // 16K buffers
            #elif defined(ETHERNET_LARGE_BUFFERS) && MAX_SOCK_NUM <= 2
            cmd[2] = ((addr >> 8) & 0x20) | 0x1C; // 8K buffers
            #elif defined(ETHERNET_LARGE_BUFFERS) && MAX_SOCK_NUM <= 4
            cmd[2] = ((addr >> 7) & 0x60) | 0x1C; // 4K buffers
            #else
            cmd[2] = ((addr >> 6) & 0xE0) | 0x1C; // 2K buffers
#endif
        }
        if (len <= 5) {
            for (uint8_t i = 0; i < len; i++) {
                cmd[i + 3] = buf[i];
            }
            SPI<LL> .transfer(cmd, len + 3);
        } else {
            SPI<LL> .transfer(cmd, 3);
#ifdef SPI_HAS_TRANSFER_BUF
            SPI<LL>.transfer(buf, NULL, len);
#else
            // TODO: copy 8 bytes at a time to cmd[] and block transfer
            for (uint16_t i = 0; i < len; i++) {
                SPI<LL> .transfer(buf[i]);
            }
#endif
        }
        resetSS();
    }
    return len;
}

template<class LL>
uint16_t W5100Class<LL>::read(uint16_t addr, uint8_t *buf, uint16_t len)
        {
    uint8_t cmd[4];

    if (chip == 51) {
        for (uint16_t i = 0; i < len; i++) {
            setSS();
#if 1
            SPI<LL> .transfer(0x0F);
            SPI<LL> .transfer(addr >> 8);
            SPI<LL> .transfer(addr & 0xFF);
            addr++;
            buf[i] = SPI<LL> .transfer(0);
#else
            cmd[0] = 0x0F;
            cmd[1] = addr >> 8;
            cmd[2] = addr & 0xFF;
            cmd[3] = 0;
            SPI<LL>.transfer(cmd, 4); // TODO: why doesn't this work?
            buf[i] = cmd[3];
            addr++;
            #endif
            resetSS();
        }
    } else if (chip == 52) {
        setSS();
        cmd[0] = addr >> 8;
        cmd[1] = addr & 0xFF;
        cmd[2] = (len >> 8) & 0x7F;
        cmd[3] = len & 0xFF;
        SPI<LL> .transfer(cmd, 4);
        memset(buf, 0, len);
        SPI<LL> .transfer(buf, len);
        resetSS();
    } else { // chip == 55
        setSS();
        if (addr < 0x100) {
            // common registers 00nn
            cmd[0] = 0;
            cmd[1] = addr & 0xFF;
            cmd[2] = 0x00;
        } else if (addr < 0x8000) {
            // socket registers  10nn, 11nn, 12nn, 13nn, etc
            cmd[0] = 0;
            cmd[1] = addr & 0xFF;
            cmd[2] = ((addr >> 3) & 0xE0) | 0x08;
        } else if (addr < 0xC000) {
            // transmit buffers  8000-87FF, 8800-8FFF, 9000-97FF, etc
            //  10## #nnn nnnn nnnn
            cmd[0] = addr >> 8;
            cmd[1] = addr & 0xFF;
#if defined(ETHERNET_LARGE_BUFFERS) && MAX_SOCK_NUM <= 1
            cmd[2] = 0x10;                       // 16K buffers
            #elif defined(ETHERNET_LARGE_BUFFERS) && MAX_SOCK_NUM <= 2
            cmd[2] = ((addr >> 8) & 0x20) | 0x10; // 8K buffers
            #elif defined(ETHERNET_LARGE_BUFFERS) && MAX_SOCK_NUM <= 4
            cmd[2] = ((addr >> 7) & 0x60) | 0x10; // 4K buffers
            #else
            cmd[2] = ((addr >> 6) & 0xE0) | 0x10; // 2K buffers
#endif
        } else {
            // receive buffers
            cmd[0] = addr >> 8;
            cmd[1] = addr & 0xFF;
#if defined(ETHERNET_LARGE_BUFFERS) && MAX_SOCK_NUM <= 1
            cmd[2] = 0x18;                       // 16K buffers
            #elif defined(ETHERNET_LARGE_BUFFERS) && MAX_SOCK_NUM <= 2
            cmd[2] = ((addr >> 8) & 0x20) | 0x18; // 8K buffers
            #elif defined(ETHERNET_LARGE_BUFFERS) && MAX_SOCK_NUM <= 4
            cmd[2] = ((addr >> 7) & 0x60) | 0x18; // 4K buffers
            #else
            cmd[2] = ((addr >> 6) & 0xE0) | 0x18; // 2K buffers
#endif
        }
        SPI<LL> .transfer(cmd, 3);
        memset(buf, 0, len);
        SPI<LL> .transfer(buf, len);
        resetSS();
    }
    return len;
}

template<class LL>
void W5100Class<LL>::execCmdSn(SOCKET s, SockCMD _cmd)
        {
    // Send command to socket
    writeSnCR(s, _cmd);
    // Wait for command to complete
    while (readSnCR(s))
        ;
}

template<class LL>
extern W5100Class<LL> W5100;

#endif
