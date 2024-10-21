Feature: Zscript Echo Command
    AS A Zscript user
    I WANT the echo command to work
    SO THAT I can perform basic sanity checking and logic

    Background:
        Given a connection to a zscript device

    Scenario: Echo Command responds
        When I send an echo command
        Then I should receive an echo response
