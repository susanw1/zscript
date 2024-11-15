Feature: Zscript Device Mirroring
    AS A Zscript client developer
    I WANT to define a Device
    SO THAT my code has entities that represent the physical hardware architecture

    Background:
        Given a locally running zscript device
        And a connection to that local device
        And a connected device handle

    Scenario: Simple textual command responds with text
        When I send "Z1A7" as a command to the device
#        Then device should answer with response sequence "A7S"
        Then device should answer with response status 0 and field A = 7

    Scenario: Command Sequence sent, and Response Sequence returns
        Given a "capabilities" command from the "Core" module in "net.zscript.model.modules.base"
        And it has field "VersionType" set to 3
        When I send the command to the device and receive a response
        Then it should include a "Status" field set to 0
        And it should include a "Version" field set to 0
#        And it should include a bigfield "Ident" set to "amd64 Linux"
