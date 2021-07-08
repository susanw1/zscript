RCode:
===
### General

RCode is a simple command-response system, designed to allow the control of MCUs (Microcontrollers) over many possible communication protocols, by either computer or human generated commands, with easily parsed responses.
It is composed of Commands, separated by `&`s built into command sequences, separated by new lines (`\n`) or packet ends. 

RCode is designed to be flexible to the capabilities of the receiver, allowing very simple, or very capable endpoints to work within their capabilities. 
It is designed to allow accurate peripheral control, such as reads and writes to GPIO pins, SPI, I2C, or other peripherals, 
	to allow simple expansion, and other features, such as programmable background code execution, or interrupt handlers.
RCode is provided to the MCU over some form of 'command channel' this might be UDP, TCP, serial lines, USB, or any other protocol.

### Communication

There are two types of channel: packet based (e.g. UDP), where information is sent in lump quantities, and stream (e.g. serial), where the protocol provides no way of telling the end of a piece of data.
In a stream system, the receiver must guarantee to either wait for the entire command sequence to be received before parsing (a new line received), or to delay parsing while information is received.
In a packet based system, the packets can be received entirely before parsing, or can be executed while they arrive, given enough wait to not overtake the reception.


To allow use of USB connections, the MCU can either implement a USB interface, which the sender then attaches a pipe to, 
	however due to a general lack of good USB libraries, it is generally worth either using a USB to serial converter, or implementing a virtual serial port on chip, and then responding to that.
	The interface for a true USB connection is not defined here, and can effectively be 'whatever works' for the libraries you have.

Due to its simplicity, UDP is the recommended network command channel for normal communication, however to allow easier debugging, TCP can also be used. 
	This is particularly recommended for the default debug channel, in the absence of serial/USB, as a telnet connection can then be used.

An RCode receiver is allowed to implement routing. This is where an `@` is placed at the begining of a command sequence - the rest of the command is skipped by the parser and ignored. 
No specification is given for the operation of such a command sequence, but it must not involve a newline (otherwise the next command sequence will be considered to have begun) and the `@` must be the first non-whitespace in the sequence. 
The encouraged method of operation is that the begining of the routed command sequence has a dot seperated series of addresses, and the command is send with the first of these stripped off.
An example:

A receiver receives: `@9.1c*R1A2`
It then sends `@1c*R1A2` along whichever communication protocol corresponds to address `9`.
The receiver of this then sends `* R1A2` along the communication path corresponding to address `1c`.
Any response to the sent communication should put the address back on the begining, so in the above example, 
when the second receiver is given the response `*SA2`, it sends `@1c*SA2` back, and the original receiver takes this, and sends `@9.1c*SA2` back to where the message comes from.

Note that the contents of a routing command need not be RCode, although that is the general intention if newlines are needed inside the message, any encoding can be chosen (again this is more of an external capability to RCode).


### Command Structure

A single command is a series of fields, and a possible big field.
A field is a single capital letter, followed by hex digits (lower case only) multi-byte fields can be considered to be single byte fields with virtual markers for the other bytes.
A single hex digit is taken to mean the least significant digit and no value at all means 0.  
These are all fields: `A01` `F00` `R13` `C` `D1` `G0000` `L900`  
These all have the same value: `C` `C0` `C00` `C000` `C0000`  
The parser must allocate only one byte to the `C000` and `C0000`, even though they appear two byte, in general leading `0`s must be ignored.
A receiver can have a maximum number of fields (or not, if memory allocation is used, but then at least 255 fields must be allowed). 
This can specify either the maximum number of seperate fields, with a seperate maximum length for each, or can specify the number of units of fields used total. 
This means that e.g. 20 bytes of fields can be used, either on lots of small fields or a few big ones. This can also be divided into discrete chunks, for example on a 32 bit processor, these might be 4 byte units.
In this case, `B` uses a single unit, and so does `C12345678` , but any larger and two units would be used.
If the limit is violated, in such a way as to cause an error, a `TOO_BIG` error must be given in response.
(A receiver could, for instance, only check validity of echo commands, before doing a basic copy, removing the limit).  

The big field can be in one of two forms: String or Hex.  

In hex form, the big field is a series of paired hex digits, beginning with a `+` character. 
	An odd number of digits is considered an error, although a blank field is allowed.  

