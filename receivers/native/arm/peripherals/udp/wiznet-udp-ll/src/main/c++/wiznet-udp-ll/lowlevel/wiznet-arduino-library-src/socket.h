/* Copyright 2018 Paul Stoffregen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "Ethernet.h"
#include "utility/w5100.h"

#if ARDUINO >= 156 && !defined(ARDUINO_ARCH_PIC32)
extern void yield(void);
#else
#define yield()
#endif

template<class LL>
class EthernetServer;

// TODO: randomize this when not using DHCP, but how?
static uint16_t local_port = 49152;  // 49152 to 65535

typedef struct {
    uint16_t RX_RSR; // Number of bytes received
    uint16_t RX_RD;  // Address to read
    uint16_t TX_FSR; // Free space ready for transmit
    uint8_t RX_inc; // how much have we advanced RX_RD
} socketstate_t;

static socketstate_t state[MAX_SOCK_NUM];

template<class LL>
static uint16_t getSnTX_FSR(uint8_t s);
template<class LL>
static uint16_t getSnRX_RSR(uint8_t s);
template<class LL>
static void write_data(uint8_t s, uint16_t offset, const uint8_t *data, uint16_t len);
template<class LL>
static void read_data(uint8_t s, uint16_t src, uint8_t *dst, uint16_t len);

/*****************************************/
/*          Socket management            */
/*****************************************/

template<class LL>
void EthernetClass<LL>::socketPortRand(uint16_t n)
        {
    n &= 0x3FFF;
    local_port ^= n;
    //Serial.printf("socketPortRand %d, srcport=%d\n", n, local_port);
}

template<class LL>
uint8_t EthernetClass<LL>::socketBegin(uint8_t protocol, uint16_t port)
        {
    uint8_t s, status[MAX_SOCK_NUM], chip, maxindex = MAX_SOCK_NUM;

    // first check hardware compatibility
    chip = W5100<LL> .getChip();
    if (!chip)
        return MAX_SOCK_NUM; // immediate error if no hardware detected
#if MAX_SOCK_NUM > 4
    if (chip == 51)
        maxindex = 4; // W5100 chip never supports more than 4 sockets
#endif
    //Serial.printf("W5000socket begin, protocol=%d, port=%d\n", protocol, port);
    SPI<LL> .beginTransaction();
    // look at all the hardware sockets, use any that are closed (unused)
    for (s = 0; s < maxindex; s++) {
        status[s] = W5100<LL> .readSnSR(s);
        if (status[s] == SnSR::CLOSED)
            goto makesocket;
    }
    //Serial.printf("W5000socket step2\n");
    // as a last resort, forcibly close any already closing
    for (s = 0; s < maxindex; s++) {
        uint8_t stat = status[s];
        if (stat == SnSR::LAST_ACK)
            goto closemakesocket;
        if (stat == SnSR::TIME_WAIT)
            goto closemakesocket;
        if (stat == SnSR::FIN_WAIT)
            goto closemakesocket;
        if (stat == SnSR::CLOSING)
            goto closemakesocket;
    }
#if 0
	//Serial.printf("W5000socket step3\n");
	// next, use any that are effectively closed
	for (s=0; s < MAX_SOCK_NUM; s++) {
		uint8_t stat = status[s];
		// TODO: this also needs to check if no more data
		if (stat == SnSR::CLOSE_WAIT) goto closemakesocket;
	}
#endif
    SPI<LL> .endTransaction();
    return MAX_SOCK_NUM; // all sockets are in use
    closemakesocket:
    //Serial.printf("W5000socket close\n");
    W5100<LL> .execCmdSn(s, Sock_CLOSE);
    makesocket:
    //Serial.printf("W5000socket %d\n", s);
#ifdef SUPPORT_TCP
    EthernetServer<LL>::server_port[s] = 0;
#endif
    for (volatile int i = 0; i < 0x800; ++i)
        ;
    //delayMicroseconds(250);
    W5100<LL> .writeSnMR(s, protocol);
    W5100<LL> .writeSnIR(s, 0xFF);
    if (port > 0) {
        W5100<LL> .writeSnPORT(s, port);
    } else {
        // if don't set the source port, set local_port number.
        if (++local_port < 49152)
            local_port = 49152;
        W5100<LL> .writeSnPORT(s, local_port);
    }
    W5100<LL> .execCmdSn(s, Sock_OPEN);
    state[s].RX_RSR = 0;
    state[s].RX_RD = W5100<LL> .readSnRX_RD(s); // always zero?
    state[s].RX_inc = 0;
    state[s].TX_FSR = 0;
    //Serial.printf("W5000socket prot=%d, RX_RD=%d\n", W5100<LL>.readSnMR(s), state[s].RX_RD);
    SPI<LL> .endTransaction();
    return s;
}

