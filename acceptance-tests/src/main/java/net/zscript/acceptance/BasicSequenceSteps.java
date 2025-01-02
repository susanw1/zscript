package net.zscript.acceptance;

import java.util.concurrent.atomic.AtomicReference;

import static net.zscript.util.ByteString.byteString;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;

import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.commandPaths.ResponseExecutionPath;
import net.zscript.javaclient.sequence.ResponseSequence;
import net.zscript.javaclient.tokens.ExtendingTokenBuffer;

public class BasicSequenceSteps {
    private static final Logger LOG = LoggerFactory.getLogger(BasicSequenceSteps.class);

    private final DeviceSteps           deviceSteps;
    private       ResponseExecutionPath actualExecutionPath;

    public BasicSequenceSteps(DeviceSteps deviceSteps) {
        this.deviceSteps = deviceSteps;
    }

    @When("the command sequence {string} is sent to the target")
    public void sendCommandSequence(String commandSequenceAsText) {
        final AtomicReference<ResponseExecutionPath> response = new AtomicReference<>();

        final ExtendingTokenBuffer buffer      = ExtendingTokenBuffer.tokenize(byteStringUtf8(commandSequenceAsText), true);
        final CommandExecutionPath commandPath = CommandExecutionPath.parse(deviceSteps.getModel(), buffer.getTokenReader().getFirstReadToken());

        deviceSteps.getTestDeviceHandle().send(commandPath, response::set);

        deviceSteps.progressDeviceWhile(t -> response.get() == null);

        actualExecutionPath = response.get();
    }

    @Then("the response sequence should match {string}")
    public void responseSequenceShouldMatch(String responseSequenceAsText) {
        final ExtendingTokenBuffer buffer              = ExtendingTokenBuffer.tokenize(byteStringUtf8(responseSequenceAsText));
        final ResponseSequence     expectedResponseSeq = ResponseSequence.parse(buffer.getTokenReader().getFirstReadToken());

        LOG.trace("sequence match: [actualExecutionPath={}, expectedResponseSeq = {}", actualExecutionPath, expectedResponseSeq);
        assertThat(byteString(actualExecutionPath)).isEqualTo(byteString(expectedResponseSeq.getExecutionPath()));
    }
}
