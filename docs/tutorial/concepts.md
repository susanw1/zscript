Glossary of Concepts
===

This is a quick look-up guide to help with understanding Zcode terms and concepts.

##### Activation

When a Zcode-enabled device is switched on, it will only execute commands in the Core module. One of those commands is the `ACTIVATE` command, after which all commands are available. This mechanism means that if the device is unexpectedly reset, then an upstream system that is sending commands will become aware and can take remedial action, rather than just continuing to send commands with the device in an unknown state.

##### Addressing

This is a mechanism that allows a device to act as a relay to sub-devices. The syntax `@2d Z1 & Z2` tells a device to send the command sequence `Z1 & Z2` to a device designated as device #2d, and might produce a response similarly annotated with the address: `@2d S0 & S0`.

Addresses may be hierarchical, like this `@1.2` perhaps representing sub-device #2 on bus #1. 

Addresses may be chained: `@1.2 @4 Z1`, where `@4 Z1` is processed by the sub-device.

The decision about how to interpret an address is made by an _AddressRouter_ which is written as a part of the Zcode implementation. The embedded dev gets to choose just how addresses are interpreted.

##### Channel

A channel is a source of Zcode commands, which feeds a Zcode executor. The responses are typically fed back up the channel to the original sender. Typically, a channel which works over a particular comms interface would be provided as an optional part of a module relating to that interface. Some channels are not associated with comms, such as a Zscript executor, which will only send responses as notifications. 

Channels can be Ordered (such as a serial line or TCP) or Unordered (such as UDP). If you use an unordered channel, and you pipeline the command sequences (ie you don't wait for responses between sends) the commands will be executed in the order they are received, and the responses may arrive back in yet another order. 

##### Notifications

Generally, responses are generated as a result of running commands, with one response corresponding to one command. However, a Zcode device may send unsolicited messages, called Notifications. These can be generated in response to interrupts ("pin 3 went low"), or when Zscript commands fail.

For notifications to be received, a specific channel must elect itself as the "notification channel", using the core `SET NOTIFICATION CHANNEL` command.

The `'!'` response marker's value is normally zero, meaning it's a simple synchronous response to a command. But it's really a notification source indicator, and if it is non-zero, it implies that the response is actually a notification produced from a script, an interrupt, or a some other condition. (Future: determine the exact meaning of the notification's value. Should it be tied to a source module, like most other things?)

##### Response Status

The response status is the `S` field in a response. It has the following significant values.

* `S0` (or just `S`) indicates success (like "true"), so the next `'&'` command will be executed.
* `S10`-`Sffff` indicates a failure (like 'false'), and execution will skip to the next `'|'` command.
* `S1`-`Sf` indicates a fatal error condition, and the sequence aborted.

##### Locking

Locking is a mildly advanced feature which allows multiple Zcode executors to access (slow) independent devices in parallel. On advanced devices, it's possible to run many Zcode executors at once, (eg Zscript and interrupt notifications), and by default, no two command sequences can be being executed simultaneously. If you have a device with, say, several I2C buses that you need to poll, it is useful to be able to run the commands actually in parallel, whilst still requiring that any one bus is only accessed by one of those executors at any one time.

If a command sequence starts with a `'%'` sign (eg `%1 Z52`), it indicates that the executor will proceed only when the specified exclusion locks are free, grabbing them before executing and releasing them afterwards. The locks are represented by a bit-set of flags, and by default a command sequence without any `'%'` will only run if all locks are free (ie it defaults to a highly defensive and exclusive `%ffff`). A command preceded by `%0000` (abbreviated to just `%`) translates to "just run, ignore locks".

By specifying locks, you can say "actually I only care about these locks", which might allow four polling activities on four I2C buses to run in parallel, with occasional incoming I2C commands too. The specified lock bits must be clear for the commands to run, and will be set for the duration of execution. If you don't care about concurrent execution, then you can ignore locking; everything should just run safely and sequentially. If you do care, then you choose a bit to represent the bus you want to access, and specify it consistently in all commands that touch it.

##### Modules

A module is a group of up to 16 Zcode commands with their low-level implementation. If a command has the Z command number 1a46, then it is command '6' in module 1a4. Modules can also provide a command/response channel, and can register an address forwarder. The embedded dev chooses which modules are available in a given firmware.

##### Capabilities

Command #0 in each module is expected to return the module's "capabilities", in a module-specific way. This might reveal the number of ports, the value limits of fields, the size of buffers, and whether optional features are supported. This allows a client to make intelligent choices, eg about what commands to use, or how much data to send without flooding a buffer.


##### Zcode Model

A new module should be described by a definition file in the Zcode Model. This lists the commands and responses, their fields, and describes what they do and how they should be used. The model can then be used to:
* auto-generate client message builders to make client code easier to write, readable and pretty.
* hook into GUI tools to interpret and analyse Zcode messages.
* generate documentation and help tools
* potentially, generate any boiler-plate parts of an embedded implementation (knowing which fields are required, etc)

##### Zcode Client Library

A client library is some software that allows a connection to be made to a Zcode-enabled device over some channel, so that commands may be sent and responses processed. A minimal client will just make a connection, and send and receive raw Zcode text. A more full-featured one has message-builders, a means of cataloguing downstream devices, notification call-backs and diagnostics.