In string form, the field is a series of bytes, encoded literally, beginning and ending with a `"` character. 
	Any `"` characters in the string should be escaped with a `\`, and any `\` characters should also be escaped similarly.
	Any new lines in the string should be replaced with a `\` followed by an `n`.  
		This refers literally to a character 10 (line feed) NOT a character 13 (carriage return), character 13 can be left alone.  
		If there is a character 10 and character 13, only the 10 needs replacing, not the 13.
		This is the standard behaviour in C, C++, Java,  `\n` referring to character 10, although some languages may not do this. This is irrelevant for text, but will be relevant for binary data.
	
The two forms both provide the same information, and are interchangeable when using commands, although string is generally more compact, but for binary data, hex is more readable.
A receiver has a maximum length of big field it can accept, and must give a `TOO_BIG` error in response (unless for whatever reason it is not an issue) when this is violated.

Spaces and tabs can be ignored where they turn up (outside of strings), and must be discarded between fields, while generators may only place them between fields.
For compatibility any carriage return characters (character 13, `\r` in C, C++, Java) must be ignored when between fields.

There is no defined ordering to fields, including the long field, so `R20A+10` and `+10AR20` are equivalent, 
	although it is recommended to place the command identifier at the beginning, and the long field at the end, to make it more readable.

### Command Sequences

`&` separated commands are executed strictly in order, with nothing being executed between them. 
	If an error occurs in one of these commands, the rest of `&` seperated sequence.
	Single trailing `&` characters must be considered an error and must get the response `S3`, and pairs of `&` similarly.
	`&` separated commands must be executed in a way that appears atomic to the rest of the system, 
	this means that no other command may be executed between two `&` separated commands 
	(unless it would have no effect on them, such as by using other resources, which have not been, and will not be, used in this command sequence).  
	This also means that any commands received over any channel (including execution space) must be executed as a single unit until a new line or end of packet is reached. 
	This is to avoid situations where I2C style 'write then read' being two commands are split up.
	The responses to two commands, separated by a `&`, must be separated by a `&`. 

`|` separated commands are considered error handlers, only executed if a previous command failed, otherwise they are skipped entirely. 
	A `|` begins an error handler for the entire command sequence, and following `&` seperated commands are considered part of the error hander.
	Following `|` seperated commands are an error handler for the error handler. A `|` based error handler is only executed if a `S10` error is hit. 
	Any other error is considered to catastrophic to continue at all.
	To clarify, given 5 commands, which only respond with `S`, unless they fail:  
	`Rx1&Rx2|Rx3&Rx4|Rx5\n`  
	if all succeed: `S&S&S\n`  
	if Rx2 fails with S10: `S&S10|S&S\n`  
	if Rx1 fails with S10: `S10|S&S\n`  
	if Rx1 fails with Sx: `Sx\n` (Sx means anything other than `S0` or `S10`)  
	if Rx1, Rx3 fail with S10: `S10|S10|S\n`  
	if Rx2, Rx4 fail with S10: `S&S10|S&S10|S\n`  
	... etc.  
	These commands are primarily to give extra information on a failure when one occurs, e.g. in an execution space command.
	The point at which the first command after the `|` is executed must be seperated by a `|` as seen in the above example.
	If a parsing error is encountered after the `|`, and this is never executed, then the error cannot be reported, so should be reported on the debug channel.

`\n` separated command sequences should be executed in order, but other command sequences can be put between them. 
	If an error occurs in one sequence, the next should still be executed. A new packet (over packet based channels such as UDP) is treated effectively the same as a `\n`.
	The `\n` refers in this context to character 10 (line feed), as is the default in C, C++, Java.
	The responses to two consecutive commands, which were separated by a new packet, must be in separate packets.
	If a packet based channel contains `\n` separated commands, the responses may be contained within separate packets, 
	but it is preferred to respond separated by `\n` in one packet (other separators are not allowed). Stream channels must respond with `\n` separated responses.

Even though the `\n` separated command sequences should be executed in the order they arrive, due to packet delays over channels like UDP, the order the sender sends them in may not be the same. 
	To mitigate this, packets should be buffered and grouped, and these groups only sent after the previous response / responses have been received. 
	Retry must also be implemented, unless provided in the protocol, and some way of coping with total communication loss (such as the cable being unplugged).


### Responses

Every command must be given a response, including empty commands (reset commands are exceptions to this, as there can be no response).
	This response must contain a status field, with the letter `S`. This is zero if the command succeeded, and non-zero for a failure.
An S response of `S10` indicates an command specific failure, whereas a lower (non-zero) code indicates an invalid command/parse error.  
	A parse error can be treated differently, and may, for example, halt execution in an execution space, where a non-parse error would not.
A command failure response does not need to follow the format for the successful command response, and can, 
	for instance, use the big field string form to give detailed failure information.
	For this reason the use of fields is not generally specified any failure condition, except for the `S` field (although commands may have specific mandated failure modes, which do have dictated structure).  
The E field of a command must always be echoed by a receiver if present, even in the event of command failure, to allow sender to give command numbers, which can be checked on reception of the response.
The E field does not need to be literally copied, as long as the value stored is the same (so `E00` can be replaced with `E`).  
An empty command is always given the response `S3`, although empty command seqences are given blank responses:

|Command| Response	|  
|-------|-----------|  
|		|			|  
|&		|S3			|  
|&&		|S3			|  
||		|S3			|  
|*		|*			|  
|* %	|*			|  
|* &	|*S3		|  
|E5		|E5S3		|  

The following status responses are defined:  
|Response		|Value  |Meaning|  
|---------------|-------|-------|  
|OK				|0		|	Command executed normally|  
|RESP_TOO_LONG	|1		|	Command response cannot be given in full, too large|  
|UNKNOWN_ERROR	|2		|	Command failed in unexpected way|  
|PARSE_ERROR	|3		|	Command not syntactically valid|  
|UNKNOWN_CMD	|4		|	Command not known by receiver|  
|MISSING_PARAM	|5		|	Command known, but required field not present|  
|BAD_PARAM		|6		|	Field in command has invalid value|  
|NOT_ACTIVATED	|7		|	receiver has not yet received activate command|  
|TOO_BIG 		|8		|	Big field too long for receiver|  
|CMD_FAIL		|10		|	Command specific failure, e.g. pin set command on non-existent pin|  
|NOT_EXECUTED	|ff		|	Reserved for sender side, indicates that command could not be executed, to give to subcomponents expecting a response.|  



### Execution Space



The receiver can create a block of memory which it can use to store commands. This is called the execution space, and should be run, if active, 
	whenever no other commands are available, although other commands should get priority.
	When an execution space command sequence has been begun, it must be completed before other commands can be executed, unless the commands are able to be run in parallel.
	This space must be managed by the sender, although to simplify management the `begin execution` RCode may be used to jump to an address in this block of memory.
	This command must be implemented so as to jump immediately, without executing any later commands, and be treated as an immediate new line, 
	starting at the new address as it would at a new command sequence.  

Commands run in the execution space must do so 'quietly' meaning their responses are not reported, unless a command fails. 
If a command fails with an `S10` status, then execution continues, other failures must result in the execution being stopped, as the space may be corrupted.
The response is sent as a notification, and with A `Z2` notification type. 
For the command `Rx1&Rx2&Rx3|Rx4&Rx5|Rx6`  
	if Rx1 failed:	`!Z2S10|S&S`  
	if Rx2 failed:	`!Z2S&S10|S&S`  
	if Rx1 and Rx3 failed:	`!Z2S10|S10|S`  
	etc...  
	if Rx2 gave too large a response to fit in the buffer (e.g.+1111111111111111111111111111...), and Rx3 failed:  
		`!Z2S&S1&S10`  
	This response is given because Rx2 could not be fitted, so instead the sequence is stopped, and the S1 is given. 
	Then the failure status is then given. Any other responses before the one which could not fit are also given.
	if on this occasion Rx5 gave a fatal error, e.g. S5:  
		`!Z2S&S1&S5`  
	would be the response, as the last error is given. No other error could be given as S5 would skip the error handlers.  
	As a special case, if it was Rx1 which could not fit:
		`!Z2&S1&S10`  
	The first command has no S status as there was no command.
	This would also be the response even if Rx1 were the one to fail.

	
### Parallel Execution

The receiver must execute commands coming over any given command channel in exactly the order they arrived in (this rule does not apply to execution space commands, so multiple can be run at once, provided that they are executed once each per full loop), 
	however, no such rules apply between different channels.
The receiver may, if it wishes, and two given command sequences strictly do not use the same resources, execute two commands in parallel, 
	although the resources must not be shared at any point up to the next new line on both commands, so an entire command sequence may have to be parsed, 
	or at least scanned for resource use in order to avoid collisions, to simplify the process, the receiver can apply a limit on lookahead, but it MUST NOT execute the sequence in parallel with any other if the limit is exceeded.
	these resources include things like the I2C channels, pins, and other peripherals, but also include the execution space, 
	so that the execution space lock must be acquired to execute a store, a state or an execution command, and cannot be acquired if a different execution space command sequence is being executed
		(although an execution space command can run these, but only if no other execution space commands are running, optionally, multiple execution space commands can be run simultaneously, 
		it must also be guaranteed that a jump must mean the next command sequence is not executed, although the current one can be finished, so that the execution follows the jump as if it were sequential, this should be guaranteed through the locking described above).
	The execution command does not need to lock the interrupt system, but any interrupt set commands must, as must any execution store commands. 
		These commands must be runnable through the interrupt code, but must wait for other interrupts, and execution space command sequences to finish, before acquiring the lock.
	This would all work using read-write locks.
	To perform an interrupt handler, read on interrupt must be acquired.
	To perform execution space, read on execution space must be acquired.
	To perform execution command, write on execution space must be acquired.
	To perform execution state, read on execution space must be acquired.
	To perform interrupt set, write on interrupt must be acquired.
	To perform execution store, write on execution space AND write on interrupt must be acquired.
	If something has the write lock, it always has the read lock as well (so an interrupt thread can write to execution space)
	For performance benefits, the lock state can be checked before the lock is acquired, 
	so an execution space command can check if it can acquire the lock, then scan the command, then acquire the lock.

A command which begins with a % marker (after a * marker if present) can be run in parallel with other commands. 
Any command not begining with such a marker cannot be run in parallel, to avoid weird errors in code that would work fine on one system, due to another being parallel.

This means that these cannot be in parallel: "R00&R00&R10P4V0\n" and "R00&R01E58&R11P4\n" or any other similar pair, 
	however "R10P4V0" and "R11P5" can, provided it can be guaranteed that one command will not effect the other.
	The purpose of the parallel system is to allow high performance MCUs (like ARMs) running low performance buses (like I2C) to use more of their capabilities. 
	Since performing GPIO operations is almost instant on most MCUs, these would most likely not be done in parallel.
	The absolute priority in implementing any parallel execution on the receiver is avoiding deadlocks and avoiding breaking the atomicity of a command sequence, on the resources it uses. 
	If there is any risk of either of these being breached, parallel execution MUST NOT BE USED.  
	Generally a parallel system can operate mostly sequentially unless a really slow command (e.g. I2C) appears.


### Notifications

To allow fast communication of information from receiver, a notification may be generated, this is effectively a response to a command not sent, 
	it must begin with a `!`, and must contain a `Z` field in the first command, indicating the type of the notification. 
	The notification is considered over after the end of a packet, or at a new line.  
	On a packet based system, the notifications should not be sent in the same packet as a different response.   
  * `Z00` indicates a reset notification. This is sent by any receiver which has reset, and still retains its notification host information.  
		This will contain the effective response to an `R00` command.
  * `Z01` indicates a interrupt from an interrupt source, and will contain basic info on the interrupt, or more as detailed below.
  * `Z02` indicates a execution space notification, meaning a command has failed in the space, and will contain the output of the failed command, 
		and all preceding `&`/`|` separated commands, if they can fit, otherwise a `RESP_TOO_LONG` status.
			
##### Interrupts
The receiver has 3 possible responses to a interrupt (an interrupt would usually be some form of line feeding to the receiver from some other device, e.g. the SMBus alert system):
  1.  Generate a notification about what indicates the interrupt occurred, e.g. interrupt on bus 0.
  2.  Attempt to find the address of the source of the interrupt. This is the case for standard SMBus alert systems, 
	  and this would be fed back, along with the information given in 1, in a notification
  3.  Use some form of interrupt vector for this source of interrupt (given by an interrupt vector set command) to jump into the execution space, 
	  and report the information given in 2, and the responses to the commands at the vector.

If an interrupt vector is not present, or there is no execution space, the 1st or 2nd form of interrupt must be used.
These principles apply to all bus forms, in whatever way possible (on some buses there may only be one possible source per bus, so the 2nd form is redundant).
An example of an I2C bus 2 interrupt, from a device with address 0x44, and handler writing then reading 1 byte from I2C, with content 0x22 (padded with spaces to make more readable):   
  1.  "Z1 I2 T S"
  2.  "Z1 I2 T S& B2 A44 S"
  3.  "Z1 I2 T S& B2 A44 S& A44 I T1 S& +22 S"

The form 3 allows fast responses to interrupts, even when the RCode is sent over a protocol with high latency.
The form 3 can deal with the interrupt entirely, or simply collect data for the sender to deal with, depending on how predicable the required response is, 
	it can also be used to perform some immediate remedial action, until the sender deals with it properly.
	In form 3 interrupt handlers, execution will continue until a new line is hit, if an execution command is hit, it sets the location for the normal execution, but does not execute a 'jump'.

Both form 2 and 3 require use of resources, so must acquire the locks on those buses 
	(this can be a problem in non-parallel systems, so a flag must be set, and the execution handled through a normal channel, rather than checking immediately). 
	Even basic SMBus style alert resolution could interfere with other communication. 
	Hence these actions should be simply considered command channels like any other, only with very high priority.

The response fields are as follows:

|Form			|Field  |Meaning												|  
|---------------|-------|-------------------------------------------------------|  
|1				|I		|The bus number of the interupt, if no action taken to resolve, otherwise not present|  
|1				|T		|The interupt type/source								|  
|1				|S		|Should always be 0, otherwise indicates failure		|  
|---------------|-------|-------------------------------------------------------|  
|2				|B		|The bus number of the interupt, useful if action needed|  
|2				|A		|The address of the device within the bus				|  
|2				|S		|Should be 0, but will be 10 if could not resolve address|  
|---------------|-------|-------------------------------------------------------|  
|3				|*		|Whatever comes from command							|  


The UART system uses notifications to indicate 'buffer full' conditions, and 'matched byte' conditions.

### Debug

The RCode receiver can also output debug information over any given channel (usually defaulting to something like serial or USB), 
	this information can be of any form, but must begin every line with a `#`, and be either new line, or packet separated from RCode data, ideally in its own packet on packet based channels.
	The receiver can also support the `debug host set` command, which can also disable debug information, and generally should start with debug disabled if it uses a shared communication protocol, such as UDP, to avoid jamming the channel.
	The sender can choose either to ignore such data, or log it in some form (including simply printing it, or writing it to a file). 

