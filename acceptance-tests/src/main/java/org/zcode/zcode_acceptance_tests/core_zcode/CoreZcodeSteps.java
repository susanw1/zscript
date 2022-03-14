package org.zcode.zcode_acceptance_tests.core_zcode;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assumptions.assumeThat;
import static org.zcode.zcode_acceptance_tests.acceptancetest_asserts.ZcodeAcceptanceTestAssert.assertThatCommand;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.function.Function;

import org.junit.Assume;
import org.zcode.zcode_acceptance_tests.ZcodeAcceptanceTestCapabilityResult;
import org.zcode.zcode_acceptance_tests.ZcodeAcceptanceTestConnectionManager;
import org.zcode.zcode_acceptance_tests.acceptancetest_asserts.ZcodeAcceptanceTestMessageAssert;
import org.zcode.zcode_acceptance_tests.acceptancetest_asserts.ZcodeAcceptanceTestResponseStatus;

import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

public class CoreZcodeSteps {
    public static ZcodeAcceptanceTestMessageAssert testAssert = null;
    public static Function<ZcodeAcceptanceTestMessageAssert, ZcodeAcceptanceTestMessageAssert> testExpectedFields = a -> a;

    @Given("a connection is established to the target")
    public void a_connection_is_established_to_the_target() {
        assumeThat(ZcodeAcceptanceTestConnectionManager.getConnections()).as("A connection to the target must be provided").isNotEmpty();
        ZcodeAcceptanceTestConnectionManager.getConnections().get(0).clearListeners();
    }

    @Given("the targets capabilities are known")
    public void the_targets_capabilities_are_known() {
        assertThat(ZcodeAcceptanceTestCapabilityResult.hasBeenRead()).isTrue();
    }

    @Given("the target supports multi-byte fields")
    public void the_target_supports_multi_byte_fields() {
        Assume.assumeFalse("target only supports single byte fields", ZcodeAcceptanceTestCapabilityResult.getMaxFieldSize() == 1);
    }

    @Given("the target has limited field size")
    public void the_target_has_limited_field_size() {
        Assume.assumeFalse("target has no limits on field size/number",
                ZcodeAcceptanceTestCapabilityResult.getMaxFieldNum() == -1 && ZcodeAcceptanceTestCapabilityResult.getMaxFieldSize() == -1);
    }

    @Given("the target has limited big-field size")
    public void the_target_has_limited_big_field_size() {
        Assume.assumeFalse("target has no limits on big-field size", ZcodeAcceptanceTestCapabilityResult.getMaxBigFieldLength() == -1);
    }

    @Given("the target has at least one peripheral command")
    public void the_target_has_at_least_one_peripheral_command() {
        boolean hasFound = false;
        for (byte b : ZcodeAcceptanceTestCapabilityResult.getSupportedCodes()) {
            if (Byte.toUnsignedInt(b) >= 0x10) {
                hasFound = true;
                break;
            }
        }
        Assume.assumeTrue("target does not have any peripheral commands", hasFound);
    }

    @Given("the target has at least one harmless peripheral command")
    public void the_target_has_at_least_one_harmless_peripheral_command() {
        boolean hasFound = false;
        for (byte b : ZcodeAcceptanceTestCapabilityResult.getSupportedCodes()) {
            if (Byte.toUnsignedInt(b) == 0x20 || Byte.toUnsignedInt(b) == 0x40 || Byte.toUnsignedInt(b) == 0x13 || Byte.toUnsignedInt(b) == 0x28) {
                hasFound = true;
                break;
            }
        }
        Assume.assumeTrue("target does not have any harmless peripheral commands", hasFound);
    }

    @When("the target is sent an identify command")
    public void the_target_is_sent_an_identify_command() {
        testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R0\n");
        testExpectedFields = a -> a.hasBigStringField().hasField('V').worked();
    }

    @When("the target is sent an echo command with various fields")
    public void the_target_is_sent_an_echo_command_with_various_fields() {
        testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1A29FL3+0001\n");
        testExpectedFields = a -> a.hasBigDataField("0001").hasField('A', 0x29).hasField('F', 0x00).hasField('L', 0x3).worked();
    }

    @When("the target is sent an echo command with multi-byte fields")
    public void the_target_is_sent_an_echo_command_with_multi_byte_fields() {
        testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1A2900G023\n");
        testExpectedFields = a -> a.hasField('A', "2900").hasField('G', 0x23).worked();
    }

    @When("the target is sent an echo command with an override for status")
    public void the_target_is_sent_an_echo_command_with_status_override() {
        testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1Sff\n");
        testExpectedFields = a -> a.hasField('S', "ff");
    }

