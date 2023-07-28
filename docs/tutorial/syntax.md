Zscript Syntax
===

In Brief
---
A Zscript-enabled device can be sent sequences of Zscript commands, and will return corresponding sequences of Zscript responses to provide status and other information. Command sequences and response sequences follow essentially the same syntax, to make them easy to figure out and parse (though response sequences always begin with a `'!'` char so you can distinguish them).

Commands and responses consist of key-value pairs, where the keys are upper-case letters, and the values are numbers made of some (lower-case) hexadecimal digits. Everything is in hex. There is special syntax for sending a single larger field per command/response, either as a string of bytes like this: `"hello"`, or as hex like this `+a68656c6c6f`, depending on what is most convenient or expressive.

In a command, the `'Z'` field always indicates the command to be run, and in a response, the `'S'` field indicates the success/error/failure status.

Here are some examples:
* Command 0032 (low-level pin set) setting the value of pin#3 to HIGH. The `'S'` response shows it worked: it's actually `S0` (success), though generally responses should drop all leading zeros.

```
Sent> Z32 P3 V1
Rcvd> !S
```

* Command 0053 (I2C write-then-read), sends 5 bytes to address 0x62 on I2C bus #0, and then reads 5 bytes back. The response indicates that two retries were required. 

```
Sent> Z53 B0 A62 L5 +48656c6c6f
Rcvd> !S +00ffffffff T2 
```

Sequences are chains of commands (or responses), separated by `'&'` ("and then") and `'|'` ("or else") characters, and terminated with a single newline (asc 0x0a, or '\n' in C notation). Subsequent commands are executed or skipped based on the S-field Status of the last command, where:
* `S0` (or just `S`) indicates success (like "true"), so the next `'&'` command will be executed.
* `S10`-`Sffff` indicates a failure (like 'false'), and execution will skip to the next `'|'` command.
* `S1`-`Sf` indicates a fatal error condition, and the sequence aborted.

So in the (hypothetical) command sequence `Z1 & Z2 & Z3 | Z4 & Z5`,
* if `Z1` fails with `S10`, the response might be `!S10 | S & S` (or if subsequent command also fail, then `!S10 | S10`, or even `!S10 | S & S10`). Either way, Z1 failed so Z2 and Z3 were skipped.
* alternatively, if `Z2` fails, the response might be `!S & S10 | S & S`
* and if `Z2` fatally errored out with `S9`, the response might be `S0 & S9`, with the whole of the remaining sequence skipped.