### Peripheral requirements

##### GUID/MAC

The RCode receiver can also choose to allow storage of a GUID, and must allow storage of a MAC address if networking is enabled. 
	These are stored somewhere in the persistent memory, along with a byte before and after, which must sum to 0xff, 
	this is to ensure that on first boot, the values are ignored, and considered invalid.
	The first check byte is also incremented every time a write is performed, before the write (wrapping so as to not include 0x00 or 0xff), 
	and the end check byte decremented after the write, so that if the write was incomplete, the value is invalid. If the value is invalid, it should be ignored.

##### I2C

The RCode receiver, if it supports I2C, must do its best to avoid bus locking (where the slave misses one or more clock signals during a read, and jams the bus), but in the event it occurs:
	Should attempt, if possible to recover, by clocking SCL until SDA is released, the sending a stop bit (releasing SDA while SCL floats high)
	Or if this is not possible (e.g. uncooperative Arduino libraries), must reset using watchdog, and recognise the SDA low, SCL high condition on startup, and, as above, release the bus.
	The first approach is preferable, but the number of attempts taken must be reported back, with the second approach, no such reporting is necessary, as the sender must recognise the reset notification, or send a retry.
	If a watchdog reset is performed like this, then the receiver must start up not activated, so as to not perform half commands, relying on other status which may have been lost.