// multicast version to set fields before open  thd
template<class LL>
uint8_t EthernetClass<LL>::socketBeginMulticast(uint8_t protocol, IPAddress ip, uint16_t port)
        {
    uint8_t s, status[MAX_SOCK_NUM], chip, maxindex = MAX_SOCK_NUM;

    // first check hardware compatibility
    chip = W5100<LL> .getChip();
    if (!chip)
        return MAX_SOCK_NUM; // immediate error if no hardware detected
#if MAX_SOCK_NUM > 4
    if (chip == 51)
        maxindex = 4; // W5100 chip never supports more than 4 sockets
#endif
    //Serial.printf("W5000socket begin, protocol=%d, port=%d\n", protocol, port);
    SPI<LL> .beginTransaction();
    // look at all the hardware sockets, use any that are closed (unused)
    for (s = 0; s < maxindex; s++) {
        status[s] = W5100<LL> .readSnSR(s);
        if (status[s] == SnSR::CLOSED)
            goto makesocket;
    }
    //Serial.printf("W5000socket step2\n");
    // as a last resort, forcibly close any already closing
    for (s = 0; s < maxindex; s++) {
        uint8_t stat = status[s];
        if (stat == SnSR::LAST_ACK)
            goto closemakesocket;
        if (stat == SnSR::TIME_WAIT)
            goto closemakesocket;
        if (stat == SnSR::FIN_WAIT)
            goto closemakesocket;
        if (stat == SnSR::CLOSING)
            goto closemakesocket;
    }
#if 0
	//Serial.printf("W5000socket step3\n");
	// next, use any that are effectively closed
	for (s=0; s < MAX_SOCK_NUM; s++) {
		uint8_t stat = status[s];
		// TODO: this also needs to check if no more data
		if (stat == SnSR::CLOSE_WAIT) goto closemakesocket;
	}
#endif
    SPI<LL> .endTransaction();
    return MAX_SOCK_NUM; // all sockets are in use
    closemakesocket:
    //Serial.printf("W5000socket close\n");
    W5100<LL> .execCmdSn(s, Sock_CLOSE);
    makesocket:
    //Serial.printf("W5000socket %d\n", s);
#ifdef SUPPORT_TCP
    EthernetServer<LL>::server_port[s] = 0;
#endif
    for (volatile int i = 0; i < 0x800; ++i)
        ;
    //delayMicroseconds(250);
    W5100<LL> .writeSnMR(s, protocol);
    W5100<LL> .writeSnIR(s, 0xFF);
    if (port > 0) {
        W5100<LL> .writeSnPORT(s, port);
    } else {
        // if don't set the source port, set local_port number.
        if (++local_port < 49152)
            local_port = 49152;
        W5100<LL> .writeSnPORT(s, local_port);
    }
    // Calculate MAC address from Multicast IP Address
    uint8_t mac[] = { 0x01, 0x00, 0x5E, 0x00, 0x00, 0x00 };
    mac[3] = ip[1] & 0x7F;
    mac[4] = ip[2];
    mac[5] = ip[3];
    W5100<LL> .writeSnDIPR(s, ip.raw_address());   //239.255.0.1
    W5100<LL> .writeSnDPORT(s, port);
    W5100<LL> .writeSnDHAR(s, mac);
    W5100<LL> .execCmdSn(s, Sock_OPEN);
    state[s].RX_RSR = 0;
    state[s].RX_RD = W5100<LL> .readSnRX_RD(s); // always zero?
    state[s].RX_inc = 0;
    state[s].TX_FSR = 0;
    //Serial.printf("W5000socket prot=%d, RX_RD=%d\n", W5100<LL>.readSnMR(s), state[s].RX_RD);
    SPI<LL> .endTransaction();
    return s;
}
// Return the socket's status
//
template<class LL>
uint8_t EthernetClass<LL>::socketStatus(uint8_t s)
        {
    SPI<LL> .beginTransaction();
    uint8_t status = W5100<LL> .readSnSR(s);
    SPI<LL> .endTransaction();
    return status;
}

