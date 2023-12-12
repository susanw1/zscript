// This file is in the public domain.  No copyright is claimed.

#include "Ethernet.h"
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
template<class LL>
class EthernetClient: public Client {
public:
    EthernetClient() :
            sockindex(MAX_SOCK_NUM), _timeout(1000) {
    }
    EthernetClient(uint8_t s) :
            sockindex(s), _timeout(1000) {
    }

    uint8_t status();
    virtual int connect(IPAddress ip, uint16_t port);
    virtual int connect(const char *host, uint16_t port);
    virtual int availableForWrite(void);
    virtual uint16_t write(uint8_t);
    virtual uint16_t write(const uint8_t *buf, uint16_t size);
    virtual int available();
    virtual int read();
    virtual int read(uint8_t *buf, uint16_t size);
    virtual int peek();
    virtual void flush();
    virtual void stop();
    virtual uint8_t connected();
    virtual operator bool() {
        return sockindex < MAX_SOCK_NUM;
    }
    virtual bool operator==(const bool value) {
        return bool() == value;
    }
    virtual bool operator!=(const bool value) {
        return bool() != value;
    }
    virtual bool operator==(const EthernetClient&);

    virtual bool operator!=(const EthernetClient &rhs) {
        return !this->operator==(rhs);
    }
    uint8_t getSocketNumber() const {
        return sockindex;
    }
    virtual uint16_t localPort();
    virtual IPAddress remoteIP();
    virtual uint16_t remotePort();
    virtual void setConnectionTimeout(uint16_t timeout) {
        _timeout = timeout;
    }

    friend class EthernetServer<LL> ;

private:
    uint8_t sockindex; // MAX_SOCK_NUM means client not in use
    uint16_t _timeout;
};

#include "Ethernet.h"
#include "Client.h"
#include <clock-ll/SystemMilliClock.hpp>
#include "Dns.h"
#include "utility/w5100.h"

template<class LL>
int EthernetClient<LL>::connect(const char *host, uint16_t port)
        {
    DNSClient<LL> dns; // Look up the host first
    IPAddress remote_addr;

    if (sockindex < MAX_SOCK_NUM) {
        if (Ethernet<LL> .socketStatus(sockindex) != SnSR::CLOSED) {
            Ethernet<LL> .socketDisconnect(sockindex); // TODO: should we call stop()?
        }
        sockindex = MAX_SOCK_NUM;
    }
    dns.begin(Ethernet<LL> .dnsServerIP());
    if (!dns.getHostByName(host, remote_addr))
        return 0; // TODO: use _timeout
    return connect(remote_addr, port);
}

template<class LL>
int EthernetClient<LL>::connect(IPAddress ip, uint16_t port)
        {
    if (sockindex < MAX_SOCK_NUM) {
        if (Ethernet<LL> .socketStatus(sockindex) != SnSR::CLOSED) {
            Ethernet<LL> .socketDisconnect(sockindex); // TODO: should we call stop()?
        }
        sockindex = MAX_SOCK_NUM;
    }
#if defined(ESP8266) || defined(ESP32)
    if (ip == IPAddress((uint32_t)0) || ip == IPAddress(0xFFFFFFFFul)) return 0;
#else
    if (ip == IPAddress(0ul) || ip == IPAddress(0xFFFFFFFFul))
        return 0;
#endif
    sockindex = Ethernet<LL> .socketBegin(SnMR::TCP, 0);
    if (sockindex >= MAX_SOCK_NUM)
        return 0;
    Ethernet<LL> .socketConnect(sockindex, rawIPAddress(ip), port);
    uint32_t start = SystemMilliClock<LL>::getTimeMillis();
    while (1) {
        uint8_t stat = Ethernet<LL> .socketStatus(sockindex);
        if (stat == SnSR::ESTABLISHED)
            return 1;
        if (stat == SnSR::CLOSE_WAIT)
            return 1;
        if (stat == SnSR::CLOSED)
            return 0;
        if (SystemMilliClock<LL>::getTimeMillis() - start > _timeout)
            break;
        SystemMilliClock<LL>::blockDelayMillis(1);
    }
    Ethernet<LL> .socketClose(sockindex);
    sockindex = MAX_SOCK_NUM;
    return 0;
}

template<class LL>
int EthernetClient<LL>::availableForWrite(void)
        {
    if (sockindex >= MAX_SOCK_NUM)
        return 0;
    return Ethernet<LL> .socketSendAvailable(sockindex);
}

template<class LL>
uint16_t EthernetClient<LL>::write(uint8_t b)
        {
    return write(&b, 1);
}

template<class LL>
uint16_t EthernetClient<LL>::write(const uint8_t *buf, uint16_t size)
        {
    if (sockindex >= MAX_SOCK_NUM)
        return 0;
    if (Ethernet<LL> .socketSend(sockindex, buf, size))
        return size;
    return 0;
}

