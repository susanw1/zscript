![ZscriptLogo-1](https://github.com/susanw1/zscript/assets/54667166/782f22bc-a916-446f-a5f5-1de6e13dff60)

[![Java Build](https://github.com/susanw1/zscript/actions/workflows/maven.yaml/badge.svg)](https://github.com/susanw1/zscript/actions/workflows/maven.yaml)
[![Codecov](https://codecov.io/gh/susanw1/zscript/graph/badge.svg?token=V4A5P958UU)](https://codecov.io/gh/susanw1/zscript)
[![CodeFactor](https://www.codefactor.io/repository/github/susanw1/zscript/badge)](https://www.codefactor.io/repository/github/susanw1/zscript)
[![Javadoc](https://javadoc.io/badge2/net.zscript/zscript-all/javadoc.svg)](https://javadoc.io/doc/net.zscript/zscript-all)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

***

##### What Is It?

Zscript ("zed-script", and formerly Zcode) is a simple text-based command/response system, designed to make it easy to control microcontrollers (MCUs) over various communications
protocols, eg ethernet, serial ports or I2C. It is loosely inspired by *G-code*, the control language used by many automated machine tools.

##### Project Status

Pre-alpha! We're still putting it together.

##### Where do I get it?

Instructions are here: [Where-to-get-Zscript](https://github.com/susanw1/zscript/wiki/Where-to-get-Zscript)

##### What's so special?

* Simple syntax - requests and responses are designed to be reasonably straightforward to use and understand, for both human- and computer-generated commands.

         # Set pin 4 to HIGH, and receive Status response OK to confirm
         sent> Z42 P4 V1
         recv> !S

* [Modular and extensible](https://github.com/susanw1/zscript/wiki/Modules)
* Client libraries, in Java (and Python, to come) - generated from declarative modules.
* Small, efficient and configurable - you can remove features and modules so you don't pay for what you don't use.
* Device addressing - Zscript commands can be tunnelled from one device to another over serial, I2C etc.
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

* [Documentation](https://github.com/susanw1/zscript/wiki)
* [Issues](https://github.com/susanw1/zscript/issues)
* [Discussion](https://github.com/susanw1/zscript/discussions)
* [ X ](https://x.com/_zscript_) and [Groups](https://groups.google.com/g/zscript)
