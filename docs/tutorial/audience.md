Audience
===

We anticipate that the developers interacting with Zscript will fall into one or more of these general roles. As we've been designing and developing it, we have tried to focus on who has responsibility for what, in order that the right people have power to do the things they need to do.

Core Devs
---

This is us. We invented it and gave life to it, and we're keeping it alive.

Responsible for:
* Designing the language and defining the specification 
* Implementing the generic parsers to comply with the specification
* Providing a test suite to enable conformance checking
* Setting out the rules, and deciding what Zscript _is_ and what it is _not_

In short, Core Devs build the foundations of Zscript.

Porting Devs
---
These are any developers who make Zscript work on some platform. This might involve implementation, for example, on small PICs or powerful ARMs and RiscVs systems, in assembler or C++ or Rust, on top of a HAL or embedded OS or just bare-metal, with enough capability to run at least the basic commands.

Or, they may be implementing new clients on new systems or in new languages.

Responsible for:
* Providing a Zscript implementation on a specific platform
* Ensuring that the parser and core modules work to spec and pass the tests
* Curating module implementations to work on the platform

In short, Porting Devs expand the availability of Zscript to new devices and architectures.


Module Devs
---
These are developers who define and standardize modules, and get them working on some subset of Zscript platforms. Examples of such modules in future might include: gyros, CAN bus, navigation devices and GNSS, DCC model railways, power control, displays and graphics, etc.

Responsible for:
* Defining the capabilities, commands and fields
* Ensuring that the module is useful

In short, Module Devs expand what Zscript can do.


Embedded App Devs
---

These are firmware developers who have a system to build, who could choose to use some other system but choose to use Zscript to implement some part of what they are doing. They are, to some extent, our target end-users, whose lives we want to make better. 

Responsible for:
* Choosing their platform - CPU, OS, hardware
* Creating and setting up core Zscript objects: command channels and executor
* Creating main.c or equivalent, and prodding the Zscript executor
* Adding, enabling and configuring modules
* Designating device assets: pins, interrupts, DMAs
* Assigning addresses to downstream subdevices, as required
* Writing their code as new user-defined modules

In short, Zscript provides capabilities to App Devs without getting in their way.

Client Devs
---

These are developers working on control software who want to make their machines do what they are meant to do. They are writing applications in Python or Java or Unix shell-script, and want the hardware to work as an extension of their software.

Responsible for:
* Importing the appropriate client library
* Using it - it should "just work"

In short, Zscript lets Client Devs do things easily that are normally hard.

