Feature: Zscript Local Device Representation for Java Receiver
    AS A Zscript client developer
    I WANT to run a local Java Receiver
    SO THAT I can easily test commands on a reference implementation

    Background:
        Given a locally running zscript receiver
        And a connection to that local receiver
        And a device handle connected to the connection

    Scenario: Simple textual command responds with text
        When I send "Z1A7" as a command to the device
#        Then device should answer with response sequence "A7S"
        Then device should answer with response status value 0 and field A = 7


    Scenario: Command Sequence sent using generated command API, and Response Sequence is processed with generated response parser
        Given the capabilities command from the Core module in net.zscript.model.modules.base
        And it has the VersionType field set to 0
        When I send the command sequence to the target and receive a response sequence
        Then it should include a Status field set to 0
        And it should include a Version field set to 1
        And it should include a Ident string-field set to ""
        And there should be no additional responses


    Scenario: Two Commands in sequence sent using generated command API, and Response Sequences is processed with generated response parser
        Given the capabilities command from the Core module in net.zscript.model.modules.base
        And it has the VersionType field set to enum value UserFirmware
        And if successful, it is followed by the echo command
        And it has the field key 'A' set to 0x14
        When I send the command sequence to the target and receive a response sequence
        Then it should include a Status field set to 0
        And it should include a Version field set to 1
        And it should include a Ident string-field set to ""
        And having succeeded, there should be a following response
        And it should include a Status field set to 0
        And it should include a field key 'A' set to 0x14
        And there should be no additional responses


    Scenario: Two Command Sequences sent, and Response Sequences matching as required
        Given the capabilities command from the Core module in net.zscript.model.modules.base
        And it has the VersionType field set to enum value UserFirmware
        And if successful, it is followed by the echo command
        And it has the field key 'A' set to 0x14
        When I send the command sequence to the target and receive a response sequence
        Then it should match "C3117 M1 V1 S I"
        And having succeeded, there should be a following response
        And it should match "A14 S"
        And there should be no additional responses
