/*
 *  Udp.cpp: Library to send/receive UDP packets with the Arduino ethernet shield.
 *  This version only offers minimal wrapping of socket.cpp
 *  Drop Udp.h/.cpp into the Ethernet library directory at hardware/libraries/Ethernet/
 *
 * NOTE: UDP is fast, but has some important limitations (thanks to Warren Gray for mentioning these)
 * 1) UDP does not guarantee the order in which assembled UDP packets are received. This
 * might not happen often in practice, but in larger network topologies, a UDP
 * packet can be received out of sequence.
 * 2) UDP does not guard against lost packets - so packets *can* disappear without the sender being
 * aware of it. Again, this may not be a concern in practice on small local networks.
 * For more information, see http://www.cafeaulait.org/course/week12/35.html
 *
 * MIT License:
 * Copyright (c) 2008 Bjoern Hartmann
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * bjoern@cs.stanford.edu 12/30/2008
 */

#include "Ethernet.h"

template<class LL>
class DNSClient;

template<class LL>
class EthernetUDP: public UDP {
private:
    uint16_t _port; // local port to listen on
    IPAddress _remoteIP; // remote IP address for the incoming packet whilst it's being processed
    uint16_t _remotePort; // remote port for the incoming packet whilst it's being processed
    uint16_t _offset; // offset into the packet being sent

protected:
    uint8_t sockindex;
    uint16_t _remaining; // remaining bytes of incoming packet yet to be processed

public:
    EthernetUDP() :
            sockindex(MAX_SOCK_NUM) {
    }  // Constructor
    virtual uint8_t begin(uint16_t);      // initialize, start listening on specified port. Returns 1 if successful, 0 if there are no sockets available to use
    virtual uint8_t beginMulticast(IPAddress, uint16_t); // initialize, start listening on specified port. Returns 1 if successful, 0 if there are no sockets available to use
    virtual void stop();  // Finish with the UDP socket

    // Sending UDP packets

    // Start building up a packet to send to the remote host specific in ip and port
    // Returns 1 if successful, 0 if there was a problem with the supplied IP address or port
    virtual int beginPacket(IPAddress ip, uint16_t port);
    // Start building up a packet to send to the remote host specific in host and port
    // Returns 1 if successful, 0 if there was a problem resolving the hostname or port
    virtual int beginPacket(const char *host, uint16_t port);
    // Finish off this packet and send it
    // Returns 1 if the packet was sent successfully, 0 if there was an error
    virtual int endPacket();
    // Write a single byte into the packet
    virtual uint16_t write(uint8_t);
    // Write size bytes from buffer into the packet
    virtual uint16_t write(const uint8_t *buffer, uint16_t size);

    // Start processing the next available incoming packet
    // Returns the size of the packet in bytes, or 0 if no packets are available
    virtual int parsePacket();
    // Number of bytes remaining in the current packet
    virtual int available();
    // Read a single byte from the current packet
    virtual int read();
    // Read up to len bytes from the current packet and place them into buffer
    // Returns the number of bytes read, or 0 if none are available
    virtual int read(unsigned char *buffer, uint16_t len);
    // Read up to len characters from the current packet and place them into buffer
    // Returns the number of characters read, or 0 if none are available
    virtual int read(char *buffer, uint16_t len) {
        return read((unsigned char*) buffer, len);
    }
    ;
    // Return the next byte from the current packet without moving on to the next byte
    virtual int peek();

    // Return the next byte from the current packet without moving on to the next byte
    virtual int peek(uint16_t dist);

    virtual void flush(); // Finish reading the current packet

    // Return the IP address of the host who sent the current incoming packet
    virtual IPAddress remoteIP() {
        return _remoteIP;
    }
    ;
    // Return the port of the host who sent the current incoming packet
    virtual uint16_t remotePort() {
        return _remotePort;
    }
    ;
    virtual uint16_t localPort() {
        return _port;
    }
};
/*
 *  Udp.cpp: Library to send/receive UDP packets with the Arduino ethernet shield.
 *  This version only offers minimal wrapping of socket.cpp
 *  Drop Udp.h/.cpp into the Ethernet library directory at hardware/libraries/Ethernet/
 *
 * MIT License:
 * Copyright (c) 2008 Bjoern Hartmann
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * bjoern@cs.stanford.edu 12/30/2008
 */

#include "Ethernet.h"
#include "Dns.h"
#include "utility/w5100.h"

/* Start EthernetUDP socket, listening at local port PORT */
template<class LL>
uint8_t EthernetUDP<LL>::begin(uint16_t port)
        {
    if (sockindex < MAX_SOCK_NUM)
        Ethernet<LL> .socketClose(sockindex);
    sockindex = Ethernet<LL> .socketBegin(SnMR::UDP, port);
    if (sockindex >= MAX_SOCK_NUM)
        return 0;
    _port = port;
    _remaining = 0;
    return 1;
}

/* return number of bytes available in the current packet,
 will return zero if parsePacket hasn't been called yet */
template<class LL>
int EthernetUDP<LL>::available()
{
    return _remaining;
}