The I2C bus system works both for systems with multiple I2C ports, and those with bus repeaters which can be switched off. This means a device with only 1 I2C port can have multiple buses.
	The bus repeater system is referred to below as 'switching', and the I2C port itself as the 'unswitched' side. 
	This is relevant when using I2C as a command channel, as the command channel must not be able to be switched off, as then communication would be impossible.
	Hence the channel must be connected to the unswitched side of any of the I2C ports, although any of the buses switched from there can be specified, they will be treated equivalently.


### Commands

Each command must contain a command code (except empty commands), indicated by the 'R' field, otherwise a `UNKNOWN_CMD` error must be given. If this is less than 0x10, then it is a system command.
Upon startup, an RCode receiver must be in a `not activated` state, and respond to any non-system command with an `NOT_ACTIVATED` error.

Any command which is addressed as a broadcast, and is intended to find RCode receivers which have not already been discovered, 
	must begin with a `*` symbol, as must the response to such a command, this is to allow these commands to occur out of synch with other commands, and be identified separately.

Every command code must be unique, and cannot contain the response fields `S`, `Z` or `E`, or the parameter field `E` (Echo command is an exception)

Key for Values column (xx means a particular number):  

|Content | Meaning															|  
|-------|-------------------------------------------------------------------|  
|xx		| Field can only take this value									|  
|xx?	| Field can only take this value, but might not be present			|  
|xx,xx	| Field can only take one of these values							|  
|xx,xx?	| Field can only take one of these values, but might not be present	|  
|*		| Any number														|  
|**		| Any number of any length											|  
|*?		| Any number or not present											|  
|**?	| Any number of any length or not present							|  
|?		| The field is either present, or not, and its value is irrelevant	|  
|b		| Binary field: 01 or 00											|  


