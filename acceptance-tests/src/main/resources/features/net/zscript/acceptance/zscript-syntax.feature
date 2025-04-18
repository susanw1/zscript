Feature: Zscript Syntax Consistency
    AS A Zscript developer
    I WANT a Zscript commands to be parsed in a consistent manner
    SO THAT I can depend on basic functionality

    Background:
        Given a connection to the target

    @Standard-operation
    Scenario: should allow up to 4 hex numeric digits in numeric fields
        When I send exactly "Z1 A Ba C8f D3d4 E1a2b" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "S A Ba C8f D3d4 E1a2b"

    @Standard-operation
    Scenario: should allow up to 4 leading zeroes in numeric fields
        When I send exactly "Z1 A0000 B000a C08f D03d4 E1a2b" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "S A Ba C8f D3d4 E1a2b"

    @Syntax-error
    Scenario: should disallow odd length, >4 hex digits in numeric fields
        When I send exactly "Z1 A12345" as a command sequence to the connection, and capture the response
        Then connection response #0 has status ODD_LENGTH

    @Syntax-error
    Scenario: should reject unterminated text string-field
        When I send exactly "Z1 A\"abc" as a command sequence to the connection, and capture the response
        Then connection response #0 has status UNTERMINATED_STRING

    @Standard-operation
    Scenario: should allow string-fields
        When I send exactly "Z1A2C74B3D\"tu\"E\"vw\"" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "S A2 B3 C74 D\"tu\" E\"vw\""

    @Standard-operation
    Scenario: should allow multiple empty string-fields
        When I send exactly "Z1 A\"\" B\"\"" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "SAB"

    @Standard-operation
    Scenario: should allow escaping in text string-fields
        When I send exactly "Z1 X\"a=4eb\"" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "S X\"aNb\""

    @Syntax-error
    Scenario: should reject upper-case escapes embedded in text string-fields
        When I send exactly "Z1 X\"a=4Bb\"" as a command sequence to the connection, and capture the response
        Then connection response #0 has status ESCAPING_ERROR

    @Syntax-error
    Scenario: should reject badly formed escapes as only value in text string-fields
        When I send exactly "Z1 X\"=4x\"" as a command sequence to the connection, and capture the response
        Then connection response #0 has status ESCAPING_ERROR

    @Syntax-error
    Scenario: should reject unescaped '=' in text
        When I send exactly "Z1 X\"1+1=2\"" as a command sequence to the connection, and capture the response
        Then connection response #0 has status ESCAPING_ERROR

    @Standard-operation
    Scenario: should allow escaped '=' in text
        When I send exactly "Z1 X\"1+1=3d2\"" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "S X 31 2b 31 3d 32 "

    @Syntax-error
    Scenario: should reject unterminated escapes in text fields
#        # The quotes string finishes before the '=xx' escape is complete
        When I send exactly "Z1 X\"a=4\"" as a command sequence to the connection, and capture the response
        Then connection response #0 has status ESCAPING_ERROR

    @Standard-operation
    Scenario: should allow single UTF-8 multibyte in text fields
        When I send exactly "Z1 X\"a£b\"" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "S X\"a£b\""

    @Standard-operation
    Scenario: should allow Chinese UTF-8 multibyte in text fields
        When I send exactly "Z1 X\"机器的兴起\"" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "S Xe69cbae599a8e79a84e585b4e8b5b7"

    @Standard-operation
    Scenario: should reject disallowed chars in text fields
        When I send exactly "Z1 X\"=0a\"" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "S X0a"


    @Standard-operation
    Scenario: should allow locking at start of sequence
        When I send exactly "%1 Z1" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "S"

    @Syntax-error
    Scenario: should disallow locking within a command
        When I send exactly "Z1 %1 A2" as a command sequence to the connection, and capture the response
        Then connection response #0 has status INVALID_KEY

    @Syntax-error
    Scenario: should disallow locking mid-sequence
        When I send exactly "Z1 & %1 Z2" as a command sequence to the connection, and capture the response
        Then connection response #1 has status INVALID_KEY

    @Standard-operation
    Scenario: should allow echoed line number at start of sequence
        When I send exactly ">1 Z1" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "S"

    @Syntax-error
    Scenario: should disallow echoed line number within a command
        When I send exactly "Z1 >1 A2" as a command sequence to the connection, and capture the response
        Then connection response #0 has status INVALID_KEY

    @Syntax-error
    Scenario: should disallow echoed line number mid-sequence
        When I send exactly "Z1 & >1 Z2" as a command sequence to the connection, and capture the response
        Then connection response #0 has status SUCCESS
        Then connection response #1 has status INVALID_KEY


    @Syntax-error
    Scenario Outline: should disallow illegal keys (forbidden by the Tokenizer syntax)
        When I send exactly "<cmd>" as a command sequence to the connection, and capture the response
        Then connection response #0 has status ILLEGAL_KEY
        Examples:
            | cmd         | desc                     |
            | Z1 A\"\" b2 | lc hex chars forbidden   |
            | Z1 £1 A2    | non-base-ascii forbidden |
            | Z1 B\"\" 1  | non-base-ascii forbidden |

    @Syntax-error
    Scenario Outline: should disallow invalid keys (allowed as keys, but not in their supplied location)
        When I send exactly "<cmd>" as a command sequence to the connection, and capture the response
        Then connection response #0 has status INVALID_KEY
        Examples:
            | cmd    | desc                                                          |
            | Z1 A g | lc non-hex                                                    |
            | Z1 A z | lc non-hex                                                    |
            | Z1 :1  | theoretically ok but undefined                                |
            | Z1 >1  | text escape theoretically ok outside of quotes, but undefined |

    @Standard-operation
    Scenario Outline: should ignore spaces and commas except in text
        When I send exactly "<cmd>" as a command sequence to the connection, and capture the response
        Then connection response #0 is equivalent to "<resp>"
        Examples:
            | cmd                 | resp        |
            | Z1 A, 2 ,b 1 B\" \" | S0 A2b1 B20 |
            | Z1 A1,2,3           | S0 A123     |
            | Z1 A\",\"           | S0 A2c      |
