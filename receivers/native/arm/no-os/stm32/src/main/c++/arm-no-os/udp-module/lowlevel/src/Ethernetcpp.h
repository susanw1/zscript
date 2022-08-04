#ifndef SRC_MAIN_C___ARM_NO_OS_UDP_MODULE_LOWLEVEL_SRC_ETHERNETCPP_H_
#define SRC_MAIN_C___ARM_NO_OS_UDP_MODULE_LOWLEVEL_SRC_ETHERNETCPP_H_
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
#include <arm-no-os/system/clock/SystemMilliClock.hpp>
#include "Dhcp.h"

template<class LL>
IPAddress EthernetClass<LL>::_dnsServerAddress;
template<class LL>
DhcpClass<LL> *EthernetClass<LL>::_dhcp = NULL;
template<class LL>
DhcpClass<LL> EthernetClass<LL>::s_dhcp;

template<class LL>
int EthernetClass<LL>::begin(uint8_t *mac, unsigned long timeout, unsigned long responseTimeout)
        {
    _dhcp = &s_dhcp;

    // Initialise the basic info
    if (W5100<LL> .init() == 0)
        return 0;
    SPI<LL> .beginTransaction();
    W5100<LL> .setMACAddress(mac);
    W5100<LL> .setIPAddress(IPAddress(0, 0, 0, 0).raw_address());
    SPI<LL> .endTransaction();

    // Now try to get our config info from a DHCP server
    int ret = _dhcp->beginWithDHCP(mac, timeout, responseTimeout);
    if (ret == 1) {
        // We've successfully found a DHCP server and got our configuration
        // info, so set things accordingly
        SPI<LL> .beginTransaction();
        W5100<LL> .setIPAddress(_dhcp->getLocalIp().raw_address());
        W5100<LL> .setGatewayIp(_dhcp->getGatewayIp().raw_address());
        W5100<LL> .setSubnetMask(_dhcp->getSubnetMask().raw_address());
        SPI<LL> .endTransaction();
        _dnsServerAddress = _dhcp->getDnsServerIp();
        socketPortRand(SystemMilliClock<LL>::getTimeMillis());
    }
    return ret;
}

template<class LL>
void EthernetClass<LL>::begin(uint8_t *mac, IPAddress ip)
        {
    // Assume the DNS server will be the machine on the same network as the local IP
    // but with last octet being '1'
    IPAddress dns = ip;
    dns[3] = 1;
    begin(mac, ip, dns);
}

template<class LL>
void EthernetClass<LL>::begin(uint8_t *mac, IPAddress ip, IPAddress dns)
        {
    // Assume the gateway will be the machine on the same network as the local IP
    // but with last octet being '1'
    IPAddress gateway = ip;
    gateway[3] = 1;
    begin(mac, ip, dns, gateway);
}

template<class LL>
void EthernetClass<LL>::begin(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway)
        {
    IPAddress subnet(255, 255, 255, 0);
    begin(mac, ip, dns, gateway, subnet);
}

template<class LL>
void EthernetClass<LL>::begin(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet)
        {
    if (W5100<LL> .init() == 0)
        return;
    SPI<LL> .beginTransaction();
    W5100<LL> .setMACAddress(mac);
    W5100<LL> .setIPAddress(ip._address.bytes);
    W5100<LL> .setGatewayIp(gateway._address.bytes);
    W5100<LL> .setSubnetMask(subnet._address.bytes);
    SPI<LL> .endTransaction();
    _dnsServerAddress = dns;
}

template<class LL>
void EthernetClass<LL>::init()
{
}

template<class LL>
EthernetLinkStatus EthernetClass<LL>::linkStatus()
{
    switch (W5100<LL> .getLinkStatus()) {
    case UNKNOWN:
        return Unknown;
    case LINK_ON:
        return LinkON;
    case LINK_OFF:
        return LinkOFF;
    default:
        return Unknown;
    }
}

template<class LL>
EthernetHardwareStatus EthernetClass<LL>::hardwareStatus()
{
    switch (W5100<LL> .getChip()) {
    case 51:
        return EthernetW5100;
    case 52:
        return EthernetW5200;
    case 55:
        return EthernetW5500;
    default:
        return EthernetNoHardware;
    }
}