// Immediately close.  If a TCP connection is established, the
// remote host is left unaware we closed.
//
template<class LL>
void EthernetClass<LL>::socketClose(uint8_t s)
        {
    SPI<LL> .beginTransaction();
    W5100<LL> .execCmdSn(s, Sock_CLOSE);
    SPI<LL> .endTransaction();
}

// Place the socket in listening (server) mode
//
template<class LL>
uint8_t EthernetClass<LL>::socketListen(uint8_t s)
        {
    SPI<LL> .beginTransaction();
    if (W5100<LL> .readSnSR(s) != SnSR::INIT) {
        SPI<LL> .endTransaction();
        return 0;
    }
    W5100<LL> .execCmdSn(s, Sock_LISTEN);
    SPI<LL> .endTransaction();
    return 1;
}

// establish a TCP connection in Active (client) mode.
//
template<class LL>
void EthernetClass<LL>::socketConnect(uint8_t s, uint8_t *addr, uint16_t port)
        {
    // set destination IP
    SPI<LL> .beginTransaction();
    W5100<LL> .writeSnDIPR(s, addr);
    W5100<LL> .writeSnDPORT(s, port);
    W5100<LL> .execCmdSn(s, Sock_CONNECT);
    SPI<LL> .endTransaction();
}

// Gracefully disconnect a TCP connection.
//
template<class LL>
void EthernetClass<LL>::socketDisconnect(uint8_t s)
        {
    SPI<LL> .beginTransaction();
    W5100<LL> .execCmdSn(s, Sock_DISCON);
    SPI<LL> .endTransaction();
}

/*****************************************/
/*    Socket Data Receive Functions      */
/*****************************************/

template<class LL>
static uint16_t getSnRX_RSR(uint8_t s)
        {
#if 1
    uint16_t val, prev;

    prev = W5100<LL> .readSnRX_RSR(s);
    while (1) {
        val = W5100<LL> .readSnRX_RSR(s);
        if (val == prev) {
            return val;
        }
        prev = val;
    }
#else
	uint16_t val = W5100<LL>.readSnRX_RSR(s);
	return val;
#endif
}

template<class LL>
static void read_data(uint8_t s, uint16_t src, uint8_t *dst, uint16_t len)
        {
    uint16_t size;
    uint16_t src_mask;
    uint16_t src_ptr;

    //Serial.printf("read_data, len=%d, at:%d\n", len, src);
    src_mask = (uint16_t) src & W5100<LL> .SMASK;
    src_ptr = W5100<LL> .RBASE(s) + src_mask;

    if (W5100<LL> .hasOffsetAddressMapping() || src_mask + len <= W5100<LL> .SSIZE) {
        W5100<LL> .read(src_ptr, dst, len);
    } else {
        size = W5100<LL> .SSIZE - src_mask;
        W5100<LL> .read(src_ptr, dst, size);
        dst += size;
        W5100<LL> .read(W5100<LL> .RBASE(s), dst, len - size);
    }
}

