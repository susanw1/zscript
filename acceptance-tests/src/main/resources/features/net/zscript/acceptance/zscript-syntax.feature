Feature: Zscript Syntax Consistency
    AS A Zscript developer
    I WANT a Zscript commands to be parsed in a consistent manner
    SO THAT I can depend on basic functionality

    Background:
        Given a connection to the target

    @Syntax-error
    Scenario: should reject unterminated text big-field
        When I send exactly "Z1\"abc" as a command sequence to the connection, and capture the response
        Then connection response #0 has status UNTERMINATED_STRING

    @Syntax-error
    Scenario: should reject odd digit big-field
        When I send exactly "Z1+1" as a command sequence to the connection, and capture the response
        Then connection response #0 has status ODD_LENGTH

    @Standard-operation
    Scenario: should allow multiple big-fields
        When I send exactly "Z1A2+74B3\"uv\"+77" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "S A2 B3 \"tuvw\""

    @Standard-operation
    Scenario: should allow escaping in text big-fields
        When I send exactly "Z1\"a=4eb\"" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "S \"aNb\""

    @Syntax-error
    Scenario: should reject badly formed escapes embedded in text big-fields
        When I send exactly "Z1\"a=4Bb\"" as a command sequence to the connection, and capture the response
        Then connection response #0 has status ESCAPING_ERROR

    @Syntax-error
    Scenario: should reject badly formed escapes as sole value in text big-fields
        When I send exactly "Z1\"=4B\"" as a command sequence to the connection, and capture the response
        Then connection response #0 has status ESCAPING_ERROR

    @Syntax-error
    Scenario: should reject unterminated escapes in text big-fields
        When I send exactly "Z1\"a=4\"" as a command sequence to the connection, and capture the response
        Then connection response #0 has status ESCAPING_ERROR

    @Syntax-error
    Scenario: should reject unterminated escapes in text big-fields
        # The quotes string finishes before the escape is complete
        When I send exactly "Z1\"a=4\"" as a command sequence to the connection, and capture the response
        Then connection response #0 has status ESCAPING_ERROR

    @Standard-operation
    Scenario: should allow single UTF-8 multibyte in text big-fields
        When I send exactly "Z1\"a£b\"" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "S \"a£b\""

    @Standard-operation
    Scenario: should allow Chinese UTF-8 ultibyte in text big-fields
        When I send exactly "Z1\"机器的兴起\"" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "S +e69cbae599a8e79a84e585b4e8b5b7"

    @Syntax-error
    Scenario: should disallow locking mid-sequence
        When I send exactly "Z1 %1" as a command sequence to the connection, and capture the response
        Then connection response #0 has status INVALID_KEY

    @Syntax-error
    Scenario: should disallow locking mid-sequence
        When I send exactly "Z1 & %1 Z2" as a command sequence to the connection, and capture the response
        Then connection response #1 has status INVALID_KEY

    @Standard-operation
    Scenario: should allow locking at start of sequence
        When I send exactly "%1 Z1" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "S"

    @Syntax-error
    Scenario: should disallow echoed line number mid-sequence
        When I send exactly "Z1 _1" as a command sequence to the connection, and capture the response
        Then connection response #0 has status INVALID_KEY

    @Syntax-error
    Scenario: should disallow echoed line number mid-sequence
        When I send exactly "Z1 & _1 Z2" as a command sequence to the connection, and capture the response
        Then connection response #1 has status INVALID_KEY

    @Standard-operation
    Scenario: should allow echoed line number at start of sequence
        When I send exactly "_1 Z1" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "S"
