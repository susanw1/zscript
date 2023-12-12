// This file is in the public domain.  No copyright is claimed.

#include "Ethernet.h"
#include "Ethernet.h"
#include "EthernetClient.h"
#include "utility/w5100.h"

template<class LL>
class EthernetServer: public Server {
private:
    uint16_t _port;
    public:
    EthernetServer(uint16_t port) :
            _port(port) {
    }
    EthernetClient<LL> available();
    EthernetClient<LL> accept();
    virtual void begin();
    virtual uint16_t write(uint8_t);
    virtual uint16_t write(const uint8_t *buf, uint16_t size);
    virtual operator bool();
    //void statusreport();

    // TODO: make private when socket allocation moves to EthernetClass
    static uint16_t server_port[MAX_SOCK_NUM];
};

template<class LL>
uint16_t EthernetServer<LL>::server_port[MAX_SOCK_NUM];

template<class LL>
void EthernetServer<LL>::begin()
{
    uint8_t sockindex = Ethernet<LL> .socketBegin(SnMR::TCP, _port);
    if (sockindex < MAX_SOCK_NUM) {
        if (Ethernet<LL> .socketListen(sockindex)) {
            server_port[sockindex] = _port;
        } else {
            Ethernet<LL> .socketDisconnect(sockindex);
        }
    }
}

template<class LL>
EthernetClient<LL> EthernetServer<LL>::available()
{
    bool listening = false;
    uint8_t sockindex = MAX_SOCK_NUM;
    uint8_t chip, maxindex = MAX_SOCK_NUM;

    chip = W5100<LL> .getChip();
    if (!chip)
        return EthernetClient<LL>(MAX_SOCK_NUM);
#if MAX_SOCK_NUM > 4
    if (chip == 51)
        maxindex = 4; // W5100 chip never supports more than 4 sockets
#endif
    for (uint8_t i = 0; i < maxindex; i++) {
        if (server_port[i] == _port) {
            uint8_t stat = Ethernet<LL> .socketStatus(i);
            if (stat == SnSR::ESTABLISHED || stat == SnSR::CLOSE_WAIT) {
                if (Ethernet<LL> .socketRecvAvailable(i) > 0) {
                    sockindex = i;
                } else {
                    // remote host closed connection, our end still open
                    if (stat == SnSR::CLOSE_WAIT) {
                        Ethernet<LL> .socketDisconnect(i);
                        // status becomes LAST_ACK for short time
                    }
                }
            } else if (stat == SnSR::LISTEN) {
                listening = true;
            } else if (stat == SnSR::CLOSED) {
                server_port[i] = 0;
            }
        }
    }
    if (!listening)
        begin();
    return EthernetClient<LL>(sockindex);
}

template<class LL>
EthernetClient<LL> EthernetServer<LL>::accept()
{
    bool listening = false;
    uint8_t sockindex = MAX_SOCK_NUM;
    uint8_t chip, maxindex = MAX_SOCK_NUM;

    chip = W5100<LL> .getChip();
    if (!chip)
        return EthernetClient<LL>(MAX_SOCK_NUM);
#if MAX_SOCK_NUM > 4
    if (chip == 51)
        maxindex = 4; // W5100 chip never supports more than 4 sockets
#endif
    for (uint8_t i = 0; i < maxindex; i++) {
        if (server_port[i] == _port) {
            uint8_t stat = Ethernet<LL> .socketStatus(i);
            if (sockindex == MAX_SOCK_NUM &&
                    (stat == SnSR::ESTABLISHED || stat == SnSR::CLOSE_WAIT)) {
                // Return the connected client even if no data received.
                // Some protocols like FTP expect the server to send the
                // first data.
                sockindex = i;
                server_port[i] = 0; // only return the client once
            } else if (stat == SnSR::LISTEN) {
                listening = true;
            } else if (stat == SnSR::CLOSED) {
                server_port[i] = 0;
            }
        }
    }
    if (!listening)
        begin();
    return EthernetClient<LL>(sockindex);
}

template<class LL>
EthernetServer<LL>::operator bool()
{
    uint8_t maxindex = MAX_SOCK_NUM;
#if MAX_SOCK_NUM > 4
    if (W5100<LL> .getChip() == 51)
        maxindex = 4; // W5100 chip never supports more than 4 sockets
#endif
    for (uint8_t i = 0; i < maxindex; i++) {
        if (server_port[i] == _port) {
            if (Ethernet<LL> .socketStatus(i) == SnSR::LISTEN) {
                return true; // server is listening for incoming clients
            }
        }
    }
    return false;
}

#if 0
template<class LL>
void EthernetServer<LL>::statusreport()
{
  //Serial.printf("EthernetServer, port=%d\n", _port);
  for (uint8_t i=0; i < MAX_SOCK_NUM; i++) {
      uint16_t port = server_port[i];
      uint8_t stat = Ethernet<LL>.socketStatus(i);
      const char *name;
      switch (stat) {
          case 0x00: name = "CLOSED"; break;
          case 0x13: name = "INIT"; break;
          case 0x14: name = "LISTEN"; break;
          case 0x15: name = "SYNSENT"; break;
          case 0x16: name = "SYNRECV"; break;
          case 0x17: name = "ESTABLISHED"; break;
          case 0x18: name = "FIN_WAIT"; break;
          case 0x1A: name = "CLOSING"; break;
          case 0x1B: name = "TIME_WAIT"; break;
          case 0x1C: name = "CLOSE_WAIT"; break;
          case 0x1D: name = "LAST_ACK"; break;
          case 0x22: name = "UDP"; break;
          case 0x32: name = "IPRAW"; break;
          case 0x42: name = "MACRAW"; break;
          case 0x5F: name = "PPPOE"; break;
          default: name = "???";
      }
      int avail = Ethernet<LL>.socketRecvAvailable(i);
      //Serial.printf("  %d: port=%d, status=%s (0x%02X), avail=%d\n",
          i, port, name, stat, avail);
  }
}
#endif

template<class LL>
uint16_t EthernetServer<LL>::write(uint8_t b)
        {
    return write(&b, 1);
}

template<class LL>
uint16_t EthernetServer<LL>::write(const uint8_t *buffer, uint16_t size)
        {
    uint8_t chip, maxindex = MAX_SOCK_NUM;

    chip = W5100<LL> .getChip();
    if (!chip)
        return 0;
#if MAX_SOCK_NUM > 4
    if (chip == 51)
        maxindex = 4; // W5100 chip never supports more than 4 sockets
#endif
    available();
    for (uint8_t i = 0; i < maxindex; i++) {
        if (server_port[i] == _port) {
            if (Ethernet<LL> .socketStatus(i) == SnSR::ESTABLISHED) {
                Ethernet<LL> .socketSend(i, buffer, size);
            }
        }
    }
    return size;
}