template<class LL>
int EthernetClass<LL>::maintain()
{
    int rc = DHCP_CHECK_NONE;
    if (_dhcp != NULL) {
        // we have a pointer to dhcp, use it
        rc = _dhcp->checkLease();
        switch (rc) {
        case DHCP_CHECK_NONE:
            //nothing done
            break;
        case DHCP_CHECK_RENEW_OK:
            case DHCP_CHECK_REBIND_OK:
            //we might have got a new IP.
            SPI<LL> .beginTransaction();
            W5100<LL> .setIPAddress(_dhcp->getLocalIp().raw_address());
            W5100<LL> .setGatewayIp(_dhcp->getGatewayIp().raw_address());
            W5100<LL> .setSubnetMask(_dhcp->getSubnetMask().raw_address());
            SPI<LL> .endTransaction();
            _dnsServerAddress = _dhcp->getDnsServerIp();
            break;
        default:
            //this is actually an error, it will retry though
            break;
        }
    }
    return rc;
}

template<class LL>
void EthernetClass<LL>::MACAddress(uint8_t *mac_address)
        {
    SPI<LL> .beginTransaction();
    W5100<LL> .getMACAddress(mac_address);
    SPI<LL> .endTransaction();
}

template<class LL>
IPAddress EthernetClass<LL>::localIP()
{
    IPAddress ret;
    SPI<LL> .beginTransaction();
    W5100<LL> .getIPAddress(ret.raw_address());
    SPI<LL> .endTransaction();
    return ret;
}

template<class LL>
IPAddress EthernetClass<LL>::subnetMask()
{
    IPAddress ret;
    SPI<LL> .beginTransaction();
    W5100<LL> .getSubnetMask(ret.raw_address());
    SPI<LL> .endTransaction();
    return ret;
}

template<class LL>
IPAddress EthernetClass<LL>::gatewayIP()
{
    IPAddress ret;
    SPI<LL> .beginTransaction();
    W5100<LL> .getGatewayIp(ret.raw_address());
    SPI<LL> .endTransaction();
    return ret;
}

template<class LL>
void EthernetClass<LL>::setMACAddress(const uint8_t *mac_address)
        {
    SPI<LL> .beginTransaction();
    W5100<LL> .setMACAddress(mac_address);
    SPI<LL> .endTransaction();
}

template<class LL>
void EthernetClass<LL>::setLocalIP(const IPAddress local_ip)
        {
    SPI<LL> .beginTransaction();
    IPAddress ip = local_ip;
    W5100<LL> .setIPAddress(ip.raw_address());
    SPI<LL> .endTransaction();
}

template<class LL>
void EthernetClass<LL>::setSubnetMask(const IPAddress subnet)
        {
    SPI<LL> .beginTransaction();
    IPAddress ip = subnet;
    W5100<LL> .setSubnetMask(ip.raw_address());
    SPI<LL> .endTransaction();
}

template<class LL>
void EthernetClass<LL>::setGatewayIP(const IPAddress gateway)
        {
    SPI<LL> .beginTransaction();
    IPAddress ip = gateway;
    W5100<LL> .setGatewayIp(ip.raw_address());
    SPI<LL> .endTransaction();
}

template<class LL>
void EthernetClass<LL>::setRetransmissionTimeout(uint16_t milliseconds)
        {
    if (milliseconds > 6553)
        milliseconds = 6553;
    SPI<LL> .beginTransaction();
    W5100<LL> .setRetransmissionTime(milliseconds * 10);
    SPI<LL> .endTransaction();
}

template<class LL>
void EthernetClass<LL>::setRetransmissionCount(uint8_t num)
        {
    SPI<LL> .beginTransaction();
    W5100<LL> .setRetransmissionCount(num);
    SPI<LL> .endTransaction();
}

template<class LL>
EthernetClass<LL> Ethernet;

#endif /* SRC_MAIN_C___ARM_NO_OS_UDP_MODULE_LOWLEVEL_SRC_ETHERNETCPP_H_ */
