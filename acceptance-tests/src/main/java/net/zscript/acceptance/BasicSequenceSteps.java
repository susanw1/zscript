package net.zscript.acceptance;

import java.util.concurrent.atomic.AtomicReference;

import static java.time.Duration.ofSeconds;
import static net.zscript.util.ByteString.byteString;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.awaitility.Awaitility.await;

import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.commandPaths.ResponseExecutionPath;
import net.zscript.javaclient.sequence.ResponseSequence;
import net.zscript.javaclient.tokens.ExtendingTokenBuffer;

public class BasicSequenceSteps {
    private static final Logger          LOG = LoggerFactory.getLogger(BasicSequenceSteps.class);
    private final        ConnectionSteps connectionSteps;

    private ResponseExecutionPath actualExecutionPath;

    public BasicSequenceSteps(ConnectionSteps connectionSteps) {
        this.connectionSteps = connectionSteps;
    }

    @When("the command sequence {string} is sent to the device")
    public void sendCommandSequence(String commandSequenceAsText) {
        final AtomicReference<ResponseExecutionPath> response = new AtomicReference<>();

        final ExtendingTokenBuffer buffer      = ExtendingTokenBuffer.tokenize(byteStringUtf8(commandSequenceAsText), true);
        final CommandExecutionPath commandPath = CommandExecutionPath.parse(connectionSteps.getModel(), buffer.getTokenReader().getFirstReadToken());

        connectionSteps.getTestDeviceHandle().send(commandPath, response::set);
        connectionSteps.progressLocalDeviceIfRequired();
        await().atMost(ofSeconds(10)).until(() -> response.get() != null);

        actualExecutionPath = response.get();
    }

    @Then("the response should match {string}")
    public void responseShouldMatch(String responseSequenceAsText) {
        final ExtendingTokenBuffer buffer              = ExtendingTokenBuffer.tokenize(byteStringUtf8(responseSequenceAsText));
        final ResponseSequence     expectedResponseSeq = ResponseSequence.parse(buffer.getTokenReader().getFirstReadToken());

        LOG.trace("sequence match: [actualExecutionPath={}, expectedResponseSeq = {}", actualExecutionPath, expectedResponseSeq);
        assertThat(byteString(actualExecutionPath)).isEqualTo(byteString(expectedResponseSeq.getExecutionPath()));
    }
}
