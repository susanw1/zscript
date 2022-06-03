Setting up Dev Environments and IDEs
====================================

These instructions should be helpful for any reasonable dev environment. Zcode was developed on PCs running Ubuntu 18.04-20.04. Other envs may be
supported more directly in future.

The project has components that build using a number of languages and tools, including some embedded compilers. However, to get a simple build working
you only need Java, Maven and a reasonable version of GCC. The embedded tools only become necessary if you enable corresponding build switches.

Instructions for Ubuntu
-----------------------

### Git

Git should Just Work. I'm using the default version on Ubuntu 20.04:
```
$ git --version
git version 2.25.1
```
And I cloned this project under `~/ZcodeProject`, so examples below generally reference `~/ZcodeProject/zcode` - adjust to taste. 


### Java

Install a JDK for at least Java 11. I'm using an Ubuntu (20.04) standard JDK (`apt install openjdk-11-jdk`):

```
$ java -version
openjdk version "11.0.15" 2022-04-19
OpenJDK Runtime Environment (build 11.0.15+10-Ubuntu-0ubuntu0.20.04.1)
OpenJDK 64-Bit Server VM (build 11.0.15+10-Ubuntu-0ubuntu0.20.04.1, mixed mode, sharing)
```
I use `update-alternatives` to select the right JDK (eg `update-alternatives --list java`).


### Maven

The project is built using Maven (which pulls in the Maven-NAR plug-in for compiling the C/C++ components), and it's the default one with Ubuntu (`apt install maven`):

```
$ mvn --version
Apache Maven 3.6.3
Maven home: /usr/share/maven
Java version: 11.0.15, vendor: Private Build, runtime: /usr/lib/jvm/java-11-openjdk-amd64
Default locale: en_GB, platform encoding: UTF-8
OS name: "linux", version: "5.4.0-109-generic", arch: "amd64", family: "unix"
```

You should be able to run `mvn install` from a command-line, and it should work.

### GCC

Any reasonably up-to-date GCC should be fine. Again, default Ubuntu should work:

```
$ gcc --version
gcc (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0
```

This is used for building the native Zcode core libs and running tests on it. The core code is a template header library, so dependent sub-projects
pull in the sources.

### Eclipse (Optional)

I use Eclipse for Zcode development. This is how it's set up:

```
Eclipse IDE for Java Developers (includes Incubating components)

Version: 2021-12 (4.22.0)
Build id: 20211202-1639
```

Including, for the record:
* M2E Maven Integration for Eclipse Core (1.19.0.20211116-0804)
* C/C++ Development Tools (Version: 10.6.0.202203091838 Build id: 20210915-0103)
* Cucumber Eclipse (1.0.0.202107150747)
* Git integration for Eclipse (6.0.0.202111291000-r)

Later versions of everything are probably fine. Earlier ones are at your own risk...

To set up, follow these steps (or equivalent) - they assume that you've cloned the project to `~/ZcodeProject/zcode`, and that `mvn install` works when run from that directory. In Eclipse:
1. Create new workspace, eg in `~/ZcodeProject/zcode-workspace`
1. From Project Explorer context menu (ie right-click it), choose "Import...", choose "Maven" > "Existing Maven Projects", and pick `~/ZcodeProject/zcode` as the Root Directory. 
1. It should show `zcode-all` - select it and all sub-projects. Finish the Import - the projects should all appear.
1. Additional step for C++ sub-projects (eg `generic-receiver-core`): 
    1. In Project Explorer, select the `generic-receiver-core` project item, right-click it and select "New" > "Other..." > "Convert to C/C++ Project (Adds C/C++ Nature)".
    1. Ensure that (Project Explorer, right-click on `generic-receiver-core`) Build Project and Clean Project both work - they should invoke the Makefile, which in turn runs Maven.
    1. Repeat this for any other C/C++ projects
1. Before making code changes, install the project formatters and tools, located in `misc/eclipse-profiles`:
    1.  Choose Window  > Preferences > Java > Code Style
    1.  For each of  _Formatter_ ,  _Clean Up_  and  _Import Order_ , "Import" the corresponding Zcode XML file, and Apply.
    1.  Choose Window  > Preferences > C/C++ > Code Style
    1.  Import the Zcode_cpp_format.xml settings file.
    

Target-Specific Toolchains
--------------------------
   
### ARM GCC

General help about getting ARM dev tools: https://askubuntu.com/questions/1243252/how-to-install-arm-none-eabi-gdb-on-ubuntu-20-04-lts-focal-fossa

Current (2022) ARM tools: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/downloads

I manually unpacked the Linux x86_64 one into `/opt/arm`:

```
$ ls /opt/arm/gcc-arm-none-eabi-10-2020-q4-major/
arm-none-eabi  bin  lib  share
```

And then created useful sym-links:

```
$ cd /opt/arm
$ sudo ln -s gcc-arm-none-eabi-10-2020-q4-major/ gcc-arm-none-eabi-current
$ sudo ln -s /opt/arm/gcc-arm-none-eabi-current/bin/arm-none-eabi-* /usr/bin
```

Enable the ARM bare metal "no-os" build with `mvn -Penable-arm-no-os ...`

### AVR (Arduino) GCC

TODO



    