    @When("the target is sent a string field containing command separators")
    public void the_target_is_sent_a_string_field_containing_command_separators() {
        testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1\"&\\\"\\a\\\"|\"\n");
        testExpectedFields = a -> a.worked().hasBigStringField("&\"a\"|");
    }

    @When("the target is sent a peripheral command")
    public void the_target_is_sent_a_peripheral_command() {
        for (byte b : ZcodeAcceptanceTestCapabilityResult.getSupportedCodes()) {
            if (Byte.toUnsignedInt(b) >= 0x10) {
                testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R" + Integer.toHexString(Byte.toUnsignedInt(b)) + "\n");
                break;
            }
        }
    }

    @When("the target is sent an activate command")
    public void the_target_is_sent_an_activate_command() throws Throwable {
        try {
            assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R3\n").worked().hasField('A', 0).hasNoOtherFields().send().get(2, TimeUnit.SECONDS);
        } catch (ExecutionException e) {
            throw e.getCause();
        }
    }

    @When("the target is sent a harmless peripheral command")
    public void the_target_is_sent_a_harmless_peripheral_command() {
        for (byte b : ZcodeAcceptanceTestCapabilityResult.getSupportedCodes()) {
            if (Byte.toUnsignedInt(b) == 0x20 || Byte.toUnsignedInt(b) == 0x40 || Byte.toUnsignedInt(b) == 0x13 || Byte.toUnsignedInt(b) == 0x28) {
                testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R" + Integer.toHexString(Byte.toUnsignedInt(b)) + "\n");
                break;
            }
        }
    }

    @When("the target is sent multiple echo commands")
    public void the_target_is_sent_multiple_echo_commands() {
        testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1A10&R1+21BC&R1\"aaa\"\n");
        testExpectedFields = a -> a.worked().hasField('A', "10").sequenceContinues().next().worked().hasField('B', "").hasField('C', "").hasBigDataField("21").sequenceContinues()
                .next().worked().hasBigStringField("aaa").isLastInSequence();
    }

    @When("the target is sent multiple echo commands including error handlers")
    public void the_target_is_sent_multiple_echo_commands_inc_error_handlers() {
        testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1A10&R1+21BC|R1A2&R1+33\n");
        testExpectedFields = a -> a.worked().hasField('A', "10").sequenceContinues().next().worked().hasField('B', "").hasField('C', "").hasBigDataField("21").isLastInSequence();
    }

    @When("the target is sent multiple echo commands one of which gives S10, in a sequence")
    public void the_target_is_sent_multiple_echo_commands_one_of_which_gives_s10_in_a_sequence() {
        testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1A10&R1S10&R1A2&R1+12\n");
        testExpectedFields = a -> a.worked().hasField('A', "10").sequenceContinues().next().hasStatus(ZcodeAcceptanceTestResponseStatus.CMD_FAIL).isLastInSequence();
    }

    @When("the target is sent multiple echo commands one of which gives S10, in a sequence with error handler")
    public void the_target_is_sent_multiple_echo_commands_one_of_which_gives_s10_in_a_sequence_with_error_handler() {
        testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1A10&R1S10+21BC&R1|R1A2&R1+44\n");
        testExpectedFields = a -> a.worked().hasField('A', "10").sequenceContinues().next().hasStatus(ZcodeAcceptanceTestResponseStatus.CMD_FAIL).sequenceHasError().next().worked()
                .hasField('A', "2").sequenceContinues().next().worked().hasBigDataField("44").isLastInSequence();
    }

    @When("the target is sent multiple echo commands one of which gives S5, in a sequence with error handler")
    public void the_target_is_sent_multiple_echo_commands_one_of_which_gives_s5_in_a_sequence_with_error_handler() {
        testAssert = assertThatCommand(ZcodeAcceptanceTestConnectionManager.getConnections().get(0), "R1A10&R1S5+21BC&R1|R1A2&R1+44\n");
        testExpectedFields = a -> a.worked().hasField('A', "10").sequenceContinues().next().hasField('S', 5).isLastInSequence();
    }

    @Then("the target must support all required functionality")
    public void the_target_must_support_all_required_functionality() {
        assertThat(ZcodeAcceptanceTestCapabilityResult.getSupportedCodes()).describedAs("Target must support required commands (Identify, Echo, Capabilities, and Activate)")
                .contains(0x0, 0x1, 0x2, 0x3);
        assertThat(ZcodeAcceptanceTestCapabilityResult.getGeneralCapabilities()).describedAs("Target must give a binary capabilities field (C)").isNotNegative();
        assertThat(ZcodeAcceptanceTestCapabilityResult.getMaxBigFieldLength()).describedAs("Target must support a big field of length >= 4")
                .satisfiesAnyOf(v -> assertThat(v).isNegative(), v -> assertThat(v).isGreaterThanOrEqualTo(4));
        assertThat(ZcodeAcceptanceTestCapabilityResult.getMaxFieldNum()).describedAs("Target must support a field number >= 5").satisfiesAnyOf(v -> assertThat(v).isNegative(),
                v -> assertThat(v).isGreaterThanOrEqualTo(5));
        assertThat(ZcodeAcceptanceTestCapabilityResult.getMaxFieldSize()).describedAs("Target must support a fields of length >= 4").satisfiesAnyOf(v -> assertThat(v).isNegative(),
                v -> assertThat(v).isGreaterThanOrEqualTo(1));
        if (ZcodeAcceptanceTestCapabilityResult.getPersistantMemorySize() != -1) {
            assertThat(ZcodeAcceptanceTestCapabilityResult.getSupportedCodes()).describedAs("If the target has persistant memory, it must support read/write commands")
                    .contains(0x10, 0x11);
        }
        ZcodeAcceptanceTestCapabilityResult.setHasBeenRead();
    }

    @Then("the target must respond with the expected fields")
    public void the_target_must_respond_with_the_expected_fields() throws Throwable {
        try {
            testExpectedFields.apply(testAssert).send().get(2, TimeUnit.SECONDS);
        } catch (ExecutionException e) {
            throw e.getCause();
        }
    }

    @Then("the target must respond with a(n) {word} status")
    public void the_target_must_respond_with_status(String status) throws Throwable {
        ZcodeAcceptanceTestResponseStatus stat = ZcodeAcceptanceTestResponseStatus.valueOf(status);
        try {
            testAssert.hasStatus(stat).send().get(2, TimeUnit.SECONDS);
        } catch (ExecutionException e) {
            throw e.getCause();
        }
    }

}
