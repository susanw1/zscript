ZCODE
===

##### What Is It?

Zcode ("zed-code") is a simple text-based command/response system, designed to make it easy to control microcontrollers (MCUs) over various communications protocols, eg ethernet, serial ports or I2C. It is loosely inspired by G-code, the control language used by many automated machine tools.

##### Key points

* Simple syntax - requests and responses are designed to be reasonably straightforward to use and understand, for both human- and computer-generated commands.

* Modular and extensible - commands are implemented as modules, with space for user-defined commands. Standard modules include I2C, UART, SPI, USBC-PD

* Embedded scripting - Zcode programs can be compiled in or loaded onto the device dynamically, to run in the background or in response to triggers.

* Introspection - device capability and version info is exposed

* Small, efficient and configurable - you can remove features and modules so you don't pay for what you don't use.

* Device addressing - Zcode commands can be tunnelled from one device to another.

* Portable - implemented on ARMs and Arduinos, with PICs to come soon.

* Interactive - simple REPL (read-eval-print-loop) supported.

* Client libraries - in Java (and Python, to come).
 

##### Purpose

Zcode makes it really easy to try out your hardware, and then control it from another computer. This facilitates:

* Interactive experimentation and prototyping: you can try out talking over buses or reading/setting pins while you get everything right.

* Service orientation: define custom operations as commands in user-defined modules, and treat them as hardware services invoked by a command system. It's RPC for hardware.

* Testing: you can capture command sequences as scripted tests, and run them for conformance / regression testing.

##### Links:

[Documentation](docs/spec/zcode-description.md)

[Developer setup](docs/dev/dev-setup.md)

[Arm Deployment](receivers/native/arm/no-os/docs/stlink-deployment.md)