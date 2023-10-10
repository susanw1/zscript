![ZscriptLogo-1](https://github.com/susanw1/zscript/assets/54667166/782f22bc-a916-446f-a5f5-1de6e13dff60)

[![Java Build](https://github.com/susanw1/zscript/actions/workflows/maven.yaml/badge.svg)](https://github.com/susanw1/zscript/actions/workflows/maven.yaml)
[![codecov](https://codecov.io/gh/susanw1/zscript/graph/badge.svg?token=V4A5P958UU)](https://codecov.io/gh/susanw1/zscript)
[![javadoc](https://javadoc.io/badge2/net.zscript/zscript-all/javadoc.svg)](https://javadoc.io/doc/net.zscript/zscript-all)
[![CodeFactor](https://www.codefactor.io/repository/github/susanw1/zscript/badge)](https://www.codefactor.io/repository/github/susanw1/zscript)

***

##### What Is It?

Zscript ("zed-script", and formerly Zcode) is a simple text-based command/response system, designed to make it easy to control microcontrollers (MCUs) over various communications
protocols, eg ethernet, serial ports or I2C. It is loosely inspired by *G-code*, the control language used by many automated machine tools.

##### Project Status

Pre-alpha! We're still putting it together. Release before end of 2023.

##### Where do I get it?

Instructions are here: [Where-to-get-Zscript](https://github.com/susanw1/zscript/wiki/Where-to-get-Zscript)

##### What's so special?

* Simple syntax - requests and responses are designed to be reasonably straightforward to use and understand, for both human- and computer-generated commands.

         # Set pin 4 to HIGH, and receive Status response OK to confirm
         Z42 P4 V1
         !S
* Modular and extensible - commands are implemented as modules, with space for user-defined commands. Standard modules include I2C, UART, SPI, USBC-PD. Modules
  defined [declaratively in YAML](https://github.com/susanw1/zscript/tree/master/model/standard-module-definitions/src/main/resources/zscript-datamodel/00xx-base-modules).

* Client libraries, in Java (and Python, to come) - generated from declarative modules.

        commPortConnection.send(PinsModule.digitalWrite()
            .setPin(4)
            .setValue(1)
            .build());

* Small, efficient and configurable - you can remove features and modules so you don't pay for what you don't use.

* Device addressing - Zscript commands can be tunnelled from one device to another over serial, I2C etc.

        # Send echo command to 2nd device via 1st over I2C link, and receive response.  
        #  5.1.62 means (module 5) bus 1 address 0x62
          @5.1.62 Z1 A2
          !@5.1.62 A2 S
* Portable - implemented on ARMs and Arduinos, with PICs to come soon.

* Interactive - simple REPL (read-eval-print-loop) supported.

##### Purpose

Zscript makes it really easy to try out your hardware, and then control it from another computer. This helps with:

* _Interactive experimentation and prototyping_:
    - you can try out talking over buses or reading/setting pins while you get everything right.

* _Service-based approach_:
    - define custom operations as commands in user-defined modules, and treat them as hardware services invoked by a command system. It's RPC for hardware.

* _Testing_:
    - you can capture command sequences as scripted tests, and run them for conformance / regression testing.

*** 

##### Links:

[Documentation](docs/spec/zscript-description.md)

[Developer setup](docs/dev/dev-setup.md)

[Arm Deployment](receivers/native/arm/no-os/docs/stlink-deployment.md)
