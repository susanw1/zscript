package net.zscript.acceptance;

import java.io.IOException;

import static java.time.Duration.ofSeconds;
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

    private final LocalDeviceSteps localDeviceSteps;

    private ZscriptModel     model;
    private Device           testDevice;
    private DirectConnection conn;

    private final CollectingConsumer<ByteString> deviceBytesCollector     = new CollectingConsumer<>();
    private final CollectingConsumer<byte[]>     connectionBytesCollector = new CollectingConsumer<>();

    public ConnectionSteps(LocalDeviceSteps localDeviceSteps) {
        this.localDeviceSteps = localDeviceSteps;
    }

    public Device getTestDeviceHandle() {
        return testDevice;
    }

    public void progressLocalDevice() {
        // allow any "not progressing" to burn away, before trying to actually progress
        await().atMost(ofSeconds(10)).until(localDeviceSteps::progressZscriptDevice);
        while (localDeviceSteps.progressZscriptDevice()) {
            LOG.trace("Progressed Zscript device...");
        }
    }

    private DirectConnection createConnection() {
        if (Boolean.getBoolean("zscript.acceptance.conn.tcp")) {
            throw new PendingException("Support for network interfaces is not implemented yet");
        }
        if (Boolean.getBoolean("zscript.acceptance.conn.serial")) {
            throw new PendingException("Support for network interfaces is not implemented yet");
        }
        return localDeviceSteps.getLocalConnection();
    }

    @After
    public void closeConnection() throws IOException {
        if (conn != null) {
            LOG.trace("Closing direct connection...");
            conn.close();
            LOG.trace("Direct connection closed");
        }
    }

    @Given("a connected device handle")
    public void deviceHandleConnected() {
        if (testDevice != null || model != null || conn != null) {
            throw new IllegalStateException("Device/model/connection already initialized");
        }
        conn = createConnection();

        final ZscriptNode node = ZscriptNode.newNode(conn);
        model = ZscriptModel.standardModel();
        testDevice = new Device(model, node);
    }

    @When("I send {string} as a command to the connection")
    public void sendCommandToConnection(String command) throws IOException {
        if (conn != null) {
            throw new IllegalStateException("Connection already initialized");
        }

        conn = createConnection();
        conn.onReceiveBytes(connectionBytesCollector);
        conn.sendBytes(byteStringUtf8(command + "\n").toByteArray());
    }

    @Then("connection should receive exactly {string} in response")
    public void shouldReceiveConnectionResponse(String response) {
        await().atMost(ofSeconds(10)).until(() -> !localDeviceSteps.progressZscriptDevice() && !connectionBytesCollector.isEmpty());
        assertThat(connectionBytesCollector.next().get()).contains(byteStringUtf8(response + "\n").toByteArray());
    }

    @When("I send {string} as a command to the device")
    public void sendCommandToDevice(String command) {
        testDevice.send(byteStringUtf8(command + "\n"), deviceBytesCollector);
    }

    private void progressDeviceUntilResponseReceived() {
        await().atMost(ofSeconds(10)).until(() -> !localDeviceSteps.progressZscriptDevice() && !deviceBytesCollector.isEmpty());
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

    //    @When("I send {string} to the device using the high level interface")
    //    public void sendCommandToDeviceUsingHLI(String command) {
    //        ExtendingTokenBuffer buf = new ExtendingTokenBuffer();
    //        Tokenizer            tok = new Tokenizer(buf.getTokenWriter());
    //        byteStringUtf8(command).forEach(tok::accept);
    //        CommandSequence seq = CommandSequence.parse(model, buf.getTokenReader().getFirstReadToken(), false);
    //        testDevice.send(seq);
    //    }

    //    @Then("device should supply response sequence {string} in response")
    //    public void shouldReceiveThisResponse(String response) {
    //        await().atMost(ofSeconds(10)).until(() -> !zscript.progress() && !deviceBytesCollector.isEmpty());
    //        assertThat(deviceBytesCollector.next().get()).isEqualTo(byteStringUtf8(response));
    //    }

}
