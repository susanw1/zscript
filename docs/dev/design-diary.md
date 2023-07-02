LOG OF IDEAS AND DEVELOPMENT
===

Realizing we're making lots of design decisions without writing them down!

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
