// DHCP Library v0.3 - April 25, 2009
// Author: Jordan Terrell - blog.jordanterrell.com
#include <string.h>
#include <stdlib.h>
#include "UipEthernet.h"
#include "DhcpClient.h"
#include "utility/util.h"

/**
 * @brief
 * @note
 * @param
 * @retval
 */
int DhcpClient::begin(uint8_t* mac, unsigned long timeout, unsigned long responseTimeout)
{
    _dhcpLeaseTime = 0;
    _dhcpT1 = 0;
    _dhcpT2 = 0;
    _lastCheck = 0;
    _timeout = timeout;
    _responseTimeout = responseTimeout;

    // zero out _dhcpMacAddr
    memset(_dhcpMacAddr, 0, 6);
    resetDhcpLease();

    memcpy((void*)_dhcpMacAddr, (void*)mac, 6);
    _dhcp_state = STATE_DHCP_START;
    return requestDhcpLease();
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
void DhcpClient::resetDhcpLease()
{
    // zero out _dhcpSubnetMask, _dhcpGatewayIp, _dhcpLocalIp, _dhcpDhcpServerIp, _dhcpDnsServerIp
    memset(_dhcpLocalIp, 0, 5 * 4);
}

//return:0 on error, 1 if request is sent and response is received
int DhcpClient::requestDhcpLease()
{
#ifdef UIPETHERNET_DEBUG_UDP
    printf("requestDhcpLease(): begin\r\n");
#endif
    uint8_t messageType = 0;
    Timer           timer;

    timer.reset();
    timer.start();

    // Pick an initial transaction ID
    srand(time(NULL)+ 1);
    _dhcpTransactionId = (rand() % 2000UL) + 1;
    _dhcpInitialTransactionId = _dhcpTransactionId;

    _dhcpUdpSocket.stop();
    if (_dhcpUdpSocket.begin(DHCP_CLIENT_PORT) == 0) {
        // Couldn't get a socket
#ifdef UIPETHERNET_DEBUG_UDP
        printf("requestDhcpLease(): Couldn't get a socket\r\n");
#endif
        return 0;
    }

    presendDhcp();

    volatile int    result = 0;

    while (_dhcp_state != STATE_DHCP_LEASED) {
        if (_dhcp_state == STATE_DHCP_START) {
#ifdef UIPETHERNET_DEBUG_UDP
            printf("_dhcp_state: STATE_DHCP_START\r\n");
#endif
            _dhcpTransactionId++;

            sendDhcpMessage(DHCP_DISCOVER, (timer.read_ms() / 1000 + 1));
            _dhcp_state = STATE_DHCP_DISCOVER;
        }
        else
        if (_dhcp_state == STATE_DHCP_REREQUEST) {
#ifdef UIPETHERNET_DEBUG_UDP
            printf("_dhcp_state: STATE_DHCP_REREQUEST\r\n");
#endif
            _dhcpTransactionId++;
            sendDhcpMessage(DHCP_REQUEST, (timer.read_ms() / 1000 + 1));
            _dhcp_state = STATE_DHCP_REQUEST;
        }
        else
        if (_dhcp_state == STATE_DHCP_DISCOVER) {
#ifdef UIPETHERNET_DEBUG_UDP
            printf("_dhcp_state: STATE_DHCP_DISCOVER\r\n");
#endif
            uint32_t    respId;
            messageType = parseDhcpResponse(_responseTimeout, respId);
            if (messageType == DHCP_OFFER) {
                // We'll use the transaction ID that the offer came with,
                // rather than the one we were up to
                _dhcpTransactionId = respId;
                sendDhcpMessage(DHCP_REQUEST, (timer.read_ms() / 1000 + 1));
                _dhcp_state = STATE_DHCP_REQUEST;
            }
        }
        else
        if (_dhcp_state == STATE_DHCP_REQUEST) {
#ifdef UIPETHERNET_DEBUG_UDP
            printf("_dhcp_state: STATE_DHCP_REQUEST\r\n");
#endif
            uint32_t    respId;
            messageType = parseDhcpResponse(_responseTimeout, respId);
            if (messageType == DHCP_ACK) {
#ifdef UIPETHERNET_DEBUG_UDP
                printf("messageType: DHCP_ACK\r\n");
#endif
                _dhcp_state = STATE_DHCP_LEASED;
                result = 1;

                //use default lease time if we didn't get it
                if (_dhcpLeaseTime == 0) {
                    _dhcpLeaseTime = DEFAULT_LEASE;
                }

                //calculate T1 & T2 if we didn't get it
                if (_dhcpT1 == 0) {
                    //T1 should be 50% of _dhcpLeaseTime
                    _dhcpT1 = _dhcpLeaseTime >> 1;
                }

                if (_dhcpT2 == 0) {
                    //T2 should be 87.5% (7/8ths) of _dhcpLeaseTime
                    _dhcpT2 = _dhcpT1 << 1;
                }

                _renewInSec = _dhcpT1;
                _rebindInSec = _dhcpT2;
            }
            else
            if (messageType == DHCP_NAK) {
 #ifdef UIPETHERNET_DEBUG_UDP
                printf("messageType: DHCP_NAK\r\n");
#endif
                _dhcp_state = STATE_DHCP_START;
            }
        }

        if (messageType == 255) {
#ifdef UIPETHERNET_DEBUG_UDP
            printf("DHCP_NAK: 255\r\n");
#endif
            messageType = 0;
            _dhcp_state = STATE_DHCP_START;
        }

        if ((result != 1) && ((timer.read_ms() / 1000) > _timeout)) {
#ifdef UIPETHERNET_DEBUG_UDP
            printf("requestDhcpLease(): timeout\r\n");
#endif
            break;
        }
    }

    // We're done with the socket now
    _dhcpUdpSocket.stop();
    _dhcpTransactionId++;

#ifdef UIPETHERNET_DEBUG_UDP
    printf("requestDhcpLease(): %d\r\n", result);
#endif
    return result;
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
void DhcpClient::presendDhcp()
{ }

/**
 * @brief
 * @note
 * @param
 * @retval
 */
void DhcpClient::sendDhcpMessage(uint8_t messageType, uint16_t secondsElapsed)
{
    uint8_t buffer[32];
    memset(buffer, 0, 32);

    IpAddress   dest_addr(255, 255, 255, 255);  // Broadcast address

    if (-1 == _dhcpUdpSocket.beginPacket(dest_addr, DHCP_SERVER_PORT)) {
        // FIXME Need to return errors
        return;
    }

    buffer[0] = DHCP_BOOTREQUEST;               // op
    buffer[1] = DHCP_HTYPE10MB;                 // htype
    buffer[2] = DHCP_HLENETHERNET;              // hlen
    buffer[3] = DHCP_HOPS;                      // hops
    // xid
    unsigned long   xid = htonl(_dhcpTransactionId);
    memcpy(buffer + 4, &(xid), 4);

    // 8, 9 - seconds elapsed
    buffer[8] = ((secondsElapsed & 0xff00) >> 8);
    buffer[9] = (secondsElapsed & 0x00ff);

    // flags
    unsigned short  flags = htons(DHCP_FLAGSBROADCAST);
    memcpy(buffer + 10, &(flags), 2);

    // ciaddr: already zeroed
    // yiaddr: already zeroed
    // siaddr: already zeroed
    // giaddr: already zeroed
    //put data in ENC28J60 transmit buffer
    _dhcpUdpSocket.write(buffer, 28);

    memset(buffer, 0, 32);                      // clear local buffer
    memcpy(buffer, _dhcpMacAddr, 6);            // chaddr
    //put data in ENC28J60 transmit buffer
    _dhcpUdpSocket.write(buffer, 16);

    memset(buffer, 0, 32);                      // clear local buffer
    // leave zeroed out for sname && file
    // put in ENC28J60 transmit buffer x 6 (192 bytes)
    for (int i = 0; i < 6; i++) {
        _dhcpUdpSocket.write(buffer, 32);
    }

    // OPT - Magic Cookie
    buffer[0] = (uint8_t) ((MAGIC_COOKIE >> 24) & 0xFF);
    buffer[1] = (uint8_t) ((MAGIC_COOKIE >> 16) & 0xFF);
    buffer[2] = (uint8_t) ((MAGIC_COOKIE >> 8) & 0xFF);
    buffer[3] = (uint8_t) (MAGIC_COOKIE & 0xFF);

    // OPT - message type
    buffer[4] = dhcpMessageType;
    buffer[5] = 0x01;
    buffer[6] = messageType;                    //DHCP_REQUEST;
    // OPT - client identifier
    buffer[7] = dhcpClientIdentifier;
    buffer[8] = 0x07;
    buffer[9] = 0x01;
    memcpy(buffer + 10, _dhcpMacAddr, 6);

    // OPT - host name
    buffer[16] = hostName;
    buffer[17] = strlen(HOST_NAME) + 6;         // length of hostname + last 3 bytes of mac address
    strcpy((char*) &(buffer[18]), HOST_NAME);

    printByte((char*) &(buffer[24]), _dhcpMacAddr[3]);
    printByte((char*) &(buffer[26]), _dhcpMacAddr[4]);
    printByte((char*) &(buffer[28]), _dhcpMacAddr[5]);

    //put data in ENC28J60 transmit buffer
    _dhcpUdpSocket.write(buffer, 30);

    if (messageType == DHCP_REQUEST) {
        buffer[0] = dhcpRequestedIPaddr;
        buffer[1] = 0x04;
        buffer[2] = _dhcpLocalIp[0];
        buffer[3] = _dhcpLocalIp[1];
        buffer[4] = _dhcpLocalIp[2];
        buffer[5] = _dhcpLocalIp[3];

        buffer[6] = dhcpServerIdentifier;
        buffer[7] = 0x04;

        //buffer[8] = _dhcpDhcpServerIp[0];
        buffer[8] = _dhcpLocalIp[0];
        buffer[9] = _dhcpDhcpServerIp[1];
        buffer[10] = _dhcpDhcpServerIp[2];
        buffer[11] = _dhcpDhcpServerIp[3];

        //put data in ENC28J60 transmit buffer
        _dhcpUdpSocket.write(buffer, 12);
    }

    buffer[0] = dhcpParamRequest;
    buffer[1] = 0x06;
    buffer[2] = subnetMask;
    buffer[3] = routersOnSubnet;
    buffer[4] = dns;
    buffer[5] = domainName;
    buffer[6] = dhcpT1value;
    buffer[7] = dhcpT2value;
    buffer[8] = endOption;

    //put data in ENC28J60 transmit buffer
    _dhcpUdpSocket.write(buffer, 9);

    _dhcpUdpSocket.endPacket();
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
uint8_t DhcpClient::parseDhcpResponse(unsigned long responseTimeout, uint32_t& transactionId)
{
    volatile uint8_t    type = 0;
    uint8_t             opt_len = 0;
    Timer               timer;

    timer.start();

    while (_dhcpUdpSocket.parsePacket() <= 0) {
        if (timer.read() > responseTimeout) {
            return 255;
        }

        wait_us(50000);
    }

    // start reading in the packet
    RIP_MSG_FIXED   fixedMsg;
    _dhcpUdpSocket.read((uint8_t*) &fixedMsg, sizeof(RIP_MSG_FIXED));

    if (fixedMsg.op == DHCP_BOOTREPLY && _dhcpUdpSocket.remotePort() == DHCP_SERVER_PORT) {
        transactionId = ntohl(fixedMsg.xid);
        if
        (
            memcmp(fixedMsg.chaddr, _dhcpMacAddr, 6) != 0 ||
            (transactionId < _dhcpInitialTransactionId) ||
            (transactionId > _dhcpTransactionId)
        ) {
            // Need to read the rest of the packet here regardless
            _dhcpUdpSocket.flush();
            return 0;
        }

        memcpy(_dhcpLocalIp, fixedMsg.yiaddr, 4);

        // Skip to the option part
        // Doing this a byte at a time so we don't have to put a big buffer
        // on the stack (as we don't have lots of memory lying around)
        for (int i = 0; i < (240 - (int)sizeof(RIP_MSG_FIXED)); i++) {
            _dhcpUdpSocket.read();  // we don't care about the returned byte
        }

        while (_dhcpUdpSocket.available() > 0) {
            switch (_dhcpUdpSocket.read()) {
                case endOption:
                    break;

                case padOption:
                    break;

                case dhcpMessageType:
                    opt_len = _dhcpUdpSocket.read();
                    type = _dhcpUdpSocket.read();
                    break;

                case subnetMask:
                    opt_len = _dhcpUdpSocket.read();
                    _dhcpUdpSocket.read(_dhcpSubnetMask, 4);
                    break;

                case routersOnSubnet:
                    opt_len = _dhcpUdpSocket.read();
                    _dhcpUdpSocket.read(_dhcpGatewayIp, 4);
                    for (int i = 0; i < opt_len - 4; i++) {
                        _dhcpUdpSocket.read();
                    }
                    break;

                case dns:
                    opt_len = _dhcpUdpSocket.read();
                    _dhcpUdpSocket.read(_dhcpDnsServerIp, 4);
                    for (int i = 0; i < opt_len - 4; i++) {
                        _dhcpUdpSocket.read();
                    }
                    break;

                case dhcpServerIdentifier:
                    opt_len = _dhcpUdpSocket.read();
                    if (*((uint32_t*)_dhcpDhcpServerIp) == 0 || IpAddress(_dhcpDhcpServerIp) == _dhcpUdpSocket.remoteIP()) {
                        _dhcpUdpSocket.read(_dhcpDhcpServerIp, sizeof(_dhcpDhcpServerIp));
                    }
                    else {
                        // Skip over the rest of this option
                        while (opt_len--) {
                            _dhcpUdpSocket.read();
                        }
                    }
                    break;

                case dhcpT1value:
                    opt_len = _dhcpUdpSocket.read();
                    _dhcpUdpSocket.read((uint8_t*) &_dhcpT1, sizeof(_dhcpT1));
                    _dhcpT1 = ntohl(_dhcpT1);
                    break;

                case dhcpT2value:
                    opt_len = _dhcpUdpSocket.read();
                    _dhcpUdpSocket.read((uint8_t*) &_dhcpT2, sizeof(_dhcpT2));
                    _dhcpT2 = ntohl(_dhcpT2);
                    break;

                case dhcpIPaddrLeaseTime:
                    opt_len = _dhcpUdpSocket.read();
                    _dhcpUdpSocket.read((uint8_t*) &_dhcpLeaseTime, sizeof(_dhcpLeaseTime));
                    _dhcpLeaseTime = ntohl(_dhcpLeaseTime);
                    _renewInSec = _dhcpLeaseTime;
                    break;

                default:
                    opt_len = _dhcpUdpSocket.read();

                    // Skip over the rest of this option
                    while (opt_len--) {
                        _dhcpUdpSocket.read();
                    }
                    break;
            }
        }
    }

    // Need to skip to end of the packet regardless here
    _dhcpUdpSocket.flush();

    return type;
}

/*
    returns:
    0/DHCP_CHECK_NONE: nothing happened
    1/DHCP_CHECK_RENEW_FAIL: renew failed
    2/DHCP_CHECK_RENEW_OK: renew success
    3/DHCP_CHECK_REBIND_FAIL: rebind fail
    4/DHCP_CHECK_REBIND_OK: rebind success
*/
int DhcpClient::checkLease()
{
    time_t          now = time(NULL);
    volatile int    rc = DHCP_CHECK_NONE;
    if (_lastCheck != 0) {
        //calc how many ms past the timeout we are
        time_t  factor = now - _secTimeout;

        //if on or passed the timeout, reduce the counters
        if (factor >= 0) {
            //next timeout should be now plus 1s
            _secTimeout = now + 1;

            //reduce the counters by that mouch
            //if we can assume that the cycle time (factor) is fairly constant
            //and if the remainder is less than cycle time * 2
            //do it early instead of late
            if (_renewInSec < factor * 2)
                _renewInSec = 0;
            else
                _renewInSec -= factor;

            if (_rebindInSec < factor * 2)
                _rebindInSec = 0;
            else
                _rebindInSec -= factor;
        }

        //if we have a lease but should renew, do it
        if (_dhcp_state == STATE_DHCP_LEASED && _renewInSec <= 0) {
            _dhcp_state = STATE_DHCP_REREQUEST;
            rc = 1 + requestDhcpLease();
        }

        //if we have a lease or is renewing but should bind, do it
        if ((_dhcp_state == STATE_DHCP_LEASED || _dhcp_state == STATE_DHCP_START) && _rebindInSec <= 0) {
            //this should basically restart completely
            _dhcp_state = STATE_DHCP_START;
            resetDhcpLease();
            rc = 3 + requestDhcpLease();
        }
    }
    else {
        _secTimeout = now + 1;
    }

    _lastCheck = now;
    return rc;
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
IpAddress DhcpClient::getLocalIp()
{
    return IpAddress(_dhcpLocalIp);
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
IpAddress DhcpClient::getSubnetMask()
{
    return IpAddress(_dhcpSubnetMask);
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
IpAddress DhcpClient::getGatewayIp()
{
    return IpAddress(_dhcpGatewayIp);
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
IpAddress DhcpClient::getDhcpServerIp()
{
    return IpAddress(_dhcpDhcpServerIp);
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
IpAddress DhcpClient::getDnsServerIp()
{
    return IpAddress(_dhcpDnsServerIp);
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
void DhcpClient::printByte(char* buf, uint8_t n)
{
    char*   str = &buf[1];
    buf[0] = '0';
    do {
        unsigned long   m = n;
        n /= 16;

        char    c = m - 16 * n;
        *str-- = c < 10 ? c + '0' : c + 'A' - 10;
    } while (n);
}
