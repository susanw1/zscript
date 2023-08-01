package net.zscript.zscript_acceptance_tests.core_zscript;

import static net.zscript.zscript_acceptance_tests.acceptancetest_asserts.AcceptanceTestAssert.assertThatCommand;
import static org.junit.jupiter.api.Assertions.assertThrows;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;
import net.zscript.zscript_acceptance_tests.AcceptanceTestConnectionManager;
import net.zscript.zscript_acceptance_tests.acceptancetest_asserts.AcceptanceTestResponseStatus;

public class GeneralParseErrorSteps {

    @When("the target is sent an echo command with the same field twice")
    public void the_target_is_sent_an_echo_command_with_the_same_field_twice() {
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), "Z1A10B30A\n");
    }

    @When("the target is sent a command with no Z field")
    public void the_target_is_sent_a_command_with_no_z_field() {
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), "A10B30\n");
    }

    @When("the target is sent a command with an unrecognised Z field")
    public void the_target_is_sent_a_command_with_an_unrecognised_z_field() {
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), "Z110304\n");
    }

    @When("the target is sent a command with two big fields")
    public void the_target_is_sent_a_command_with_two_big_fields() {
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), "Z1+84\"d\"\n");
    }

    @When("the target is sent a command with an unterminated big string")
    public void the_target_is_sent_a_command_with_an_unterminated_big_string() {
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), "Z1\"d\n");
    }

    @When("the target is sent a command with an odd digit big field")
    public void the_target_is_sent_a_command_with_an_odd_digit_big_field() {
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), "Z1+84d\n");
    }

    @When("the target is sent a command with an escaped quote in a string")
    public void the_target_is_sent_a_command_with_an_escaped_quote_in_a_string() {
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), "Z1\"\\\"d\"\n");
    }

    @When("the target is sent a command with a non hex character in the wrong place")
    public void the_target_is_sent_a_command_with_a_non_hex_character_in_the_wrong_place() {
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), "Z1A0g\n");
    }

    @When("the target is sent debug info")
    public void the_target_is_sent_debug_info() {
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), "#Z1A2\n");
    }

    @When("the target is sent a broadcast command")
    public void the_target_is_sent_a_broadcast_command() {
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), "*Z1A5\n");
    }

    @When("the target is sent a broadcast command mid sequence")
    public void the_target_is_sent_a_broadcast_command_mid_sequence() {
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), "Z1A1&*Z1H2\n");
        CoreZscriptSteps.testExpectedFields = a -> a.worked().hasField('A', 1).next().hasStatus(AcceptanceTestResponseStatus.PARSE_ERROR).isLastInSequence();
    }

    @When("the target is sent debug info mid sequence")
    public void the_target_is_sent_debug_info_mid_sequence() {
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), "Z1A2&#Z1H2\n");
        CoreZscriptSteps.testExpectedFields = a -> a.worked().hasField('A', 2).next().hasStatus(AcceptanceTestResponseStatus.PARSE_ERROR).isLastInSequence();
    }

    @When("the target is set to parallel mid sequence")
    public void the_target_is_set_to_parallel_mid_sequence() {
        CoreZscriptSteps.testAssert = assertThatCommand(AcceptanceTestConnectionManager.getConnections().get(0), "Z1A3&%Z1H2\n");
        CoreZscriptSteps.testExpectedFields = a -> a.worked().hasField('A', 3).next().hasStatus(AcceptanceTestResponseStatus.PARSE_ERROR).isLastInSequence();
    }

    @Then("the target must not respond")
    public void the_target_must_not_respond() {
        assertThrows(TimeoutException.class, () -> CoreZscriptSteps.testAssert.send().get(1, TimeUnit.SECONDS));
    }

    @Then("the target must respond with a broadcast response")
    public void the_target_must_respond_with_a_broadcast_response() throws Throwable {
        try {
            CoreZscriptSteps.testAssert.isBroadcast().send().get(2, TimeUnit.SECONDS);
        } catch (ExecutionException e) {
            throw e.getCause();
        }
    }

    @Then("the target must respond with a(n) {word} status after expected fields")
    public void the_target_must_respond_with_status_after(String status) throws Throwable {
        AcceptanceTestResponseStatus stat = AcceptanceTestResponseStatus.valueOf(status);
        try {
            CoreZscriptSteps.testExpectedFields.apply(CoreZscriptSteps.testAssert).hasStatus(stat).send().get(2, TimeUnit.SECONDS);
        } catch (ExecutionException e) {
            throw e.getCause();
        }
    }

}
