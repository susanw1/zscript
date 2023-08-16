LOG OF IDEAS AND DEVELOPMENT
===

Realizing we're making lots of design decisions without writing them down!


2023-08-16
---
A few other interesting discussions/decisions:
- The word 'port' is not a good fit for all communication protocols. Instead we'll switch to "interface" as a descriptor for physically different subsystems of the same protocol.
- We will not have boolean or tri-state values. Instead they will be two state enums, and two state optional enums. This allows for better descriptions and gives only one true/false type (flags)
- Channel info will have: Module (number, M), Notifications (opt-enum, N), Activated (flag, A), and Interface (number, I), and others as required. No string descriptors.
- Channel setup will have: Notifications (opt-enum, N), and other setting as required
- String descriptors should be avoided. Instead the machine-readable state should be unambiguous, and the client side should generate the strings (through lookup or dynamic generation). This saves memory, makes coms easier, etc.

2023-08-16
---
Discussed whether UARTs, USB and TCP should be unified at all. Conclusion: NO.
The philosophy of Zscript should be: Any time two things are distinct, they should live in different modules, even if they act very similarly.
This should also be seen in the Pins system, where no attempt will be made to unify analog and digital read/write.
Instead they are considered different commands within the same module (which is valid), but unless the capabilities are as related as different ways of reading the same pin, they should be in different modules.
Zscript supports LOTS of modules (4096), and they should be used liberally. Abstracting shared functionality like that is not the role of the Zscript receiver, instead, the command layouts should share patterns and languages. Then it is the role of good client side software to provide meaningfully shared interfaces onto those capabilities. This way, no more tension is added to the difficult low-level code, making it easier to use on a range of devices, and no capabilities are hidden from the client side, so abstractions are optional rather than mandatory.
This gives better flexibility, moves complexity to where it can easily be absorbed, while still providing useful abstractions.
It also removes issues surrounding pointless settings, like setting the baud rate of a TCP serial connection.


2023-08-14
---
Spent a while optimising for code size on the arduino - got a minimal version (including deleting code which wasn't relevant) down to 8388 bytes, which is larger than `generic-core 2` (at ~7400 bytes), but not too large. To get this in a real version we'll need to add `#ifdef`s to the code.
More general optimisation guides appear to be:
- Inlining is very important, for both speed and code size. In light of this, calling a complex function twice (instead of once) costs ~200 bytes of flash.
- Bit-packing bools is free, bit-packing other values at the bottom of bytes is free, while putting them elsewhere in a byte isn't too expensive.
- Most other optimisation attempts result in marginal or minimal gains. virtual functions aren't free, but aren't too bad either.


2023-07
---
Semantic parser (Java version) is done, and we've ported the tokenizer to C++. Interesting decisions along the way include:
* changing response status meanings (the 'S' field). So far, we've had 0 (success), 0x1-0xf (hard error, stop execution), and 0x10 onwards (command failures, skip to ORELSE ('|') commands). But actually, we have lots of detectable hard error conditions, like syntax errors (eg missing close-quote), resource issues (eg command too long), and command errors (eg required field missing). And yet we've only ever needed a single command-fail value. *Decision*: we've reworked it: 0x1-0xf are command-fails, and 0x10+ are errors. Better.

* We realized that it might be bad not to be able to distinguish command sequences from responses - they both have the same general syntax and structure. *Decision*:  _All_ response-sequences MUST begin with `'!'`. Up to now, only notification (async) responses had them, to identify their source (eg script-space, or interrupt notification). Now, `'!'` really means `'!0'`, implying source `0`, which is the synchronous response source.


2023-06
---
Lots of progress with Java RingBuffer pilot. Much simplified tokenizing, but still got the complicated sequence-level semantic analyzer to go.


2023-04-09
---
Situation: we've been thinking about slots and parsing and general complexity of the parse system, realized that separating Zcode tokenizing from semantic parsing makes all components far simpler, and means that we don't need separate stores for the BigField, other Fields, etc. 

We've been trying to consolidate on a single design approach for a parser that would be easy to write in C, Java, Python, PIC assembly, etc ... ideally so that anyone can write a new Zcode code in a short time. Then we need to build a test framework to allow them to "certify" it. 

So, we're trialling a new approach in Java, where we have a fixed size ring-buffer, and the main parser writes its finding in a sequence of &lt;LEN> &lt;CODE> &lt;DATA...> tokens. By running as a ring-buffer, there can be a region where an incoming command byte-stream dumps its tokenized state, and another region where command-sequences are being read. Ideally the write phase is so lightweight and byte-by-byte that it could live in a MCU's interrupt handler, with small and bounded execution time.


Failures priorities - esp around parse/buffer overrun errors, in descending order of priority:
1. Leave devices in a usable state, locks not left locked, return suitable Sxx field: overrun overrides others
1. Ensure that caller knows what has been done and what has been aborted (using the returned 'S0 & S0 & | |' command separators
1. Failing as early as possible in command sequence - a parse error later will terminate sequence after any currently running commands
1. Include '_' echo field if readable
1. Include '&' and '|' to indicate the command where the error occurred

Probably reserve S1 as "Fatal Device Error" - we're not going to execute your command because the Zcode can't guarantee anything, eg battery is flat. Best effort. This might have been pre-warned with a Notification.
