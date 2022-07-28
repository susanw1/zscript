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

#ifndef ethernet_h_
#define ethernet_h_

// All symbols exposed to Arduino sketches are contained in this header file
//
// Older versions had much of this stuff in EthernetClient.h, EthernetServer.h,
// and socket.h.  Including headers in different order could cause trouble, so
// these "friend" classes are now defined in the same header file.  socket.h
// was removed to avoid possible conflict with the C library header files.

// Configure the maximum number of sockets to support.  W5100 chips can have
// up to 4 sockets.  W5200 & W5500 can have up to 8 sockets.  Several bytes
// of RAM are used for each socket.  Reducing the maximum can save RAM, but
// you are limited to fewer simultaneous connections.
#define MAX_SOCK_NUM 4

// By default, each socket uses 2K buffers inside the Wiznet chip.  If
// MAX_SOCK_NUM is set to fewer than the chip's maximum, uncommenting
// this will use larger buffers within the Wiznet chip.  Large buffers
// can really help with UDP protocols like Artnet.  In theory larger
// buffers should allow faster TCP over high-latency links, but this
// does not always seem to work in practice (maybe Wiznet bugs?)
//#define ETHERNET_LARGE_BUFFERS

#include "Client.h"
#include "Server.h"
#include "Udp.h"
#include "../SPI.h"
#include "utility/w5100.h"

enum EthernetLinkStatus {
    Unknown,
    LinkON,
    LinkOFF
};

enum EthernetHardwareStatus {
    EthernetNoHardware,
    EthernetW5100,
    EthernetW5200,
    EthernetW5500
};

template<class LL>
class EthernetUDP;
template<class LL>
class EthernetClient;
template<class LL>
class EthernetServer;
template<class LL>
class DhcpClass;

template<class LL>
class EthernetClass {
private:
    static IPAddress _dnsServerAddress;
    static DhcpClass<LL> *_dhcp;
    static DhcpClass<LL> s_dhcp;
    public:
    // Initialise the Ethernet shield to use the provided MAC address and
    // gain the rest of the configuration through DHCP.
    // Returns 0 if the DHCP configuration failed, and 1 if it succeeded
    static int begin(uint8_t *mac, unsigned long timeout = 60000, unsigned long responseTimeout = 4000);
    static int maintain();
    static EthernetLinkStatus linkStatus();
    static EthernetHardwareStatus hardwareStatus();

    // Manaul configuration
    static void begin(uint8_t *mac, IPAddress ip);
    static void begin(uint8_t *mac, IPAddress ip, IPAddress dns);
    static void begin(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway);
    static void begin(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet);
    static void init();

    static void MACAddress(uint8_t *mac_address);
    static IPAddress localIP();
    static IPAddress subnetMask();
    static IPAddress gatewayIP();
    static IPAddress dnsServerIP() {
        return _dnsServerAddress;
    }

    void setMACAddress(const uint8_t *mac_address);
    void setLocalIP(const IPAddress local_ip);
    void setSubnetMask(const IPAddress subnet);
    void setGatewayIP(const IPAddress gateway);
    void setDnsServerIP(const IPAddress dns_server) {
        _dnsServerAddress = dns_server;
    }
    void setRetransmissionTimeout(uint16_t milliseconds);
    void setRetransmissionCount(uint8_t num);

    friend class EthernetClient<LL> ;
    friend class EthernetServer<LL> ;
    friend class EthernetUDP<LL> ;
    private:
    // Opens a socket(TCP or UDP or IP_RAW mode)
    static uint8_t socketBegin(uint8_t protocol, uint16_t port);
    static uint8_t socketBeginMulticast(uint8_t protocol, IPAddress ip, uint16_t port);
    static uint8_t socketStatus(uint8_t s);
    // Close socket
    static void socketClose(uint8_t s);
    // Establish TCP connection (Active connection)
    static void socketConnect(uint8_t s, uint8_t *addr, uint16_t port);
    // disconnect the connection
    static void socketDisconnect(uint8_t s);
    // Establish TCP connection (Passive connection)
    static uint8_t socketListen(uint8_t s);
    // Send data (TCP)
    static uint16_t socketSend(uint8_t s, const uint8_t *buf, uint16_t len);
    static uint16_t socketSendAvailable(uint8_t s);
    // Receive data (TCP)
    static int socketRecv(uint8_t s, uint8_t *buf, int16_t len);
    static uint16_t socketRecvAvailable(uint8_t s);
    static uint8_t socketPeek(uint8_t s);
    static uint8_t socketPeek(uint8_t s, uint16_t dist);
    // sets up a UDP datagram, the data for which will be provided by one
    // or more calls to bufferData and then finally sent with sendUDP.
    // return true if the datagram was successfully set up, or false if there was an error
    static bool socketStartUDP(uint8_t s, uint8_t *addr, uint16_t port);
    // copy up to len bytes of data from buf into a UDP datagram to be
    // sent later by sendUDP.  Allows datagrams to be built up from a series of bufferData calls.
    // return Number of bytes successfully buffered
    static uint16_t socketBufferData(uint8_t s, uint16_t offset, const uint8_t *buf, uint16_t len);
    // Send a UDP datagram built up from a sequence of startUDP followed by one or more
    // calls to bufferData.
    // return true if the datagram was successfully sent, or false if there was an error
    static bool socketSendUDP(uint8_t s);
    // Initialize the "random" source port number
    static void socketPortRand(uint16_t n);
};

template<class LL>
extern EthernetClass<LL> Ethernet;

#define UDP_TX_PACKET_MAX_SIZE 24

#include "socket.h"
#include "Dhcp.h"
#include "Dns.h"
#include "EthernetUdp.h"
#include "Ethernetcpp.h"
#endif
