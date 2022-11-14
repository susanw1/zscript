Hints for deploying to ARM Cortex4 STM32G4(84) over ST-Link/v2 with openocd under Ubuntu (20.04, 22.04)
===

Ubuntu 20.04 includes OpenOCD v10 which doesn't include STM32G4 support.

Ubuntu 22.04 includes OpenOCD v11 which DOES include STM32G4 support.

So... git clone https://github.com/susanw1/zcode.git

(Earlier experiment - don't need this: git clone https://github.com/ARMmbed/mbed-os.git)

Configuring for OpenOCD v10 - sorry, mostly obsolete
----

If using Ubuntu 20.04, you'll have Openocd v10 by default which doen't support the STM32G4 series yet. I took a private copy of latest, like this: 
```
$ git clone https://git.code.sf.net/p/openocd/code openocd-code
$ cd openocd-code
  ./bootstrap # (when building from the git repository)
  ./configure [options]
Enabled transports:
 - USB ............................ no (missing: libusb-1.0)
 - TCP ............................ yes
ST-Link Programmer                  no

$ # remembered to install libusb, and then ask for the right programmer
$ sudo apt install libusb-1.0
$ ./configure --enable-stlink 
... all worked for me!
$ make


$ # point a variable at the openocd-code location:
$ P=~/ZcodeProject/openocd-code
$ cd ../zcode/receivers/native/arm/no-os
$ make install OCD=$P/src/openocd OCDSCRIPTS=$P/tcl
```

Debug command (connect without reflash/restart) (in output from make):

```
$ # Opens a connection, but told it nothing to do, so leaves connection open (opens GDB connection on 3333)
$ $P/src/openocd --search $P/tcl -f $P/tcl/interface/stlink.cfg  -f $P/tcl/target/stm32g4x.cfg
```



REFLASHING
===========

Plug in USB for STM32G484 (and network + 5V supply if required)

```
$ mvn deploy -pl receivers/native/arm/no-os/stm32g484
...
**pre-verifying**
Error: checksum mismatch - attempting binary compare	(checking existing flash to avoid dupe)
** Programming Started **
```

Unplug USB, replug, wait 5s (dhcp etc)

```
$ nc -u -p 43243 192.168.23.147 4889

$ # To debug, run openocd with connection left open:
$ mvn exec:exec@openocd-gdb -pl receivers/native/arm/no-os/stm32g484
$ # Note - ^C leaves this running - 

$ # if required: sudo apt install gdb-multiarch
$ # was: gdb-multiarch target/bin/no-os.elf .... main is the ELF file anyway.
$ gdb-multiarch target/test-nar/bin/arm32g4-no_os-gpp/main
(gdb) target extended-remote localhost:3333
- connects to earlier opened openocd GDB connection, using ELF file for symbols
```

Quick GDB guide:
*	bt - backtrace
*	frame 10 - details of stack-frame 10
*	info locals
*	info args
*	continue, ^C 
*	step/fin
*	break/clear functionName() - brkpts
*	p <expr> - print


More rules:
* Note, both Arm boards are same MAC. But we can run Z commands to persist a different MAC-address.

Arduino:
lib area
keywords.txt
library.properties
src/zcode = generic
src/arduino = "the arduino dir"
src/* = arduino-converter-files

Addressing...
(ether, 144) ARM G484 -> (Ser#0) 152 ARM F030 -> (I2C#0) Arduino Nano


###Example for toggling LED on Arduino p13
Z0
##### SC311fMaf
Z2
##### AS
Z18N
##### +deadbeeffeedS
Z71P
##### S

@7.0Z
##### @7.SC311fMab
@7.0 Z2
##### @7.AS
@7.0 Z18N
##### @7.S
@7.0 Z51
##### @7.S
@7.0.5.0.61 Z0
##### @7..5..61SC310fMbNa
@7.0.5.0.61 Z2
##### @7..5..61AS
@7.0.5.0.61 Z31 Pd C M4
##### @7..5..61M313S
@7.0.5.0.61 Z32 Pd V1
##### @7..5..61S
@7.0.5.0.61 Z32 Pd V0
##### @7..5..61S




Red STlink unplugged,
USBC power
Plug in USB from host