This mechanism all allows for commands to be executed as a dependency sequence where later commands only run when earlier ones succeeded; it also allows for conditional and alternative flows. The structure of a response sequence, with its `'&'` and `'|'` chars, always mirrors the sequence it executed (minus the parts it didn't) so you can deterministically match responses to commands.


Zscript Command Definition
---

When a host or controlling device wants to communicate with a Zscript device, it sends one or more Zscript commands. A Zscript command consists of a series of key/value fields, which indicate the command that is to be run along with additional parameter data. It's similar to a command with options and arguments, or a function with parameters.

There are two kinds of field:
* Numeric fields: An upper-case letter key followed by up to 4 lower-case hex digits, eg A1b4 or D2, meaning that the numbers are 2 bytes long, representing values 0-65535. 
    - There can be multiple numeric fields, either optional or mandatory, depending on the command.
    - All leading zeroes may be omitted, so `A20` is same as `A0020`, and `M` is the same as `M0000`.
    - `Z` is a special field: it is always mandatory in a command and signifies which command is to be run.
    - `S` is a special field: it is always mandatory in a response and signifies the exit status of a command.
* Big field: a command may have a single "big" field, which is a series of hex values or an arbitrary-length string, allowing chunks of data to be sent.
    - hex-mode uses a `'+'` key, followed by lower-case hex pairs, eg `+1234ab` (must be an even number of digits).
    - string-mode allows an arbitrary set of 1-byte values surrounded by normal double-quotes, eg "hello".
         - inside a string, `'='` followed by exactly 2 hex digits may be used as an escape.
         - the following characters *must* be escaped in a string: double-quote (`=22`), '=' (`=3d`), newline (`=0a`), null (`=00`).
    - the maximum size of Big Field is determined by the available buffer space on the device - overruns are reported as a fatal error.
    - the choice of whether to use string or hex format is up to you - the commands don't know or care which you use. String may be more compact in transmission.

The order of the fields in a command is _not_ significant, so you can put them in any order you like. Also, except for inside strings, the following characters are completely ignored:
* spaces (asc 0x20)
* tab (asc 0x09, '\t')
* carriage return (CR, asc 0x0d, '\r' - not to be confused with newline!)
* comma (',' - note, actually we're wondering about this)
* null (asc 0x00, '\0') - always dropped under all circumstances.

This means you can choose to space out your Zscript as you wish to make it readable, or compact it for performance reasons, eg `B4AZ21&Z22"foo"` is same as `Z21 A0 B04 & Z22 "foo"` 

Note: Null chars may be used over certain protocols to eg pad out packets.


Zscript Response Definition
---

When a Zscript device needs to send a message back upstream, it sends a Response Sequence. Usually, this would be a (synchronous) status response to a command sequence, but Zscript can also generate asynchronous responses, generally called Notifications, from other sources (see scripting, interrupts, warm boot).

The syntax or a response is identical to that of a command - it's essentially symmetrical - with the follow exceptions:
* Responses always begin with a `'!'` character. Actually this is really saying `!0`, where `'0'` means "synchronous response". Other numbers would indicate different (usually async) sources, such as an interrupt handler.
* Numeric response values may be 4 bytes long, not just 2. 

Addressing
---
If a command sequence begins with `'@'`, then it is not executed on this device but is passed on to another downstream device. Here are some examples:

```
Sent> @7.1 Z32 P3 V1
Rcvd> !@7.1 S0
Sent> @7.1@5.0.61Z32P3V1
Rcvd> !@7.1@5.0.61S
```
An address is a `'.'`-separated sequence of (2-byte) numbers representing a (hierarchical) address which is passed to an address dispatcher, which maps it to a downstream device and sends the Zscript characters following the address over to it.  When a response comes back, the address is re-inserted and passed back upstream. Addresses can be nested, so sub-sub-devices can be addressed too.

The default address dispatcher grabs the first part of the address (the `'7'` in the example above) and talks to module '007' (serial comms), which provides a dispatcher that treats the next part as the port number (#1). The second example shows a nested address talking to module '005' (I2C) and its dispatcher uses I2C port 0, address 0x61.

Comments
---
If the first non-blank character on a line is `'#'`, then the remainder of the line up to the next newline is ignored.

Locking
---
If a command sequence starts with a `'%'` sign (eg `%1a Z52`), it indicates that the sequence must grab specific exclusion locks before executing. On advanced devices, it's possible to run many Zscript executors at once, with Zscript and Interrupt notifications, and in principle the hardware devices could end up under contention. The locks are represented by a (normally 2 byte) bit-set of flags, and by default (without a `'%'`) a command sequence will only run if all locks are available (ie default is effectively a highly defensive and exclusive `%ffff`). 

By specifying locks, you can say "actually I only care about these locks", which might allow four polling activities on four I2C buses to run concurrently with occasional incoming I2C commands too. The specified lock bits must be clear for the commands to run, and will be set for the duration of execution. If you don't care about concurrent execution, then you can ignore locking; everything should just run sequentially. If you do care, then you choose a bit to represent your device, and specify it consistently in all commands that touch it.

(We need a whole page on locking and concurrency.)

Command Line Number Tagging
---
In a command sequence, if a `'_'` is specified with a value, then it will be echoed back into the response. It can be used like a line number. It allows a call/response client to marry the incoming responses back up with the commands to which they relate. This command-sequence has been given line number 0x56, which is echoed back in the response sequence.

```
Sent> @7.1 %4 _56 Z32 P3 V1
Rcvd> !@7.1 _56 S0
```

This is particularly useful if you are "pipelining" commands, that is, sending them in one after another without waiting for responses.
