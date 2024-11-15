package net.zscript.acceptance;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;

import static org.assertj.core.api.Assertions.assertThat;

import io.cucumber.java.en.And;
import io.cucumber.java.en.Given;
import io.cucumber.java.en.When;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.commandbuilder.ZscriptResponse;
import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandBuilder;
import net.zscript.javaclient.devices.ResponseSequenceCallback;
import net.zscript.model.modules.base.CoreModule;

public class CommandSteps {
    private static final Logger LOG = LoggerFactory.getLogger(CommandSteps.class);

    ConnectionSteps connectionSteps;

    ZscriptCommandBuilder<?> builder;
    CoreModule               c;

    ResponseSequenceCallback responseResult;

    public CommandSteps(ConnectionSteps connectionSteps) {
        this.connectionSteps = connectionSteps;
    }

    @Given("a {string} command from the {string} module in {string}")
    public void createCommandBuilder(String cmdName, String moduleName, String pkg) throws Exception {
        Class<?> module = Class.forName(pkg + "." + uc(moduleName) + "Module");
        builder = (ZscriptCommandBuilder<?>) module.getMethod(lc(cmdName) + "Builder", new Class<?>[0]).invoke(null);
    }

    @Given("it has field {string} set to {int}")
    public void addFieldToCommand(String fieldName, int value) throws Exception {
        builder.getClass().getMethod("set" + uc(fieldName), new Class<?>[] { int.class }).invoke(builder, value);
    }

    @When("I send the command to the device and receive a response")
    public void sendCommandToDeviceAndResponse() throws InterruptedException, ExecutionException {
        Future<ResponseSequenceCallback> future = connectionSteps.getTestDeviceHandle().send(builder.build());
        connectionSteps.progressLocalDevice();
        responseResult = future.get();
    }

    @And("it should include a {string} field set to {word}")
    public void shouldIncludeFieldSetTo(String fieldName, String value) throws Exception {
        ZscriptResponse r           = responseResult.getResponses().get(0);
        int             actualValue = (Integer) r.getClass().getMethod("get" + uc(fieldName), new Class<?>[] {}).invoke(r);
        assertThat(Integer.decode(value)).isEqualTo(actualValue);
    }

    @And("it should include a bigfield {string} set to {string}")
    public void shouldIncludeBigFieldSetTo(String fieldName, String value) throws Exception {
        ZscriptResponse r           = responseResult.getResponses().get(0);
        String          actualValue = (String) r.getClass().getMethod("get" + uc(fieldName) + "AsString", new Class<?>[] {}).invoke(r);
        assertThat(value).isEqualTo(actualValue);
    }

    // Text-bashing to ensure conformance with naming convention
    private String lc(String s) {
        return s.isBlank() ? s : Character.toLowerCase(s.charAt(0)) + s.substring(1);
    }

    private String uc(String s) {
        return s.isBlank() ? s : Character.toUpperCase(s.charAt(0)) + s.substring(1);
    }
}