|Command		|	Type		|	Field	|	Values	|	Meaning  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Ident			|				|			|			|	Self identification  
|				|	Command		|	R		|	00		|  
|				|	Resp		|	A		|	*		|	Major version  
|				|	Resp		|	B		|	*		|	Minor version  
|				|	Resp		|	W		|	*		|	Watchdog reset number  
|				|	Resp		|	big		|	**		|	text id (e.g. "Arduino/Alpha")  
|				|	Resp		|	S		|	00		|  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Echo			|				|			|			|	Direct, literal echo  
|				|	Command		|	R		|	01		|  	
|				|	Param		|	*		|	**?		|	any parameter (including E, Z, S)  
|				|	Resp		|	*		|	**?		|	the same as parameters, with same values (including E, Z)  
|				|	Resp		|	S		|	**?		|	the same as given S, if present, otherwise 00 (the primary use of this is testing error responses)  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Capabilities	|				|			|			|	Responds with the capabilities of the RCode receiver  
|				|	Command		|	R		|	02		|  
|				|	Param		|	P		|	*?		|	'page number' selects which set of supported commands/capabilities to look at. The total set is the union of all the pages,   
|				|				|			|			|		with the big fields combined (this means the non-big fields can also change with page, provided all needed are present on some page), defaults to 0.  
|				|	Resp		|	C		|	*		|	Binary capability field: [?, ?, Notification, Debug, interrupt handling, parallel execution, single packet newline separated response, GUID]  
|				|	Resp		|	B		|	**		|	Maximum big field length (should always be more than around 8, just to allow basic functionality)  
|				|	Resp		|	N		|	**		|	Maximum field number (should always be more than around 5, as otherwise basic commands will not function)  
|				|	Resp		|	F		|	*		|	Maximum field size, maximum number of bytes given to a field.  
|				|	Resp		|	U		|	*		|	Field Unit size, minimum number of bytes given to a field.  
|				|	Resp		|	P		|	**?		|	Size of persistent memory, if not present, not supported. If =0, then only have GUID, MAC  
|				|	Resp		|	M		|	*		|	Number of pages  
|				|	Resp		|	big		|	**		|	list, in binary, of supported RCodes, e.g. +0001020304, supports Ident, Echo, Capabilities, Activate, and Soft reset  
|				|				|			|			|		From this list it must be possible to infer what capabilities are supported in terms of peripherals, as peripherals which appear must be supported.  
|				|				|			|			|		1 byte commands must come first, then 2, then 3, etc. Before any n > 1 byte commands are sent, n `00`s must be sent.
|				|	Resp		|	S		|	00,06	|	gives BAD_PARAM if page number exceeds last page  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Activate		|				|			|			|	Activates the receiver, allowing non-system commands  
|				|	Command		|	R		|	03		|  
|				|	Resp		|	A		|	b		|	was activated previously (1 if already set)  
|				|	Resp		|	S		|	00		|  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Soft Reset		|				|			|			|	Performs a soft reset  
|				|	Command		|	R		|	04		|  
|				|	Resp		|	None	|			|	has no response whatsoever, as receiver has reset  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Hard Reset		|				|			|			|	Performs a hard reset  
|				|	Command		|	R		|	05		|  
|				|	Resp		|	None	|			|	has no response whatsoever, as receiver has reset  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Fetch GUID		|				|			|			|	Fetches the 16 byte GUID  
|				|	Command		|	R		|	06		|  
|				|	Resp		|	S		|	00,10	|	fails if the GUID is not set correctly  
|				|	Resp		|	big		|	**		|	The GUID, if valid  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Notification Host set|				|			|			|	Sets the target of the notifications, to the sending system on the sending channel (e.g. a particular UDP port on a particular address on a network)  
|				|	Command		|	R		|	08		|  
|				|	Resp		|	S		|	00,10	|  fails if the channel cannot do notifications  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|	Set Debug	|				|			|			|	Sets the target of the debug info, to the sending system on the sending channel (e.g. a particular UDP port on a particular address on a network)  
|	Host		|	Command		|	R		|	09		|  
|				|	Param		|	D		|	?		|	If present enables Debug, otherwise, disable (if possible)  
|				|	Resp		|	S		|	00		|  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Make Code		|				|			|			|	Creates and stores a pseudo-random 4 byte code  
|				|	Command		|	R		|	0a		|  
|				|	Resp		|	big		|	**		|	The code created  
|				|	Resp		|	S		|	00		|  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Match Code		|				|			|			|	Matches the code created with Make Code  
|				|	Command		|	R		|	0b		|  
|				|	Param		|	big		|	**		|	The expected random code  
|				|	Resp		|	S		|	00,10	|	fails if given code not the same as stored code  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|	Channel		|				|			|			|	Re-initialises the specified command channel (only resets the first in order given here, defaults to current)  
|	reset		|	Command		|	R		|	0c		|  
|				|	Param		|	A		|	p		|	Resets all channels, guaranteed no response  
|				|	Param		|	C		|	p		|	Resets current connection, guaranteed no response  
|				|	Param		|	N		|	p		|	Resets all network ports (sets MAC address, etc.)  
|				|				|			|			|  
|				|	Resp		|	S		|	00		|	has no response whatsoever if used channel has reset  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Persist		|				|			|			|	Stores the given data at the given position in the receiver's persistent store  
|				|	Command		|	R		|	10		|  
|				|	Param		|	A		|	**		|	Address to store the data at  
|				|	Param		|	big		|	**		|	Data to store  
|				|	Resp		|	S		|	00		|  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Fetch Persisted|				|			|			|	Fetches the data at the given position in the receiver's persistent store  
|				|	Command		|	R		|	11		|  
|				|	Param		|	A		|	**		|	Address to read the data from  
|				|	Param		|	L		|	*		|	Length of data to read  
|				|	Resp		|	big		|	**		|	Data read  
|				|	Resp		|	S		|	00,06	|	Gives BAD_PARAM if too much data requested  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Set MAC address|				|			|			|	Stores the given MAC address at the location specified internally by the RCode receiver, so that on startup, the right MAC address will be chosen  
|				|	Command		|	R		|	12		|		Also adds the two 'check bytes' on the start and end, and performs the increment.  
|				|	Param		|	big		|	**		|	MAC address. Must be exactly 6 bytes  
|				|	Resp		|	S		|	00,06	|	Gives BAD_PARAM if wrong length big field given  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Set GUID		|				|			|			|	Stores the given GUID at the location specified internally by the RCode receiver, so that on startup, the right Fetch GUID command will work  
|				|	Command		|	R		|	13		|  
|				|	Param		|	big		|	*		|	GUID to store, must be exactly 16 bytes.  
|				|	Resp		|	S		|	00,06	|	Gives BAD_PARAM if wrong length big field given  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Execution State|				|			|			|	Returns the state of the execution space  
|				|	Command		|	R		|	20		|  
|				|	Resp		|	D		|	*		|	Current delay value  
|				|	Resp		|	F		|	b		|	0->no error   1->halted with system error (S < 0x10)  
|				|	Resp		|	G		|	b		|	0->not running   1->running  
|				|	Resp		|	L		|	**		|	Execution space current used length  
|				|	Resp		|	M		|	**		|	Execution space total length  
|				|	Resp		|	S		|	00		|  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Execution		|				|			|			|	Starts or stops execution, and sets execution address  
|				|	Command		|	R		|	21		|  
|				|	Param		|	A		|	**?		|	new current address (default: unchanged)  
|				|	Param		|	D		|	*?		|	Sets execution delay to (`2^value` ms), e.g. 5 gives 32ms delay   (default: unchanged)  
|				|	Param		|	G		|	?		|	If present, begins executing, otherwise, halts  
|				|	Resp		|	S		|	00		|  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Execution Store|				|			|			|	Stores in the execution space  
|				|	Command		|	R		|	22		|  
|				|	Param		|	A		|	**		|	Start address of write  
|				|	Param		|	L		|	?		|	If present, end is considered the last section of data, and execution will loop.  
|				|	Param		|	big		|	**		|	The new code to store at the address (in literal binary, good to use string form for)  
|				|	Resp		|	S		|	00,08	|	Gives TOO_BIG if cannot fit the new code, and writes nothing.  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Interrupt Setup	|				|			|			|	Reports capabilities of interrupt system back, and allows setup of interrupt system  
|				|	Command		|	R		|	28		|  
|				|	Resp		|	I		|	*		|	Total number of assignable interrupts  
|				|	Resp		|	big		|	**		|	Bytes for interrupt types, any included interrupt types can have interrupts assigned  
|				|	Resp		|	S		|	00		|  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Interrupt Set	|				|			|			|	Sets the jump location of an interrupt handler within the execution space.  
|				|	Command		|	R		|	29		|  
|				|	Resp		|	J		|	**		|	Address to execute from in execution space (can be outside of normal execution space length).  
|				|	Resp		|	A		|	*?		|	Device address on bus (if bus supports addressing)  
|				|	Resp		|	I		|	*		|	Bus number for target interrupt  
|				|	Resp		|	T		|	*		|	Interrupt type for target interrupt  
|				|	Resp		|	S		|	00		|  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|	Pin Setup	|				|			|			|	Reports capabilities of Pins system back  
|				|	Command		|	R		|	40		|  
|				|	Param		|	P		|	*?		|	'page number' selects which set of supported commands/capabilities to look at. The total set is the union of all the pages,   
|				|				|			|			|		with the big fields combined (this means the non-big fields can also change with page, provided all needed are present on some page), defaults to 0.  
|				|	Param		|	N		|	?		|	If present, indicates sets notifications to be created on interrupt.  
|				|	Param		|	big		|	**		|	4 bits per pin, enables interrupts, selects source, sets value: [both edge, compare value/edge select, int on change, int en], (int on change, if 1, interrupt on edge, otherwise on compare)  
|				|				|			|			|		(compare value, if interrupt on compare, if value different to this, interrupt; if interrupt on edge, if 1, rising edge, 0, falling)   
|				|				|			|			|		(both edge: if in edge mode, if this bit is set, ignore edge select, interrupt on either edge)   
|				|				|			|			|		These 4 bit commands given in the order (as appearing in long field): [pin 3, pin 2], [pin 1, pin 0]  
|				|	Resp		|	P		|	*		|	Total number of GPIO pins  
|				|	Resp		|	N		|	?		|	If present, indicates notifications are available  
|				|	Resp		|	I		|	?		|	If present, indicates interrupts are available  
|				|	Resp		|	M		|	*		|	Number of pages  
|				|	Resp		|	big		|	**		|	1 byte for each IO: (PU->pull-up   PD->pull-down   OD->open-drain): [output, input, input PU, input PD, output OD, output OD-PU, output OD-PD, Analog In] set bits indicate a supported capabilities  
|				|				|			|			|		in order in message: [pin 3], [pin 2], [pin 1], [pin 0]  
|				|	Resp		|	S		|	00,06	|	gives BAD_PARAM if page number exceeds last page  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Pin Set		|				|			|			|	Sets the output value of a pin  
|				|	Command		|	R		|	41		|  
|				|	Param		|	P		|	**		|	The pin number (not on the package, but the logical pin number)  
|				|	Param		|	M		|	0,1,2?	|	The new pin mode: 	0->in   	1->out   	2->input pull-up    3->input pull-down    4->output open drain    5->output open drain pull-up    6->output open drain pull-down    none->no change  
|				|	Param		|	V		|	0,1?	|	The new pin output: 0->low   	1->high  		none->no change  
|				|	Resp		|	P		|	**		|	Same as P given  
|				|	Resp		|	V		|	0,1?	|	Same as V given (or not present if V not given)  
|				|	Resp		|	S		|	00,05	|	gives MISSING_PARAM if P not present.  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Pin Get		|				|			|			|	Gets the output value of a pin  
|				|	Command		|	R		|	42		|  
|				|	Param		|	P		|	**		|	The pin number (not on the package, but the logical pin number)  
|				|	Param		|	A		|	?		|	Read mode: 	present->analog   	not->digital  
|				|	Resp		|	P		|	**		|	Same as P given  
|				|	Resp		|	V		|	0,1,*	|	if digital read, value of pin 	0->low   1->high  if analog: entire value, left aligned (so if we have a 12 bit A-D, we left shift 4 bits)  
|				|	Resp		|	S		|	00,05	|	gives MISSING_PARAM if P not present.  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|Pin Compare	|				|			|			|	Compares the value of a pin with a given value.  
|				|	Command		|	R		|	43		|  
|				|	Param		|	P		|	**		|	The pin number (not on the package, but the logical pin number)  
|				|	Param		|	A		|	?		|	Read mode: 	present->analog   	not->digital  
|				|	Param		|	V		|	0,1,*	|	expected value of pin
|				|	Resp		|	P		|	**		|	Same as P given  
|				|	Resp		|	V		|	0,1,*	|	if digital read, value of pin 	0->low   1->high	if analog, most significant byte of value  
|				|	Resp		|	S		|  00,05,10 |	gives MISSING_PARAM if P not present, CMD_FAIL if pin value != expected value.  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|	I2C Setup	|				|			|			|	Does setup of I2C system, and reports capabilities of I2C system back  
|				|	Command		|	R		|	50		|  
|				|	Param		|	F		|  0,1,2,3	|	Set I2C frequency (0->10kHz   1->100kHz   2->400kHz   3->1000kHz)  
|				|	Param		|	N		|	?		|	If present, indicates sets notifications to be created on interrupt.  
|				|				|			|			|  
|				|	Resp		|	N		|	?		|	If present, indicates notifications are available  
|				|	Resp		|	I		|	?		|	If present, indicates interrupts are available  
|				|	Resp		|	C		|	*		|	Binary capability field: [?, ?, Low Speed 10kHz, restart, individual bus interrupt, bus interrupt, bus free without restart, SMBus compatible]  
|				|	Resp		|	B		|	**		|	Number of I2C buses  
|				|	Resp		|	F		|	1,2,3	|	Maximum frequency (1->100kHz   2->400kHz   3->1000kHz), receivers have to support all frequencies below their maximum  
|				|	Resp		|	S		|	00,06	|	gives BAD_PARAM frequency above maximum  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|I2C Send		|				|			|			|	Sends an I2C message over the specified bus, or the previous bus used if unspecified  
|				|	Command		|	R		|	51		|  
|				|	Param		|	A		|	*		|	I2C address  
|				|	Param		|	B		|	*?		|	Target bus  
|				|	Param		|	C		|	?		|	If present, will continue on failure, useful for probing addresses, where failure is expected.  
|				|	Param		|	T		|	*?		|	Number of retries, defaults to 5.   (Set to 1 if repeating the command would be bad)  
|				|	Param		|	big		|	**		|	Data to send to target address and bus  
|				|	Resp		|	A		|	*		|	I2C address (Echoed)  
|				|	Resp		|	I		| 0,1,2,3,4	|	I2C status response: 0->ok  1->data too long  2->address nack  3->data nack  4->other    (standard responses from arduino wire library)  
|				|	Resp		|	T		|	*		|	Attempts taken (1 means worked first time)  
|				|	Resp		|	S		| 00,06,10	|	CMD_FAIL if I2C status non-zero, and continue on error disabled, `BAD_PARAM` on restart with no good next command.  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|I2C Read		|				|			|			|	Reads an I2C message over the specified bus, or the previous bus used if unspecified (Retries are harder, as auto increment, and bus locking become issues, hence the default to 1)  
|				|	Command		|	R		|	52		|  
|				|	Param		|	A		|	*		|	I2C address  
|				|	Param		|	B		|	*?		|	Target bus  
|				|	Param		|	T		|	*?		|	Number of retries, defaults to 1 (will always work for address NACK, but for data errors, there is little recovery if bus freeing requires reset).  
|				|	Param		|	L		|	*		|	Length of data to read  
|				|	Resp		|	big		|	**		|	Data received, will be zero length if failed  
|				|	Resp		|	T		|	*		|	Attempts taken (1 means worked first time)  
|				|	Resp		|	S		| 00,10,06?	|	CMD_FAIL if I2C failed, BAD_PARAM if length too long for big field, or restart with invalid next command. Can result in reset and no response, if bus jams and bus freeing requires reset.  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|I2C Send + Read|				|			|			|	Sends, then reads an I2C message over the specified bus, or the previous bus used if unspecified (Retries are harder, as auto increment, and bus locking become issues, hence the default to 1)  
|				|	Command		|	R		|	53		|  
|				|	Param		|	A		|	*		|	I2C address  
|				|	Param		|	B		|	*?		|	Target bus  
|				|	Param		|	L		|	?		|	Length to read
|				|	Param		|	T		|	*?		|	Number of retries for send, defaults to 5. (No retries supported for read, as then atomicity is broken)  
|				|	Param		|	big		|	**		|	The data to send.  
|				|	Resp		|	big		|	**?		|	Data received.  
|				|	Resp		|	T		|	*		|	Attempts taken for send (1 means worked first time)  
|				|	Resp		|	S		| 00,06,10?	|	CMD_FAIL if I2C failed for send or read. Can result in reset and no response, if bus jams and bus freeing requires reset.  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|I2C Read And Compare|				|			|			|	Reads an I2C message over the specified bus, or the previous bus used if unspecified (Retries are harder, as auto increment, and bus locking become issues, hence the default to 1)  
|				|	Command		|	R		|	54		|  
|				|	Param		|	A		|	*		|	I2C address  
|				|	Param		|	B		|	*?		|	Target bus  
|				|	Param		|	C		|	?		|	If present, will continue on difference, useful for getting multiple results.  
|				|	Param		|	T		|	*?		|	Number of retries, defaults to 1 (will always work for address NACK, but for data errors, there is little recovery if bus freeing requires reset).  
|				|	Param		|	big		|	**		|	set of mask|value pairs, for each read position i, expect (read[i] & field[2*i]==field[2*i+1]) So value must have blank spaces where mask not present.  
|				|	Resp		|	big		|	**?		|	Data received, not present if no difference.  
|				|	Resp		|	D		|	*?		|	index of first difference, not present if no difference.  
|				|	Resp		|	T		|	*		|	Attempts taken (1 means worked first time)  
|				|	Resp		|	S		| 00,06,10?	|	CMD_FAIL if I2C failed, or difference detected (and continue on diff disabled)`BAD_PARAM` on restart with no good next command.   
|				|				|			|			|		Can result in reset and no response, if bus jams and bus freeing requires reset.  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|USB-C PD Status|				|			|			|	Gives general info on power delivery status  
|				|	Command		|	R		|	1110	|  
|				|	Resp		|	I		|	**		|	Agreed current, 10mA units  
|				|	Resp		|	V		|	*		|	Agreed voltage, integer (acts as minimum)  
|				|	Resp		|	A		|	**		|	Minimum voltage, 50mV units  
|				|	Resp		|	M		|	**?		|	Maximum voltage, 50mV units (not given if same as minimum)  
|				|	Resp		|	P		|	*		|	Power Status  
|				|	Resp		|	S		|   00		|  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|USB-C PD Set	|				|			|			|	Requests a new power set-up  
|				|	Command		|	R		|	1111	|  
|				|	Param		|	I		|	**		|	Target current, 10mA units  
|				|	Param		|	V		|	*		|	Target voltage, integer (acts as minimum if maximum specified)  
|				|	Param		|	A		|	**		|	Target Minimum voltage, 50mV units (takes priority)  
|				|	Param		|	M		|	**?		|	Target Maximum voltage, 50mV units (if not given, assumes same as minimum)  
|				|	Resp		|	S		|   00		|  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|USB-C PD Source|				|			|			|	Gives static Source info  
|				|	Command		|	R		|	1112	|  
|				|	Param		|	N		|	*?		|	Which PDO to give info on (0 indexed)  
|				|	Resp		|	I		|	**		|	If N given, the max current of that PDO, 10mA units  
|				|	Resp		|	V		|	*		|	If N given, the integer minimum voltage  
|				|	Resp		|	A		|	**		|	If N given, the minimum voltage, 50mV units  
|				|	Resp		|	M		|	**?		|	If N given, the maximum voltage, 50mV units (not given if same as minimum)  
|				|				|			|			|	All following are given if N not present:  
|				|	Resp		|	I		|	**		|	8 bytes: 4 bytes of XID, 2 bytes VID, 2 bytes PID  
|				|	Resp		|	V		|	**		|	2 bytes: 1st byte firmware version, 2nd byte hardware version  
|				|	Resp		|	R		|	*		|	The voltage regulation field, as from extended source cap  
|				|	Resp		|	C		|	**		|	6 bytes: each 2 bytes are the three peak currents, 3 then 2 then 1  
|				|	Resp		|	F		|	b?		|	If not present, no external supply. If present - 1: unconstrained, 0: constrained  
|				|	Resp		|	G		|	*		|	The touch current field, as from extended source cap  
|				|	Resp		|	T		|	*		|	The touch temperature field, as from extended source cap (only the touch temp bits)  
|				|	Resp		|	B		|	**		|	The battery count, first byte is hotswappable, second is fixed (only present if has batteries)
|				|	Resp		|	P		|	*		|	The source PDP rating
|				|	Resp		|	A		|	?		|	If present, source is usb coms capable
|				|	Resp		|	U		|	?		|	If present, source has unconstrained power
|				|	Resp		|	O		|	?		|	If present, source requires USB suspend
|				|	Resp		|	big		|	**		|	The manufacturer info string (in string form, null removed) 
|				|	Resp		|	S		|   00,06	|   gives BAD_PARAM if given N >= #of PDOs
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|USB-C PD Source Status|				|			|			|	Gives changing Source info  
|				|	Command		|	R		|	1113	|  
|				|	Resp		|	H		|	*?		|	The temperature status: Normal = 1, Warning = 2, OverTemperature = 3 (if not present, then not supported)  
|				|	Resp		|	F		|	*?		|	The Flags, as given by Event flags in the USB-C status message (if not present, then not supported)  
|				|	Resp		|	T		|	*?		|	The temperature in degrees C - (0 implies temp<=1) (if not present, then not supported)  
|				|	Resp		|	L		|	*?		|	The limiting factor on power - as in Status message the power status field  
|				|	Resp		|	D		|	*		|	Power indicator status - as in the Power state change field  
|				|	Resp		|	A		|	b?		|	If present: has external power, if 1: has external AC power (otherwise has DC)  
|				|	Resp		|	G		|	?		|	If present: has non battery internal power source (e.g. solar)  
|				|	Resp		|	B		|	*?		|	The present battery input field from status message - if not present, no internal batteries  
|				|	Resp		|	P		|	*		|	The power status from power state change field - not present if not supported
|				|	Resp		|	V		|	**		|	The output voltage of the PPS, (not present implies not supported) - 20mV units
|				|	Resp		|	I		|	**		|	The output current of the PPS, (not present implies not supported) - 50mA units
|				|	Resp		|	S		|   00,10	|   gives CMD_FAIL if no status is known  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  