/* Release any resources being used by this EthernetUDP instance */
template<class LL>
void EthernetUDP<LL>::stop()
{
    if (sockindex < MAX_SOCK_NUM) {
        Ethernet<LL> .socketClose(sockindex);
        sockindex = MAX_SOCK_NUM;
    }
}

template<class LL>
int EthernetUDP<LL>::beginPacket(const char *host, uint16_t port)
        {
    // Look up the host first
    int ret = 0;
    DNSClient<LL> dns;
    IPAddress remote_addr;

    dns.begin(Ethernet<LL> .dnsServerIP());
    ret = dns.getHostByName(host, remote_addr);
    if (ret != 1)
        return ret;
    return beginPacket(remote_addr, port);
}

template<class LL>
int EthernetUDP<LL>::beginPacket(IPAddress ip, uint16_t port)
        {
    _offset = 0;
    //Serial.printf("UDP beginPacket\n");
    return Ethernet<LL> .socketStartUDP(sockindex, rawIPAddress(ip), port);
}

template<class LL>
int EthernetUDP<LL>::endPacket()
{
    return Ethernet<LL> .socketSendUDP(sockindex);
}

template<class LL>
uint16_t EthernetUDP<LL>::write(uint8_t byte)
        {
    return write(&byte, 1);
}

template<class LL>
uint16_t EthernetUDP<LL>::write(const uint8_t *buffer, uint16_t size)
        {
    //Serial.printf("UDP write %d\n", size);
    uint16_t bytes_written = Ethernet<LL> .socketBufferData(sockindex, _offset, buffer, size);
    _offset += bytes_written;
    return bytes_written;
}

template<class LL>
int EthernetUDP<LL>::parsePacket()
{
    // discard any remaining bytes in the last packet
    while (_remaining) {
        // could this fail (loop endlessly) if _remaining > 0 and recv in read fails?
        // should only occur if recv fails after telling us the data is there, lets
        // hope the w5100 always behaves :)
        read((uint8_t*) NULL, _remaining);
    }

    if (Ethernet<LL> .socketRecvAvailable(sockindex) > 0) {
        //HACK - hand-parse the UDP packet using TCP recv method
        uint8_t tmpBuf[8];
        int ret = 0;
        //read 8 header bytes and get IP and port from it
        ret = Ethernet<LL> .socketRecv(sockindex, tmpBuf, 8);
        if (ret > 0) {
            _remoteIP = tmpBuf;
            _remotePort = tmpBuf[4];
            _remotePort = (_remotePort << 8) + tmpBuf[5];
            _remaining = tmpBuf[6];
            _remaining = (_remaining << 8) + tmpBuf[7];

            // When we get here, any remaining bytes are the data
            ret = _remaining;
        }
        return ret;
    }
    // There aren't any packets available
    return 0;
}

template<class LL>
int EthernetUDP<LL>::read()
{
    uint8_t byte;

    if ((_remaining > 0) && (Ethernet<LL> .socketRecv(sockindex, &byte, 1) > 0)) {
        // We read things without any problems
        _remaining--;
        return byte;
    }

    // If we get here, there's no data available
    return -1;
}

template<class LL>
int EthernetUDP<LL>::read(unsigned char *buffer, uint16_t len)
        {
    if (_remaining > 0) {
        int got;
        if (_remaining <= len) {
            // data should fit in the buffer
            got = Ethernet<LL> .socketRecv(sockindex, buffer, _remaining);
        } else {
            // too much data for the buffer,
            // grab as much as will fit
            got = Ethernet<LL> .socketRecv(sockindex, buffer, len);
        }
        if (got > 0) {
            _remaining -= got;
            //Serial.printf("UDP read %d\n", got);
            return got;
        }
    }
    // If we get here, there's no data available or recv failed
    return -1;
}

template<class LL>
int EthernetUDP<LL>::peek()
{
    // Unlike recv, peek doesn't check to see if there's any data available, so we must.
    // If the user hasn't called parsePacket yet then return nothing otherwise they
    // may get the UDP header
    if (sockindex >= MAX_SOCK_NUM || _remaining == 0)
        return -1;
    return Ethernet<LL> .socketPeek(sockindex);
}

template<class LL>
int EthernetUDP<LL>::peek(uint16_t dist) {
    if (sockindex >= MAX_SOCK_NUM || _remaining == 0)
        return -1;
    return Ethernet<LL> .socketPeek(sockindex, dist);
}

template<class LL>
void EthernetUDP<LL>::flush()
{
    // TODO: we should wait for TX buffer to be emptied
}

/* Start EthernetUDP socket, listening at local port PORT */
template<class LL>
uint8_t EthernetUDP<LL>::beginMulticast(IPAddress ip, uint16_t port)
        {
    if (sockindex < MAX_SOCK_NUM)
        Ethernet<LL> .socketClose(sockindex);
    sockindex = Ethernet<LL> .socketBeginMulticast(SnMR::UDP | SnMR::MULTI, ip, port);
    if (sockindex >= MAX_SOCK_NUM)
        return 0;
    _port = port;
    _remaining = 0;
    return 1;
}

