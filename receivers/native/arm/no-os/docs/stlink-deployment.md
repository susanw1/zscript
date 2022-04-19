Hints for deploying to ARM Cortex4 STM32G4(84) over ST-Link/v2 with openocd under Ubuntu (20.04)
===

Ubuntu 20.04 includes OpenOCD v10 which doesn't include STM32G4 support.

So... git clone https://github.com/susanw1/zcode.git

(Earlier experiment - don't need this: git clone https://github.com/ARMmbed/mbed-os.git)

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


$ P=~/Code/ZcodeProject/openocd-code
$ cd ../zcode/receivers/native/arm/no-os
$ make install OCD=$P/src/openocd OCDSCRIPTS=$P/tcl

Debug command (connect without reflash/restart) (in output from make):
$ $P/src/openocd --search $P/tcl -f $P/tcl/interface/stlink.cfg  -f $P/tcl/target/stm32g4x.cfg
	- opens a connection, but told it nothing to do, so leaves connection open (opens GDB connection on 3333)


REFLASHING
===========

Plug in USB (and network + 5V supply if required)
run make command again

**pre-verifying**
Error: checksum mismatch - attempting binary compare	(checking existing flash to avoid dupe)
** Programming Started **

Unplug USB, replug, wait 5s (dhcp etc)

$ nc -u -p 43243 192.168.23.147 4889


$ sudo apt install gdb-multiarch
$ gdb-multiarch build/mbed-os.elf
(gdb) target remote localhost:3333
- connects to earlier opened openocd GDB connection, using ELF file for symbols

GDB:
	bt - backtrace
	frame 10 - details of stack-frame 10
	info locals
	info args
	continue, ^C 
	step/fin
	break/clear functionName() - brkpts
	p <expr> - print

+ Cucumber Eclipse Plugin