##### Interrupt sources:  

| #	| Source 	|  
|---|-----------|  
| 0 | I2C		|  
| 1 | SPI		|  
| 2 | UART		|  
| 3 | Parallel	|  
| 4 | USB		|  
| 5 | UDP		|  
| 6 | TCP		|  
| 7 | Pin		|  
| 8 | CAN		|  
	




##### Suggested new ordering:  
  
| Name				| R number 	|  
|-------------------|-----------|  
| ***SYSTEM***		|			|  
| *Core*			|			|  
| Ident				| 00		|  
| Echo				| 01		|  
| Capabilities		| 02		|  
| Activate			| 03		|  
| Soft reset		| 04		|  
| Hard reset		| 05		|  
| Fetch GUID		| 06		|  
| *Channel*			|			|  
| Notification host	| 08		|  
| Debug host		| 09		|  
| Make code			| 0a		|  
| Match code		| 0b		|  
| Channel reset		| 0c		|  
| ***Peripheral***	|			|  
| *Persistence*		|			|  
| Persist			| 10		|  
| Fetch Persisted	| 11		|  
| Set MAC			| 12		|  
| Set GUID			| 13		|  
| *Execution space*	|			|  
| Execution State	| 20		|  
| Execution 		| 21		|  
| Execution Store	| 22		|  
| Interrupt Setup	| 28		|  
| Interrupt Set		| 29		|  
| *Pin*				|			|  
| Pin setup			| 40		|  
| Pin set			| 41		|  
| Pin get			| 42		|  
| Pin compare		| 43		|  
| *I2C*				|			|  
| I2C Setup			| 50		|  
| I2C Send			| 51		|  
| I2C Read			| 52		|  
| I2C Send + Read	| 53		|  
| I2C Read + Compare| 54		|  
| *UART* 			| 70-7f		|  
| UART Setup		| 70		|  
| UART Send			| 71		|  
| UART Read			| 72		|  
| UART Available	| 73		|  
| ***Future***		|			|  
| *SPI*				|			|  
| SPI Setup			| 60		|  
| SPI Send			| 61		|  
| SPI Read			| 62		|  
| SPI Send + Read(simultanious)			| 63	|  
| SPI Read + Compare					| 64	|  
| SPI Send,Read,Compare (simultanious)	| 65	|  
| *User defined*	| f0-ff		|  
| ***2 byte***		|			|  
| *USB-C PD*		|			|  
| USB-C PD Status	| 1110		|  
| USB-C PD Set		| 1111		|  
| USB-C PD Source	| 1112		|  
| USB-C PD Source Status | 1113		|  
| *User defined*	| ff00-ffff	|  