// Receive data.  Returns size, or -1 for no data, or 0 if connection closed
//
template<class LL>
int EthernetClass<LL>::socketRecv(uint8_t s, uint8_t *buf, int16_t len)
        {
    // Check how much data is available
    int ret = state[s].RX_RSR;
    SPI<LL> .beginTransaction();
    if (ret < len) {
        uint16_t rsr = getSnRX_RSR<LL>(s);
        ret = rsr - state[s].RX_inc;
        state[s].RX_RSR = ret;
        //Serial.printf("Sock_RECV, RX_RSR=%d, RX_inc=%d\n", ret, state[s].RX_inc);
    }
    if (ret == 0) {
        // No data available.
        uint8_t status = W5100<LL> .readSnSR(s);
        if (status == SnSR::LISTEN || status == SnSR::CLOSED ||
                status == SnSR::CLOSE_WAIT) {
            // The remote end has closed its side of the connection,
            // so this is the eof state
            ret = 0;
        } else {
            // The connection is still up, but there's no data waiting to be read
            ret = -1;
        }
    } else {
        if (ret > len)
            ret = len; // more data available than buffer length
        uint16_t ptr = state[s].RX_RD;
        if (buf)
            read_data<LL>(s, ptr, buf, ret);
        ptr += ret;
        state[s].RX_RD = ptr;
        state[s].RX_RSR -= ret;
        uint16_t inc = state[s].RX_inc + ret;
        if (inc >= 250 || state[s].RX_RSR == 0) {
            state[s].RX_inc = 0;
            W5100<LL> .writeSnRX_RD(s, ptr);
            W5100<LL> .execCmdSn(s, Sock_RECV);
            //Serial.printf("Sock_RECV cmd, RX_RD=%d, RX_RSR=%d\n",
            //  state[s].RX_RD, state[s].RX_RSR);
        } else {
            state[s].RX_inc = inc;
        }
    }
    SPI<LL> .endTransaction();
    //Serial.printf("socketRecv, ret=%d\n", ret);
    return ret;
}

template<class LL>
uint16_t EthernetClass<LL>::socketRecvAvailable(uint8_t s)
        {
    uint16_t ret = state[s].RX_RSR;
    if (ret == 0) {
        SPI<LL> .beginTransaction();
        uint16_t rsr = getSnRX_RSR<LL>(s);
        SPI<LL> .endTransaction();
        ret = rsr - state[s].RX_inc;
        state[s].RX_RSR = ret;
        //Serial.printf("sockRecvAvailable s=%d, RX_RSR=%d\n", s, ret);
    }
    return ret;
}

// get the first byte in the receive queue (no checking)
//
template<class LL>
uint8_t EthernetClass<LL>::socketPeek(uint8_t s)
        {
    uint8_t b;
    SPI<LL> .beginTransaction();
    uint16_t ptr = state[s].RX_RD;
    W5100<LL> .read((ptr & W5100<LL> .SMASK) + W5100<LL> .RBASE(s), &b, 1);
    SPI<LL> .endTransaction();
    return b;
}
template<class LL>
uint8_t EthernetClass<LL>::socketPeek(uint8_t s, uint16_t dist)
        {
    uint8_t b;
    SPI<LL> .beginTransaction();
    uint16_t ptr = state[s].RX_RD + dist;
    W5100<LL> .read((ptr & W5100<LL> .SMASK) + W5100<LL> .RBASE(s), &b, 1);
    SPI<LL> .endTransaction();
    return b;
}

/*****************************************/
/*    Socket Data Transmit Functions     */
/*****************************************/

template<class LL>
static uint16_t getSnTX_FSR(uint8_t s)
        {
    uint16_t val, prev;

    prev = W5100<LL> .readSnTX_FSR(s);
    while (1) {
        val = W5100<LL> .readSnTX_FSR(s);
        if (val == prev) {
            state[s].TX_FSR = val;
            return val;
        }
        prev = val;
    }
}

template<class LL>
static void write_data(uint8_t s, uint16_t data_offset, const uint8_t *data, uint16_t len)
        {
    uint16_t ptr = W5100<LL> .readSnTX_WR(s);
    ptr += data_offset;
    uint16_t offset = ptr & W5100<LL> .SMASK;
    uint16_t dstAddr = offset + W5100<LL> .SBASE(s);

    if (W5100<LL> .hasOffsetAddressMapping() || offset + len <= W5100<LL> .SSIZE) {
        W5100<LL> .write(dstAddr, data, len);
    } else {
        // Wrap around circular buffer
        uint16_t size = W5100<LL> .SSIZE - offset;
        W5100<LL> .write(dstAddr, data, size);
        W5100<LL> .write(W5100<LL> .SBASE(s), data + size, len - size);
    }
    ptr += len;
    W5100<LL> .writeSnTX_WR(s, ptr);
}

/**
 * @brief	This function used to send the data in TCP mode
 * @return	1 for success else 0.
 */