template<class LL>
int EthernetClient<LL>::available()
{
    if (sockindex >= MAX_SOCK_NUM)
        return 0;
    return Ethernet<LL> .socketRecvAvailable(sockindex);
    // TODO: do the Wiznet chips automatically retransmit TCP ACK
    // packets if they are lost by the network?  Someday this should
    // be checked by a man-in-the-middle test which discards certain
    // packets.  If ACKs aren't resent, we would need to check for
    // returning 0 here and after a timeout do another Sock_RECV
    // command to cause the Wiznet chip to resend the ACK packet.
}

template<class LL>
int EthernetClient<LL>::read(uint8_t *buf, uint16_t size)
        {
    if (sockindex >= MAX_SOCK_NUM)
        return 0;
    return Ethernet<LL> .socketRecv(sockindex, buf, size);
}

template<class LL>
int EthernetClient<LL>::peek()
{
    if (sockindex >= MAX_SOCK_NUM)
        return -1;
    if (!available())
        return -1;
    return Ethernet<LL> .socketPeek(sockindex);
}

template<class LL>
int EthernetClient<LL>::read()
{
    uint8_t b;
    if (Ethernet<LL> .socketRecv(sockindex, &b, 1) > 0)
        return b;
    return -1;
}

template<class LL>
void EthernetClient<LL>::flush()
{
    while (sockindex < MAX_SOCK_NUM) {
        uint8_t stat = Ethernet<LL> .socketStatus(sockindex);
        if (stat != SnSR::ESTABLISHED && stat != SnSR::CLOSE_WAIT)
            return;
        if (Ethernet<LL> .socketSendAvailable(sockindex) >= W5100<LL> .SSIZE)
            return;
    }
}

template<class LL>
void EthernetClient<LL>::stop()
{
    if (sockindex >= MAX_SOCK_NUM)
        return;

    // attempt to close the connection gracefully (send a FIN to other side)
    Ethernet<LL> .socketDisconnect(sockindex);
    unsigned long start = SystemMilliClock<LL>::getTimeMillis();

    // wait up to a second for the connection to close
    do {
        if (Ethernet<LL> .socketStatus(sockindex) == SnSR::CLOSED) {
            sockindex = MAX_SOCK_NUM;
            return; // exit the loop
        }
        SystemMilliClock<LL>::blockDelayMillis(1);
    } while (SystemMilliClock<LL>::getTimeMillis() - start < _timeout);

    // if it hasn't closed, close it forcefully
    Ethernet<LL> .socketClose(sockindex);
    sockindex = MAX_SOCK_NUM;
}

template<class LL>
uint8_t EthernetClient<LL>::connected()
{
    if (sockindex >= MAX_SOCK_NUM)
        return 0;

    uint8_t s = Ethernet<LL> .socketStatus(sockindex);
    return !(s == SnSR::LISTEN || s == SnSR::CLOSED || s == SnSR::FIN_WAIT ||
            (s == SnSR::CLOSE_WAIT && !available()));
}

template<class LL>
uint8_t EthernetClient<LL>::status()
{
    if (sockindex >= MAX_SOCK_NUM)
        return SnSR::CLOSED;
    return Ethernet<LL> .socketStatus(sockindex);
}

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.
template<class LL>
bool EthernetClient<LL>::operator==(const EthernetClient &rhs)
        {
    if (sockindex != rhs.sockindex)
        return false;
    if (sockindex >= MAX_SOCK_NUM)
        return false;
    if (rhs.sockindex >= MAX_SOCK_NUM)
        return false;
    return true;
}

// https://github.com/per1234/EthernetMod
// from: https://github.com/ntruchsess/Arduino-1/commit/937bce1a0bb2567f6d03b15df79525569377dabd
template<class LL>
uint16_t EthernetClient<LL>::localPort()
{
    if (sockindex >= MAX_SOCK_NUM)
        return 0;
    uint16_t port;
    SPI<LL> .beginTransaction();
    port = W5100<LL> .readSnPORT(sockindex);
    SPI<LL> .endTransaction();
    return port;
}

// https://github.com/per1234/EthernetMod
// returns the remote IP address: http://forum.arduino.cc/index.php?topic=82416.0
template<class LL>
IPAddress EthernetClient<LL>::remoteIP()
{
    if (sockindex >= MAX_SOCK_NUM)
        return IPAddress((uint32_t) 0);
    uint8_t remoteIParray[4];
    SPI<LL> .beginTransaction();
    W5100<LL> .readSnDIPR(sockindex, remoteIParray);
    SPI<LL> .endTransaction();
    return IPAddress(remoteIParray);
}

// https://github.com/per1234/EthernetMod
// from: https://github.com/ntruchsess/Arduino-1/commit/ca37de4ba4ecbdb941f14ac1fe7dd40f3008af75
template<class LL>
uint16_t EthernetClient<LL>::remotePort()
{
    if (sockindex >= MAX_SOCK_NUM)
        return 0;
    uint16_t port;
    SPI<LL> .beginTransaction();
    port = W5100<LL> .readSnDPORT(sockindex);
    SPI<LL> .endTransaction();
    return port;
}