#### USB-C PD Power Status:


| Value	| Meaning 							|  
|-------|-----------------------------------|  
| 0	    | Negotiating						|  
| 2	    | Unresponsive Source				|  
| 3	    | Source Cannot provide voltage		|  
| 4	    | New Voltage Contract Made			|  
| 5	    | New Voltage, Insufficient Current	|  
| 6	    | Source cannot provide current		|  
| 7	    | Hard reset immenent				|  
| 8	    | Over-current Alert				|  
| 9	    | Over-temperature Alert			|  
| 10    | Over-voltage Alert				|  


### General principles of command design:

The level of abstraction in a command should reflect the level of complexity of the system it represents, and multiple commands may be made to represent different abstraction levels.
The command's requirements on fields should also reflect the level of complexity of the system, as devices with more complex subsystems are likely to be more capable generally - hence pin commands are very simple to implement, whereas USB-C PD commands are not.

The interface a set of commands provide must be abstract enough to work cross-device, so that it can be implemented on a different device later on. 
Also if RCode is being used to control a system with a specific purpose, then commands should be made to fit the purpose, 
as this reduces the risk of accidentally breaking a system - this may include limiting the pins available for pin commands to those not used anywhere else, or even not providing low level interfaces on a production system. 
This avoids some of the risks surrounding a malicious (or simply incompetant) device, connected to the same network, from causing any damage to the receiver. 
It is reasonable to expect the device to know the circuit surrounding it, and have its programming reflect this, although it is encouraged that such customisations are well parameterised where possible, to allow changes to the circuit without a total re-design.


