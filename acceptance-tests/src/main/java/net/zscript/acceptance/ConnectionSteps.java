package net.zscript.acceptance;

import java.io.IOException;

import static java.time.Duration.ofSeconds;
import static java.util.Objects.requireNonNull;
import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
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
import net.zscript.javaclient.commandPaths.ResponseExecutionPath;
import net.zscript.javaclient.devices.Device;
import net.zscript.javaclient.nodes.DirectConnection;
import net.zscript.javaclient.nodes.ZscriptNode;
import net.zscript.javareceiver.testing.CollectingConsumer;
import net.zscript.model.ZscriptModel;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;

public class ConnectionSteps {
    private static final Logger LOG = LoggerFactory.getLogger(ConnectionSteps.class);

    private final LocalJavaReceiverSteps localJavaReceiverSteps;

    private ZscriptModel     model;
    private Device           testDevice;
    private DirectConnection conn;

    private final CollectingConsumer<ByteString> deviceBytesCollector     = new CollectingConsumer<>();
    private final CollectingConsumer<byte[]>     connectionBytesCollector = new CollectingConsumer<>();

    public ConnectionSteps(LocalJavaReceiverSteps localJavaReceiverSteps) {
        this.localJavaReceiverSteps = localJavaReceiverSteps;
    }

    public Device getTestDeviceHandle() {
        return requireNonNull(testDevice);
    }

    public ZscriptModel getModel() {
        return requireNonNull(model);
    }

    public void progressLocalDeviceIfRequired() {
        if (localJavaReceiverSteps != null) {
            // allow any "not progressing" to burn away, before trying to actually progress
            await().atMost(ofSeconds(10)).until(localJavaReceiverSteps::progressZscriptDevice);
            while (localJavaReceiverSteps.progressZscriptDevice()) {
                LOG.trace("Progressed Zscript device...");
            }
        }
    }

    private DirectConnection createConnection() {
        // If local device setup has been done, assume it overrides system-property settings
        if (localJavaReceiverSteps.isConnected()) {
            return localJavaReceiverSteps.getLocalConnection();
        }

        // Otherwise use those system settings
        if (Boolean.getBoolean("zscript.acceptance.conn.tcp")) {
            throw new PendingException("Support for tests over network interfaces is not implemented yet");
        } else if (Boolean.getBoolean("zscript.acceptance.conn.serial")) {
            throw new PendingException("Support for tests over serial interfaces is not implemented yet");
        } else {
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

    @Given("a connection to the receiver")
    public void connectionToReceiver() {
        if (conn != null) {
            throw new IllegalStateException("Device/model/connection already initialized");
        }
        conn = createConnection();
    }

    @Given("a connected device handle")
    public void deviceHandleConnected() {
        if (testDevice != null || model != null) {
            throw new IllegalStateException("Device/model already initialized");
        }
        if (conn != null) {
            throw new IllegalStateException("connection already initialized");
        }
        conn = createConnection();
        final ZscriptNode node = ZscriptNode.newNode(conn);
        model = ZscriptModel.standardModel();
        testDevice = new Device(model, node);
    }

    @When("I send {string} as a command to the connection")
    public void sendCommandToConnection(String command) throws IOException {
        if (conn == null) {
            throw new IllegalStateException("Connection not initialized");
        }
        conn.onReceiveBytes(connectionBytesCollector);
        conn.sendBytes(byteStringUtf8(command + "\n").toByteArray());
    }

    @Then("connection should receive exactly {string} in response")
    public void shouldReceiveConnectionResponse(String response) {
        await().atMost(ofSeconds(10)).until(() -> !localJavaReceiverSteps.progressZscriptDevice() && !connectionBytesCollector.isEmpty());
        assertThat(connectionBytesCollector.next().get()).contains(byteStringUtf8(response + "\n").toByteArray());
    }

    @When("I send {string} as a command to the device")
    public void sendCommandToDevice(String command) {
        testDevice.send(byteStringUtf8(command + "\n"), deviceBytesCollector);
    }

    private void progressDeviceUntilResponseReceived() {
        await().atMost(ofSeconds(10)).until(() -> !localJavaReceiverSteps.progressZscriptDevice() && !deviceBytesCollector.isEmpty());
    }

    @Then("device should answer with response sequence {string}")
    public void shouldReceiveThisResponse(String response) {
        progressDeviceUntilResponseReceived();
        assertThat(deviceBytesCollector.next().get()).isEqualTo(byteStringUtf8(response));
    }

    @Then("device should answer with response status {int} and field {word} = {word}")
    public void shouldReceiveThisResponse(int status, String field, String value) {
        progressDeviceUntilResponseReceived();

        final ByteString actual   = deviceBytesCollector.next().orElseThrow();
        final Response   response = ResponseExecutionPath.parse(tokenize(actual).getTokenReader().getFirstReadToken()).getFirstResponse();

        assertThat(response.getFields().getField(Zchars.Z_STATUS)).hasValue(status);
        assertThat(response.getFields().getField(field.charAt(0))).hasValue(Integer.decode(value));
    }
}
