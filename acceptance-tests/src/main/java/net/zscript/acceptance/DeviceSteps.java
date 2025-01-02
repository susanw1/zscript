package net.zscript.acceptance;

import java.util.function.Predicate;

import static java.util.Objects.requireNonNull;
import static net.zscript.acceptance.CommandSteps.statusNameToValue;
import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;

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

/**
 * High-level device-level communication, using testDevice and deviceBytesCollector
 */
public class DeviceSteps {
    private static final Logger LOG = LoggerFactory.getLogger(DeviceSteps.class);

    private final ConnectionSteps                connectionSteps;
    private final CollectingConsumer<ByteString> deviceBytesCollector = new CollectingConsumer<>();

    private ZscriptModel model;
    private Device       testDevice;

    public DeviceSteps(ConnectionSteps connectionSteps) {
        this.connectionSteps = connectionSteps;
    }

    public Device getTestDeviceHandle() {
        return requireNonNull(testDevice);
    }

    public ZscriptModel getModel() {
        return requireNonNull(model);
    }

    public <T> void progressDeviceWhile(Predicate<T> p) {
        connectionSteps.progressDeviceWhile(p);
    }

    @Given("a device handle connected to the target")
    public void deviceHandleConnected() {
        if (testDevice != null || model != null) {
            throw new IllegalStateException("Device/model already initialized");
        }
        DirectConnection conn = connectionSteps.getConnection();

        final ZscriptNode node = ZscriptNode.newNode(conn);
        model = ZscriptModel.standardModel();
        testDevice = new Device(model, node);
    }

    @When("I send {string} as a command to the device")
    public void sendCommandToDevice(String command) {
        testDevice.send(byteStringUtf8(command + "\n"), deviceBytesCollector);
    }

    @Then("device should answer with response sequence {string}")
    public void shouldReceiveThisResponseFromDevice(String response) {
        progressDeviceWhile(t -> deviceBytesCollector.isEmpty());
        assertThat(deviceBytesCollector.next().get()).isEqualTo(byteStringUtf8(response));
    }

    @Then("device should answer with response status value {word} and field {word} = {word}")
    public void shouldReceiveThisResponseFromDevice(String status, String field, String value) {
        progressDeviceWhile(t -> deviceBytesCollector.isEmpty());

        final ByteString actual   = deviceBytesCollector.next().orElseThrow();
        final Response   response = ResponseExecutionPath.parse(tokenize(actual).getTokenReader().getFirstReadToken()).getFirstResponse();

        assertThat(response.getFields().getField(Zchars.Z_STATUS)).hasValue(Integer.decode(status));
        assertThat(response.getFields().getField(field.charAt(0))).hasValue(Integer.decode(value));
    }

    @Then("device should answer with status value {word}")
    public void shouldReceiveThisStatusValue(String statusValue) {
        progressDeviceWhile(t -> deviceBytesCollector.isEmpty());

        final ByteString actual   = deviceBytesCollector.next().orElseThrow();
        final Response   response = ResponseExecutionPath.parse(tokenize(actual).getTokenReader().getFirstReadToken()).getFirstResponse();

        assertThat(response.getFields().getField(Zchars.Z_STATUS)).hasValue(Integer.decode(statusValue));
    }

    @Then("device should answer with status {word}")
    public void shouldReceiveThisStatus(String statusName) throws Exception {
        progressDeviceWhile(t -> deviceBytesCollector.isEmpty());

        final ByteString actual   = deviceBytesCollector.next().orElseThrow();
        final Response   response = ResponseExecutionPath.parse(tokenize(actual).getTokenReader().getFirstReadToken()).getFirstResponse();

        assertThat(response.getFields().getField(Zchars.Z_STATUS)).hasValue(statusNameToValue(statusName));
    }
}
