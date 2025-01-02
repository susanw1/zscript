Feature: Zscript Connection
    AS A Zscript client developer
    I WANT a connection to send commands and return responses
    SO THAT I can perform basic sanity checking and logic

    Background:
        Given a connection to the target

    Scenario: Echo Command responds
        When I send exactly "Z1A5" as a command sequence to the connection
        Then connection should receive exactly "!A5S" in response

    Scenario: Two Echo Command responds
        When I send exactly "Z1A1 & Z1A2" as a command sequence to the connection
        Then connection should receive exactly "!A1S&A2S" in response
