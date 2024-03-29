This is UIPEthernet for Mbed OS 2 and Mbed OS 5

Full support for persistent (streaming) TCP-connections and UDP (Client and Server each), ARP, ICMP, DHCP and DNS. 

UIPEthernet is written as a wrapper around the mature uIP Stack by Adam Dunkels, which provides the low-level implementation for all supported protocols.
To overcome the memory-constrains (a 'regular' uIP-application does all processing in RAM) the ENC28J60 internal memory is used for all stream buffers (in and out).

This library is written by Norbert Truchsess <norbert.truchsess@t-online.de>

uIP was written by Adam Dunkels of the Networked Embedded Systems group at the Swedish Institute of Computer Science.

This library was inspired by the SerialIP implementation by Adam Nielsen <malvineous@shikadi.net>, actually I took this code as a starting point, but in the latest versions there are very few lines left.

This library was ported to mbed by Zoltan Hudak <hudakz@outlook.com>

Additional information can be found on the Arduino website: http://www.arduino.cc/en/Hacking/Libraries

Documentation
-------------

For more information visit:

 - UIPEthernet Repository on github:
    https://github.com/ntruchsess/arduino_uip
    
 - Arduino Ethernet library description 
    http://arduino.cc/en/Reference/Ethernet
    
 - uIP API reference:
    http://www.sics.se/~adam/uip/uip-1.0-refman/

 - Arduino forums
    http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl

 - uIP homepage:
    http://www.sics.se/~adam/uip/index.php/Main_Page

Licenses
-------------

UipEthernet.h
UipEthernet.cpp
UdpSocket.h
UdpSocket.cpp
TcpClient.h
TcpClient.cpp
TcpServer.h
TcpServer.cpp
utility/MemPool.h
utility/MemPool.cpp

Copyright (c) 2013 Norbert Truchsess <norbert.truchsess@t-online.de>
All rights reserved.

Modified (ported to mbed) by Zoltan Hudak <hudakz@outlook.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

--------------

utility/enc28j60.h

Author: Pascal Stang (c)2005
Modified by Norbert Truchsess
Copyright: GPL V2

--------------

utility/Enc28j60Py.h
utility/Enc28j60Py.cpp

Copyright (c) 2013 Norbert Truchsess <norbert.truchsess@t-online.de>
All rights reserved.

inspired and based on enc28j60.c file from the AVRlib library by Pascal Stang.

Modified (ported to mbed) by Zoltan Hudak <hudakz@outlook.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

--------------

utility/uip.c
utility/uip_arp.h
utility/uip_arp.c
utility/uip_arch.h
utility/uip.h
utility/uipopt.h

Copyright (c) 2001-2003, Adam Dunkels <adam@sics.se>, <adam@dunkels.com>.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of the Institute nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

--------------

uip-conf.h
utility/uip-neighbor.h
utility/uip-neighbor.c
utility/uip_timer.h
utility/uip_timer.c
utility/uip_clock.h

Author Adam Dunkels Adam Dunkels <adam@sics.se>, <adam@dunkels.com>
Copyright (c) 2004,2006, Swedish Institute of Computer Science.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of the Institute nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

--------------

DhcpClient.h
DhcpClient.cpp

DHCP Library v0.3 - April 25, 2009
Author: Jordan Terrell - blog.jordanterrell.com
- as included in Arduinos stock Ethernet-library, no special licence mentioned here

Modified (ported to mbed) by Zoltan Hudak <hudakz@outlook.com>

--------------

DnsClient.h
DnsClient.cpp

(c) Copyright 2009-2010 MCQN Ltd.
Released under Apache License, version 2.0

--------------

clock-arch.h
clock-arch.c
 
Copyright (c) 2010 Adam Nielsen <malvineous@shikadi.net>
All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

--------------

IpAddress.h
IpAddress.cpp
Copyright (c) 2011 Adrian McEwen.  All right reserved.

Modified (ported to mbed) by Zoltan Hudak <hudakz@outlook.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
