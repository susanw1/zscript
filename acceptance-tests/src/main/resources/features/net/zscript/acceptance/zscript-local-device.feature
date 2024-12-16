Feature: Zscript Device Representation
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
        Given the capabilities command from the Core module in net.zscript.model.modules.base
        And it has the VersionType field set to 0
        When I send the command sequence to the device and receive a response sequence
        Then it should include a Status field set to 0
        And it should include a Version field set to 1
        And it should include a bigfield Ident set to ""
        And there should be no additional responses


    Scenario: Two Command Sequences sent, and Response Sequences returned
        Given the capabilities command from the Core module in net.zscript.model.modules.base
        And it has the VersionType field set to enum value UserFirmware
        And if successful, it is followed by the echo command
        And it has the field key 'A' set to 0x14

        When I send the command sequence to the device and receive a response sequence

        Then it should include a Status field set to 0
        And it should include a Version field set to 1
        And it should include a bigfield Ident set to ""
        And as successful, there should be a following response
        And it should include a Status field set to 0
        And it should include a field key 'A' set to 0x14
        And there should be no additional responses
