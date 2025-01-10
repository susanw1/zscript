Feature: Zscript Connection
    AS A Zscript client developer
    I WANT a connection to send commands and return responses
    SO THAT I can connect to target devices

    Background:
        Given a connection to the target

    Scenario: Echo Command responds
        When I send exactly "Z1A5" as a command sequence to the connection, and capture the response
        Then connection response is exactly "!A5S"

    Scenario: Two Echo Command responds
        When I send exactly "Z1A1 & Z1A2" as a command sequence to the connection, and capture the response
        Then connection response is exactly "!A1S&A2S"
