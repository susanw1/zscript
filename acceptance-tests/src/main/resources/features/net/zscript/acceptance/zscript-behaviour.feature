Feature: Zscript Behaviour Consistency
    AS A Zscript developer
    I WANT a Zscript commands to work in a consistent manner
    SO THAT I can depend on basic behaviour

    Background:
        Given a connection to the target

    Scenario: Simple command (sanity check)
        When I send exactly "Z1A" as a command sequence to the connection
        Then connection should answer with response element #0 should match "S0 A"

    @Semantic-failure
    Scenario: should return error when numeric fields are duplicated
        When I send exactly "Z1A1A2" as a command sequence to the connection
        Then connection should answer with response element #0 containing status REPEATED_KEY

    @Semantic-failure
    Scenario: should return error when command has unrecognised Z command
        # this is flaky - what if a device defines Zf? This should do Capabilities and find a gap.
        When I send exactly "Zf" as a command sequence to the connection
        Then connection should answer with response element #0 containing status COMMAND_NOT_FOUND