template<class LL>
uint16_t EthernetClass<LL>::socketSend(uint8_t s, const uint8_t *buf, uint16_t len)
        {
    uint8_t status = 0;
    uint16_t ret = 0;
    uint16_t freesize = 0;

    if (len > W5100<LL> .SSIZE) {
        ret = W5100<LL> .SSIZE; // check size not to exceed MAX size.
    } else {
        ret = len;
    }

    // if freebuf is available, start.
    do {
        SPI<LL> .beginTransaction();
        freesize = getSnTX_FSR<LL>(s);
        status = W5100<LL> .readSnSR(s);
        SPI<LL> .endTransaction();
        if ((status != SnSR::ESTABLISHED) && (status != SnSR::CLOSE_WAIT)) {
            ret = 0;
            break;
        }
        yield();
    } while (freesize < ret);

    // copy data
    SPI<LL> .beginTransaction();
    write_data<LL>(s, 0, (uint8_t*) buf, ret);
    W5100<LL> .execCmdSn(s, Sock_SEND);

    /* +2008.01 bj */
    while ((W5100<LL> .readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK) {
        /* m2008.01 [bj] : reduce code */
        if (W5100<LL> .readSnSR(s) == SnSR::CLOSED) {
            SPI<LL> .endTransaction();
            return 0;
        }
        SPI<LL> .endTransaction();
        yield();
        SPI<LL> .beginTransaction();
    }
    /* +2008.01 bj */
    W5100<LL> .writeSnIR(s, SnIR::SEND_OK);
    SPI<LL> .endTransaction();
    return ret;
}

template<class LL>
uint16_t EthernetClass<LL>::socketSendAvailable(uint8_t s)
        {
    uint8_t status = 0;
    uint16_t freesize = 0;
    SPI<LL> .beginTransaction();
    freesize = getSnTX_FSR<LL>(s);
    status = W5100<LL> .readSnSR(s);
    SPI<LL> .endTransaction();
    if ((status == SnSR::ESTABLISHED) || (status == SnSR::CLOSE_WAIT)) {
        return freesize;
    }
    return 0;
}

template<class LL>
uint16_t EthernetClass<LL>::socketBufferData(uint8_t s, uint16_t offset, const uint8_t *buf, uint16_t len)
        {
    //Serial.printf("  bufferData, offset=%d, len=%d\n", offset, len);
    uint16_t ret = 0;
    SPI<LL> .beginTransaction();
    uint16_t txfree = getSnTX_FSR<LL>(s);
    if (len > txfree) {
        ret = txfree; // check size not to exceed MAX size.
    } else {
        ret = len;
    }
    write_data<LL>(s, offset, buf, ret);
    SPI<LL> .endTransaction();
    return ret;
}

template<class LL>
bool EthernetClass<LL>::socketStartUDP(uint8_t s, uint8_t *addr, uint16_t port)
        {
    if (((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
            ((port == 0x00))) {
        return false;
    }
    SPI<LL> .beginTransaction();
    W5100<LL> .writeSnDIPR(s, addr);
    W5100<LL> .writeSnDPORT(s, port);
    SPI<LL> .endTransaction();
    return true;
}

template<class LL>
bool EthernetClass<LL>::socketSendUDP(uint8_t s)
        {
    SPI<LL> .beginTransaction();
    W5100<LL> .execCmdSn(s, Sock_SEND);

    /* +2008.01 bj */
    while ((W5100<LL> .readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK) {
        if (W5100<LL> .readSnIR(s) & SnIR::TIMEOUT) {
            /* +2008.01 [bj]: clear interrupt */
            W5100<LL> .writeSnIR(s, (SnIR::SEND_OK | SnIR::TIMEOUT));
            SPI<LL> .endTransaction();
            //Serial.printf("sendUDP timeout\n");
            return false;
        }
        SPI<LL> .endTransaction();
        yield();
        SPI<LL> .beginTransaction();
    }

    /* +2008.01 bj */
    W5100<LL> .writeSnIR(s, SnIR::SEND_OK);
    SPI<LL> .endTransaction();

    //Serial.printf("sendUDP ok\n");
    /* Sent ok */
    return true;
}

#include "../../../wiznet-udp/lowlevel/src/EthernetServer.h"
