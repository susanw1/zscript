package com.wittsfamily.rcode_acceptance_tests.core_rcode;

import static com.wittsfamily.rcode_acceptance_tests.acceptancetest_asserts.RCodeAcceptanceTestAssert.assertThatCommand;

import com.wittsfamily.rcode_acceptance_tests.RCodeAcceptanceTestConnectionManager;

import io.cucumber.java.en.When;

public class GeneralParseErrorSteps {

    @When("the target is sent an echo command with the same field twice")
    public void the_target_is_sent_an_echo_command_with_the_same_field_twice() {
        CoreRCodeSteps.testAssert = assertThatCommand(RCodeAcceptanceTestConnectionManager.getConnections().get(0), "R1A10B30A\n");
    }

    @When("the target is sent an command with no R field")
    public void the_target_is_sent_an_command_with_no_r_field() {
        CoreRCodeSteps.testAssert = assertThatCommand(RCodeAcceptanceTestConnectionManager.getConnections().get(0), "A10B30\n");
    }

    @When("the target is sent an command with an unrecognised R field")
    public void the_target_is_sent_an_command_with_an_unrecognised_r_field() {
        CoreRCodeSteps.testAssert = assertThatCommand(RCodeAcceptanceTestConnectionManager.getConnections().get(0), "R110304\n");
    }

    @When("the target is sent an command with two big fields")
    public void the_target_is_sent_an_command_with_two_big_fields() {
        CoreRCodeSteps.testAssert = assertThatCommand(RCodeAcceptanceTestConnectionManager.getConnections().get(0), "R1+84\"d\"\n");
    }

    @When("the target is sent an command with an unterminated big string")
    public void the_target_is_sent_an_command_with_an_unterminated_big_string() {
        CoreRCodeSteps.testAssert = assertThatCommand(RCodeAcceptanceTestConnectionManager.getConnections().get(0), "R1\"d\n");
    }

    @When("the target is sent an command with an odd digit big field")
    public void the_target_is_sent_an_command_with_an_odd_digit_big_field() {
        CoreRCodeSteps.testAssert = assertThatCommand(RCodeAcceptanceTestConnectionManager.getConnections().get(0), "R1+84d\n");
    }

    @When("the target is sent an command with an escaped quote in a string")
    public void the_target_is_sent_an_command_with_an_escaped_quote_in_a_string() {
        CoreRCodeSteps.testAssert = assertThatCommand(RCodeAcceptanceTestConnectionManager.getConnections().get(0), "R1\"\\\"d\"\n");
    }

    @When("the target is sent an command with a non hex character in the wrong place")
    public void the_target_is_sent_an_command_with_a_non_hex_character_in_the_wrong_place() {
        CoreRCodeSteps.testAssert = assertThatCommand(RCodeAcceptanceTestConnectionManager.getConnections().get(0), "R1A0g\n");
    }

}
