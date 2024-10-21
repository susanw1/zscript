Feature: Is it Friday yet?
    AS a human
    I WANT to know if it's a Friday, or any specified day
    SO THAT I can make plans for the weekend

    Background:
        Given all this
        """
        Loads of text here
        """
        And an "A"
        And a "b"

    Rule: days are unique experimental

        Scenario: Friday is Friday 1
            Given today is Friday
            When I ask whether it's Friday yet
            Then I should be told Yay

        Scenario: Checking something where the given assumption fails 1
            Given today is Friday afternoon
            When I ask whether it's Friday yet
            Then I should be told Yay


        Scenario: Friday is not Friday 1
            Given today is Tuesday
            And a big table
                | z | animal | part  |

                | a | frog   | eye   |
                | b | dog    | brain |
            When I ask whether it's Friday yet
            Then I should be told Nope

    Rule: days are still unique experimental 1

#    Scenario: Checking for Thursday
#        Given today is <day1>
#        When I ask whether it's <day2> yet
#        Then I should be told <result>

        Scenario Outline: Checking the day 1
            Given today is <day1>
            When I ask whether it's <day2> yet
            Then I should be told <result>

            @Boo
            Examples:
                | day1   | day2    | result |
                | Sunday | Friday  | Nope   |
                | Sunday | Sunday  | Yay    |
                | Friday | Friday  | Yay    |
                | Friday | Tuesday | Nope   |
