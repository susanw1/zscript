package org.zcode.zcode_acceptance_tests.core_zcode;

import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.zcode.zcode_acceptance_tests.acceptancetest_asserts.ZcodeAcceptanceTestAssert.assertThatCommand;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import org.zcode.zcode_acceptance_tests.ZcodeAcceptanceTestConnectionManager;
import org.zcode.zcode_acceptance_tests.acceptancetest_asserts.ZcodeAcceptanceTestResponseStatus;

import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

public class GeneralParseErrorSteps {

    @When("the target is sent an echo command with the same field twice")
    public void the_target_is_sent_an_echo_command_with_the_same_field_twice() {
        CoreZcodeSteps.testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1A10B30A\n");
    }

    @When("the target is sent an command with no R field")
    public void the_target_is_sent_an_command_with_no_r_field() {
        CoreZcodeSteps.testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "A10B30\n");
    }

    @When("the target is sent an command with an unrecognised R field")
    public void the_target_is_sent_an_command_with_an_unrecognised_r_field() {
        CoreZcodeSteps.testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R110304\n");
    }

    @When("the target is sent an command with two big fields")
    public void the_target_is_sent_an_command_with_two_big_fields() {
        CoreZcodeSteps.testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1+84\"d\"\n");
    }

    @When("the target is sent an command with an unterminated big string")
    public void the_target_is_sent_an_command_with_an_unterminated_big_string() {
        CoreZcodeSteps.testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1\"d\n");
    }

    @When("the target is sent an command with an odd digit big field")
    public void the_target_is_sent_an_command_with_an_odd_digit_big_field() {
        CoreZcodeSteps.testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1+84d\n");
    }

    @When("the target is sent an command with an escaped quote in a string")
    public void the_target_is_sent_an_command_with_an_escaped_quote_in_a_string() {
        CoreZcodeSteps.testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1\"\\\"d\"\n");
    }

    @When("the target is sent an command with a non hex character in the wrong place")
    public void the_target_is_sent_an_command_with_a_non_hex_character_in_the_wrong_place() {
        CoreZcodeSteps.testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1A0g\n");
    }

    @When("the target is sent debug info")
    public void the_target_is_sent_debug_info() {
        CoreZcodeSteps.testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "#R1A2\n");
    }

    @When("the target is sent a broadcast command")
    public void the_target_is_sent_a_broadcast_command() {
        CoreZcodeSteps.testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "*R1A5\n");
    }

    @When("the target is sent a broadcast command mid sequence")
    public void the_target_is_sent_a_broadcast_command_mid_sequence() {
        CoreZcodeSteps.testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1A1&*R1H2\n");
        CoreZcodeSteps.testExpectedFields = a -> a.worked().hasField('A', 1).next().hasStatus(ZcodeAcceptanceTestResponseStatus.PARSE_ERROR).isLastInSequence();
    }

    @When("the target is sent debug info mid sequence")
    public void the_target_is_sent_debug_info_mid_sequence() {
        CoreZcodeSteps.testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1A2&#R1H2\n");
        CoreZcodeSteps.testExpectedFields = a -> a.worked().hasField('A', 2).next().hasStatus(ZcodeAcceptanceTestResponseStatus.PARSE_ERROR).isLastInSequence();
    }

    @When("the target is set to parallel mid sequence")
    public void the_target_is_set_to_parallel_mid_sequence() {
        CoreZcodeSteps.testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1A3&%R1H2\n");
        CoreZcodeSteps.testExpectedFields = a -> a.worked().hasField('A', 3).next().hasStatus(ZcodeAcceptanceTestResponseStatus.PARSE_ERROR).isLastInSequence();
    }

    @Then("the target must not respond")
    public void the_target_must_not_respond() {
        assertThrows(TimeoutException.class, () -> CoreZcodeSteps.testAssert.send().get(1, TimeUnit.SECONDS));
    }

    @Then("the target must respond with a broadcast response")
    public void the_target_must_respond_with_a_broadcast_response() throws Throwable {
        try {
            CoreZcodeSteps.testAssert.isBroadcast().send().get(2, TimeUnit.SECONDS);
        } catch (ExecutionException e) {
            throw e.getCause();
        }
    }

    @Then("the target must respond with a(n) {word} status after expected fields")
    public void the_target_must_respond_with_status_after(String status) throws Throwable {
        ZcodeAcceptanceTestResponseStatus stat = ZcodeAcceptanceTestResponseStatus.valueOf(status);
        try {
            CoreZcodeSteps.testExpectedFields.apply(CoreZcodeSteps.testAssert).hasStatus(stat).send().get(2, TimeUnit.SECONDS);
        } catch (ExecutionException e) {
            throw e.getCause();
        }
    }

}