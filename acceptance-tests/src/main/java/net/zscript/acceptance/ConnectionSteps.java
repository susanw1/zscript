package net.zscript.acceptance;

import java.io.IOException;
import java.util.List;
import java.util.function.Predicate;

import static java.time.Duration.ofSeconds;
import static net.zscript.acceptance.CommandSteps.statusNameToValue;
import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.util.ByteString.byteString;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.awaitility.Awaitility.await;

import io.cucumber.java.After;
import io.cucumber.java.PendingException;
import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.commandPaths.Response;
import net.zscript.javaclient.commandPaths.ZscriptFieldSet;
import net.zscript.javaclient.nodes.DirectConnection;
import net.zscript.javaclient.sequence.ResponseSequence;
import net.zscript.javareceiver.testing.CollectingConsumer;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;

/**
 * Steps involving low-level connection-based communication.
 */
public class ConnectionSteps {
    private static final Logger LOG = LoggerFactory.getLogger(ConnectionSteps.class);

    private final LocalJavaReceiverSteps localJavaReceiverSteps;

    private DirectConnection conn;

    private final CollectingConsumer<byte[]> connectionBytesCollector = new CollectingConsumer<>();
    private       ByteString                 responseBytes;

    public ConnectionSteps(LocalJavaReceiverSteps localJavaReceiverSteps) {
        this.localJavaReceiverSteps = localJavaReceiverSteps;
    }

    /**
     * Repeatedly progresses any local Zscript device and checks the supplied predicate, until any local device stops progressing and the predicate fails. The predicate argument is
     * always null.
     *
     * @param p some predicate to test (use t->false if only testing for device progression)
     */
    public void progressDeviceWhile(Predicate<?> p) {
        await().atMost(ofSeconds(10))
                .until(() -> (!localJavaReceiverSteps.isConnected() || !localJavaReceiverSteps.progressZscriptDevice()) && !p.test(null));
    }

    private DirectConnection createConnection() {
        // If local device setup has been done (ie explicitly), assume it overrides system-property settings
        if (localJavaReceiverSteps.isConnected()) {
            return localJavaReceiverSteps.getLocalConnection();
        }

        // Otherwise use those system settings
        if (Boolean.getBoolean("zscript.acceptance.conn.tcp")) {
            throw new PendingException("Support for tests over network interfaces is not implemented yet");
        } else if (Boolean.getBoolean("zscript.acceptance.conn.serial")) {
            throw new PendingException("Support for tests over serial interfaces is not implemented yet");
        } else {
            // And then use the local java device as a default
            if (!Boolean.getBoolean("zscript.acceptance.conn.local.java")) {
                LOG.warn("No acceptance test connection defined - defaulting to zscript.acceptance.conn.local.java");
            }
            localJavaReceiverSteps.runAndConnectToZscriptReceiver();
            return localJavaReceiverSteps.getLocalConnection();
        }
    }

    @After
    public void closeConnection() throws IOException {
        if (conn != null) {
            LOG.trace("Closing direct connection...");
            conn.close();
            LOG.trace("Direct connection closed");
        }
    }

    public DirectConnection getConnection() {
        if (conn == null) {
            conn = createConnection();
        }
        return conn;
    }

    @Given("a connection to the target")
    public void connectionToTarget() {
        if (conn != null) {
            throw new IllegalStateException("Device/model/connection already initialized");
        }
        conn = createConnection();
    }

    @When("I send exactly {string} as a command sequence to the connection, and capture the response")
    public void sendCommandToConnection(String command) throws IOException {
        if (conn == null) {
            throw new IllegalStateException("Connection not initialized");
        }
        conn.onReceiveBytes(connectionBytesCollector);
        conn.sendBytes(byteStringUtf8(command + "\n").toByteArray());

        progressDeviceWhile(t -> connectionBytesCollector.isEmpty());
        responseBytes = byteString(connectionBytesCollector.next().orElseThrow());
    }

    @Then("connection response is exactly {string}")
    public void shouldReceiveExactConnectionResponse(String expectedResponse) {
        assertThat(responseBytes).isEqualTo(byteStringUtf8(expectedResponse + "\n"));
    }

    @Then("connection response #{int} has status value {word}")
    public void shouldReceiveThisResponseStatusByValueFromConnection(int index, String statusValue) {
        final List<Response> responses = ResponseSequence.parse(tokenize(responseBytes).getTokenReader().getFirstReadToken())
                .getExecutionPath().getResponses();

        assertThat(responses.get(index).getFields().getField(Zchars.Z_STATUS))
                .as("actual response: %s", responseBytes.asString())
                .hasValue(Integer.decode(statusValue));
    }

    @Then("connection response #{int} has status {word}")
    public void shouldReceiveThisResponseStatusByNameFromConnection(int index, String statusName) throws Exception {
        shouldReceiveThisResponseStatusByValueFromConnection(index, String.valueOf(statusNameToValue(statusName)));
    }

    @Then("connection response #{int} is equivalent to {string}")
    public void shouldReceiveMatchingResponseFromConnection(int index, String expectedResponse) {
        final List<Response> actualResponses = ResponseSequence.parse(tokenize(responseBytes).getTokenReader().getFirstReadToken())
                .getExecutionPath().getResponses();

        final ZscriptFieldSet expectedFields = ZscriptFieldSet.fromTokens(tokenize(byteStringUtf8(expectedResponse)).getTokenReader().getFirstReadToken());
        assertThat(actualResponses.get(index).getFields().matchDescription(expectedFields))
                .as("actual response: %s", responseBytes.asString())
                .isEmpty();
    }
}
