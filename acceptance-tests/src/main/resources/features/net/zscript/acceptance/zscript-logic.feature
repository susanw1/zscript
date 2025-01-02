Feature: Zscript Logic Handling
    AS A Zscript client developer
    I WANT to chain commands using consistent logical sequencing operators
    SO THAT my commands are known to run in a predictable way

    Background:
        Given a device handle connected to the target


    Scenario Outline: Sequences of empty commands
        When the command sequence "<commands>" is sent to the target
        Then the response sequence should match "<responses>"

        # Empty command sequences joined by &, | and grouped ()
        Examples:
            | commands       | responses   | description                                        |
            |                | !           | empty command sequence                             |
            | &              | ! &         | 2x ANDed empty commands                            |
            | &&&            | ! &&&       | 4x ANDed empty commands                            |
            | \|             | ! \|        | 2x ORed empty commands                             |
            | \|\|\|         | ! \|\|\|    | 4x ORed empty commands                             |
            | ( )            | ! &&        | empty parens - implies ∅ & ( ∅ ) & ∅               |
            | ( & )          | ! &&&       | ANDed empty commands in parens                     |
            | ( \| )         | ! ( )       | ORed empty commands in parens                      |
            | ( \| ) &       | ! ( ) &     | ORed empty commands in parens with ANDed successor |
            | ( \| ) &( \| ) | ! ( ) & ( ) | 2x ORed empty commands in parens                   |
            | \| ) &( \|     | !   ) & (   | 2x ORed empty commands in minimal parens           |


    Scenario Outline: Sequences of successful echo commands
        When the command sequence "<commands>" is sent to the target
        Then the response sequence should match "<responses>"

        # Echo command sequences joined by &, | and grouped () - all successful
        Examples:
            | commands                            | responses                   | description                                                                         |
            | Z1A1                                | ! S0A1                      | single successful cmd, should just execute                                          |
            | Z1A1 & Z1A2                         | ! S0A1 & S0A2               | 2x ANDed cmds, should execute both                                                  |
            | Z1A1 & Z1A2 & Z1A3 & Z1A4           | ! S0A1 & S0A2 & S0A3 & S0A4 | 4x ANDed cmds, should execute all                                                   |
            | Z1A1 \| Z1A2                        | ! S0A1                      | 2x ORed cmds, should execute first only                                             |
            | Z1A1 \| Z1A2 \| Z1A3 \| Z1A4        | ! S0A1                      | 4x ORed cmds, should execute first only                                             |
            | ( Z1A1 )                            | ! & S0A1 &                  | Paren'ed cmd, should treat excess parens as implying empty cmd, "AND with ∅"        |
            | ( Z1A1 & Z1A2)                      | ! & S0A1 & S0A2 &           | Paren'ed ANDed cmds, should treat excess parens as implying empty cmd, "AND with ∅" |
            | ( Z1A1 \| Z1A2)                     | ! ( S0A1 )                  | ORed echo commands in parens                                                        |
            | ( Z1A1 \| Z1A2 ) & Z1A3             | ! ( S0A1 ) & S0A3           | ORed echo commands in parens with ANDed successor                                   |
            | ( Z1A1 \| Z1A2 ) & ( Z1A3 \| Z1A4 ) | ! ( S0A1 ) & ( S0A3 )       | 2x ORed echo commands in parens                                                     |
            | Z1A1 \| Z1A2 ) & ( Z1A3 \| Z1A4     | ! S0A1 ) & ( S0A3           | 2x ORed echo commands in minimal parens                                             |


    Scenario Outline: Logic sequences of AND/ORed success/failure echo commands
        When the command sequence "<commands>" is sent to the target
        Then the response sequence should match "<responses>"

        # Echo command sequences joined by &, | and grouped () - various combinations of failure using S2 "programmatic" fail
        Examples:
            | commands                                 | responses                      | description                                                       |
            | Z1A1 S2                                  | ! S2A1                         | single echo, failed                                               |
            | Z1A1 S2 & Z1A2                           | ! S2A1                         | 2x ANDed cmds, 1st failed, should skip 2nd                        |
            | Z1A1 & Z1A2 S2                           | ! S0A1 & S2A2                  | 2x ANDed cmds, 2nd failed, should run 1st and fail 2nd            |
            | Z1A1 S2 & Z1A2 & Z1A3 & Z1A4             | ! S2A1                         | 4x ANDed cmds, 1st failed                                         |
            | Z1A1 & Z1A2 & Z1A3 S2 & Z1A4             | ! S0A1 & S0A2 & S2A3           | 4x ANDed cmds, 3rd failed, should run only 1/2/3 with 3rd failing |
            | Z1A1 & Z1A2 & Z1A3 & Z1A4 S2             | ! S0A1 & S0A2 & S0A3 & S2A4    | 4x ANDed cmds, 4th failed, should run all with 4th failing        |
            | Z1A1 S2 \| Z1A2                          | ! S2A1 \| S0A2                 | 2x ORed cmds, 1st failed, should run both                         |
            | Z1A1 \| Z1A2 S2                          | ! S0A1                         | 2x ORed cmds, 2nd failed, 1st success should prevent others       |
            | Z1A1 \| Z1A2 \| Z1A3 S2 \| Z1A4          | ! S0A1                         | 4x ORed cmds, 3rd failed, 1st success should prevent others       |
            | Z1A1 S2 \| Z1A2 S2 \| Z1A3 S2 \| Z1A4 S2 | ! S2A1 \| S2A2 \| S2A3 \| S2A4 | 4x ORed cmds, ALL failed, should all execute                      |

    Scenario Outline: Logic sequences of AND/ORed success/failure echo commands with more complex parens
        When the command sequence "<commands>" is sent to the target
        Then the response sequence should match "<responses>"

        Examples:
            | commands                                   | responses                             | description                                                               |
            | ( Z1A1 S2)                                 | ! & S2A1                              | Single failing paren'ed cmd                                               |
            | ( Z1A1 S2 & Z1A2)                          | ! & S2A1                              | Paren'ed 2x ANDed cmds, 1st failed                                        |
            | ( Z1A1 & Z1A2 S2)                          | ! & S0A1 & S2A2                       | Paren'ed 2x ANDed cmds, 2nd failed                                        |
            | ( Z1A1 S2 \| Z1A2)                         | ! ( S2A1 \| S0A2 )                    | ORed echo commands in parens                                              |
            | ( Z1A1 S2 \| Z1A2 ) & Z1A3                 | ! ( S2A1 \| S0A2) & S0A3              | ORed echo commands in parens with ANDed successor                         |
            | ( Z1A1 S2 \| Z1A2 S2 ) & Z1A3              | ! ( S2A1 \| S2A2 )                    | ORed echo commands in parens with ANDed successor                         |
            | ( Z1A1 S2 \| Z1A2 ) & ( Z1A3 S2 \| Z1A4 )  | ! ( S2A1 \| S0A2 ) & ( S2A3 \| S0A4 ) | 2x ORed cmds in parens, ANDed, should run first pair and then second pair |
            | ( Z1A1 S2 \| Z1A2 ) \| ( Z1A3 S2 \| Z1A4 ) | ! ( S2A1 \| S0A2 )                    | 2x ORed cmds in parens, ORed; should run 1st pair and skip 2nd pair       |

    Scenario Outline: Logic sequences of AND/ORed echo commands including ERROR responses
        When the command sequence "<commands>" is sent to the target
        Then the response sequence should match "<responses>"

        Examples:
            | commands                    | responses       | description                                                     |
            | Z1A1 S13                    | ! S13A1         | Single errored cmd, should return status                        |
            | Z1A1 S13 & Z1A2             | ! S13A1         | Error before AND, should skip 2nd                               |
            | Z1A1 S13 \| Z1A2            | ! S13A1         | Error before OR, should skip 2nd                                |
            | Z1A1 S2 & Z1A2 S13 \| Z1A3  | ! S2A1 \| S0A3  | 3x cmds, 1st fails and error after AND, should run 1st+3rd only |
            | Z1A1 S2 \| Z1A2 S13 \| Z1A3 | ! S2A1 \| S13A2 | 3x cmds, 1st fails OR 2nd err, should run 1st+2nd only          |

    Scenario Outline: Logic sequences of AND/ORed echo commands including ERROR responses, in more complex parens
        When the command sequence "<commands>" is sent to the target
        Then the response sequence should match "<responses>"

        # note error expressions don't bother closing parens (instant death)
        Examples:
            | commands                                       | responses         | description                                                         |
            | ( Z1A1 S2 \| Z1A2 S13 ) & Z1A3                 | ! ( S2A1 \| S13A2 | ORed echo commands in parens with ANDed successor                   |
            | ( Z1A1 S2 \| Z1A2 S13 ) \| ( Z1A3 S2 \| Z1A4 ) | ! ( S2A1 \| S13A2 | 2x ORed cmds in parens, ORed; should run 1st pair and skip 2nd pair |
