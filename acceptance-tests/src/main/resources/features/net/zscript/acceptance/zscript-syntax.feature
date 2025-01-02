Feature: Zscript Syntax Consistency
    AS A Zscript developer
    I WANT a Zscript commands to be parsed in a consistent manner
    SO THAT I can depend on basic functionality

    @Syntax-error
    Scenario: should reject unterminated text big-field
        Given a connection to the target
        When I send exactly "Z1\"abc" as a command sequence to the connection
        Then connection should answer with response element #0 containing status UNTERMINATED_STRING

    @Syntax-error
    Scenario: should reject odd digit big-field
        Given a connection to the target
        When I send exactly "Z1+1" as a command sequence to the connection
        Then connection should answer with response element #0 containing status ODD_LENGTH

    @Standard-operation
    Scenario: should allow multiple big-fields
        Given a connection to the target
        When I send exactly "Z1A2+74B3\"uv\"+77" as a command sequence to the connection
        Then connection should answer with response element #0 should match "S A2 B3 \"tuvw\""

    @Syntax-error
    Scenario: should disallow locking mid-sequence
        Given a connection to the target
        When I send exactly "Z1 %1" as a command sequence to the connection
        Then connection should answer with response element #0 containing status INVALID_KEY

    @Syntax-error
    Scenario: should disallow locking mid-sequence
        Given a connection to the target
        When I send exactly "Z1 & %1 Z2" as a command sequence to the connection
        Then connection should answer with response element #1 containing status INVALID_KEY

    @Standard-operation
    Scenario: should allow locking at start of sequence
        Given a connection to the target
        When I send exactly "%1 Z1" as a command sequence to the connection
        Then connection should answer with response element #0 should match "S"

    @Syntax-error
    Scenario: should disallow echoed line number mid-sequence
        Given a connection to the target
        When I send exactly "Z1 _1" as a command sequence to the connection
        Then connection should answer with response element #0 containing status INVALID_KEY

    @Syntax-error
    Scenario: should disallow echoed line number mid-sequence
        Given a connection to the target
        When I send exactly "Z1 & _1 Z2" as a command sequence to the connection
        Then connection should answer with response element #1 containing status INVALID_KEY

    @Standard-operation
    Scenario: should allow echoed line number at start of sequence
        Given a connection to the target
        When I send exactly "_1 Z1" as a command sequence to the connection
        Then connection should answer with response element #0 should match "S"
