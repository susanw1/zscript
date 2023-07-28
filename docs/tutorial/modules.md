Modules
===

Zscript commands are delivered as sets of up to 16 commands, called modules. A module is expected to provide a capability, such as "I2C" or "stepper motors", and is comprised of:
* some useful Zscript commands for exposing the capability;
* a low-level implementation of the capability, either as bare-metal code or leveraging some HAL or OS calls.

Plus, if the module defines some kind of comms or interconnect, then:
* (optional) a _channel_ that allows Zscript commands to be received from upstream and answered;
* (optional) an _address forwarder_ for forwarding addressed commands to a downstream device.

##### Command Format

When a command is being executed, the `Z` 16-bit field is decoded: the top 12 bits identify the module, and the lowest-order 4 bits (the bottom digit in hex) defines the command number within the module. This allows for 4096 different modules, with 16 commands each. 

##### Types of Module

Loosely, there are the following types of module to make up a Zscript device:

* _Core_ modules: these are delivered with every installation of Zscript to provide the basic mechanics.
* _Standard_ modules: these might be supplied with an installation, or be added in as 3rd-party components. They have well-known module numbers and conform to a specification. Examples might be: a servo-control module; I2C; LEDs; Power Delivery; LCD graphical display. Each standard module might be implemented on different platforms, even for the same device (eg bare-metal; Arduino libraries; Mbed; STM HAL etc).
* _User-defined_ modules: some module number ranges are defined to be unallocated, allowing embedded devs to implement their own systems as Zscript commands on a bespoke basis. Writing embedded functionality this way facilitates testing, development and modularity - as well as enabling upstream devices to invoke it. 

Generally useful and well-designed user-defined modules might be sanctified to become Standard.

A given Zscript-enabled device should provide the collection of modules that perform the tasks it need to do. It is possible to exclude unwanted modules in order to minimize the code/memory overhead.

##### Capabilities 

By convention, command 0 in each module exposes module-specific "capabilities" - how many ports, which commands are supported, how big the buffers are, version info etc. 

The module number assignments provisionally look like this:

|| Module Number || Name || Purpose ||
| 000 | Core | Basic Zscript management - the only module usable before activation |
| 001 | Outer Core | Basic management only required after activation |
| 002 | Zscript | Install and execute scripted Zscript |
| 003 | Pins | Low-level digital and analog IO |
| 004 | ... | | 
| 005 | Serial | Serial line read/write and notifications |
| 006 | SPI | SPI read and write |
| 007 | I2C | Read/write as an I2C master |
| 008-00e | Future standard modules |
| 00f | User | User-defined module |
| 01x-0ex | Future standard modules |
| 0fx | User | More user-defined modules |
| 1xx-exx | Future standard modules |
| fxx | User | Even more user-defined modules |