### Possible example command structure (can be changed to suit requirements):

  
|Command		|	Type		|	Field	|	Values	|	Meaning  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|	SPI Setup	|				|			|			|	Does setup of SPI system, and reports capabilities of SPI system back  
|				|	Command		|	R		|	60		|  
|				|	Param		|	F		|  	*		|	Set SPI frequency (f*100kHz (f=0 -> 10kHz))  
|				|	Param		|	C		|	?		|	Set up a command channel on the SPI port, will select any slave configured ports available.  
|				|	Param		|	B		|	*		|	Which slave SPI port to use for command channel  
|				|	Param		|	big		|	**		|	Contains the map of SS inversion, bit by bit, (..., [SS15, SS14, SS13, SS12, SS11, SS10, SS9, SS8], [SS7, SS6, SS5, SS4, SS3, SS2, SS1, SS0]) 1 means active low  
|				|				|			|			|  
|				|	Resp		|	C		|	*		|	Binary capability field: [?, ?, cross command SS, 3 Wire mode, SPI command channel, transmit while receive, individual bus interrupt, bus interrupt]  
|				|	Resp		|	I		|	**		|	Number of SPI SS lines  
|				|	Resp		|	F		|	*		|	Maximum frequency (f*100kHz), receivers have to support all frequencies below their maximum  
|				|	Resp		|	S		|	00,06	|	gives BAD_PARAM frequency above maximum  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
  
|Command		|	Type		|	Field	|	Values	|	Meaning  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  
|	SPI Send	|				|			|			|	Sends SPI message to specified slave  
|				|	Command		|	R		|	60		|  
|				|	Param		|	D		|	*?		|	Sets data size in bits(will fail with BAD_PARAM if data size not supported, pads to be full bytes on send/read), defaults to 8 bit  
|				|	Param		|	S		|	*?		|	Target Slave (defaults to previous)  
|				|	Param		|	W		|	?		|	If present, uses three wire mode, default 4  
|				|	Param		|	E		|	?		|	If present, does not release SS, (if next command is not SPI, or packet ends here, will fail `BAD_PARAM`)... not supported on all receivers.  
|				|	Param		|	big		|	**		|	Data to send to target  
|				|	Resp		|	T		|	*		|	Attempts taken (1 means worked first time)  
|				|	Resp		|	S		| 	00,06	|  
|---------------|---------------|-----------|-----------|---------------------------------------------------------------------  










