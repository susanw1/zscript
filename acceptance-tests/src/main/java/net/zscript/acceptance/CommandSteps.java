package net.zscript.acceptance;

import java.lang.reflect.InvocationTargetException;
import java.util.List;
import java.util.OptionalInt;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeoutException;
import java.util.function.BiFunction;

import static java.util.Arrays.stream;
import static java.util.concurrent.TimeUnit.SECONDS;
import static org.assertj.core.api.Assertions.assertThat;

import io.cucumber.java.en.And;
import io.cucumber.java.en.Given;
import io.cucumber.java.en.When;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.commandbuilder.ZscriptResponse;
import net.zscript.javaclient.commandbuilder.commandnodes.CommandSequenceNode;
import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandBuilder;
import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandNode;
import net.zscript.javaclient.devices.ResponseSequenceCallback;

/**
 * Steps that relate to building commands and decoding responses.
 */
public class CommandSteps {
    private static final Logger LOG = LoggerFactory.getLogger(CommandSteps.class);

    private ConnectionSteps connectionSteps;

    private Class<?>                 currentModule;
    private ZscriptCommandBuilder<?> nodeBuilder;
    private CommandSequenceNode      prevCommandNode;

    private BiFunction<CommandSequenceNode, CommandSequenceNode, CommandSequenceNode> prevCombiner;

    private List<ZscriptResponse> responses;
    private int                   currentResponseIndex;

    public CommandSteps(ConnectionSteps connectionSteps) {
        this.connectionSteps = connectionSteps;
    }

    @Given("the {word} command from the {word} module in {word}")
    public void createCommandBuilder(String cmdName, String moduleName, String pkg) throws Exception {
        if (currentModule != null) {
            throw new IllegalStateException("A command is already in progress");
        }
        currentModule = getModule(moduleName, pkg);
        nodeBuilder = getBuilder(currentModule, cmdName);
    }

    @Given("it has the {word} field set to {word}")
    public void addIntFieldToCommand(String fieldName, String value) throws Exception {
        nodeBuilder.getClass().getMethod("set" + uc(fieldName), int.class).invoke(nodeBuilder, Integer.decode(value));
    }

    @And("it has the field key {string} set to {word}")
    public void addNumberFieldToCommandByKey(String key, String value) {
        nodeBuilder.setField(asKey(key), Integer.decode(value));
    }

    private static char asKey(String key) {
        if (key.length() != 1 || !Character.isUpperCase(key.charAt(0))) {
            throw new IllegalArgumentException("field key must be a single uppercase character, not: " + key);
        }
        return key.charAt(0);
    }

    @Given("it has the {word} field set to enum value {word}")
    public void addEnumFieldToCommand(String fieldName, String value) throws Exception {
        // The fieldName is the name of the enum class inside the command builder
        final Class<?> enumClass = stream(nodeBuilder.getClass().getDeclaredClasses())
                .filter(cls -> cls.getSimpleName().equals(fieldName))
                .findFirst().orElseThrow();
        final Object enumValue = Enum.valueOf((Class<Enum>) enumClass, value);
        nodeBuilder.getClass().getMethod("set" + uc(fieldName), enumClass).invoke(nodeBuilder, enumValue);
    }

    @And("if successful, it is followed by the {word} command")
    public void andedWithCommand(String cmdName) throws Exception {
        prevCommandNode = combineCommands();
        nodeBuilder = getBuilder(currentModule, cmdName);
        prevCombiner = CommandSequenceNode::andThen;
    }

    @And("if failed, it is followed by the {word} command")
    public void oredWithCommand(String cmdName) throws Exception {
        prevCommandNode = combineCommands();
        nodeBuilder = getBuilder(currentModule, cmdName);
        prevCombiner = CommandSequenceNode::onFail;

    }

    private CommandSequenceNode combineCommands() {
        ZscriptCommandNode<?> commandNode = nodeBuilder.build();
        return prevCommandNode == null ? commandNode : prevCombiner.apply(prevCommandNode, commandNode);
    }

    @When("I send the command sequence to the device and receive a response sequence")
    public void sendCommandToDeviceAndResponse() throws InterruptedException, ExecutionException, TimeoutException {
        final Future<ResponseSequenceCallback> future = connectionSteps.getTestDeviceHandle().send(combineCommands());
        connectionSteps.progressLocalDevice();
        final ResponseSequenceCallback responseResult = future.get(10, SECONDS);
        currentResponseIndex = 0;
        responses = responseResult.getResponses();
        responses.forEach(r -> LOG.debug("Response: {}", r));
    }

    @And("it should include a {word} field set to {word}")
    public void shouldIncludeFieldSetTo(String fieldName, String value) throws Exception {
        final ZscriptResponse currentResponse = responses.get(currentResponseIndex);
        final int             actualValue     = (Integer) currentResponse.getClass().getMethod("get" + uc(fieldName)).invoke(currentResponse);
        assertThat(Integer.decode(value)).isEqualTo(actualValue);
    }

    @And("it should include a bigfield {word} set to {string}")
    public void shouldIncludeBigFieldSetTo(String fieldName, String value) throws Exception {
        final ZscriptResponse currentResponse = responses.get(currentResponseIndex);
        final String          actualValue     = (String) currentResponse.getClass().getMethod("get" + uc(fieldName) + "AsString").invoke(currentResponse);
        assertThat(value).isEqualTo(actualValue);
    }

    @And("it should include a field key {string} set to {word}")
    public void shouldIncludeKeyedFieldSetTo(String key, String value) {
        final ZscriptResponse currentResponse = responses.get(currentResponseIndex);
        final OptionalInt     actualValue     = currentResponse.getField((byte) asKey(key));
        assertThat(Integer.decode(value)).isEqualTo(actualValue.orElseThrow());
    }

    /**
     * This method moves us to the next response in the sequence.
     */
    @And("as successful, there should be a following response")
    public void shouldContainFollowingSuccessResponse() {
        final ZscriptResponse currentResponse = responses.get(currentResponseIndex);

        currentResponseIndex++;
        assertThat(responses.size()).isGreaterThan(currentResponseIndex);
    }

    /**
     * This method moves us to the next response in the sequence.
     */
    @And("as failed, there should be a following response")
    public void shouldContainFollowingFailResponse() {
        currentResponseIndex++;
        assertThat(responses.size()).isGreaterThan(currentResponseIndex);
    }

    /**
     * Verify that there are no more responses in the sequence.
     */
    @And("there should be no additional responses")
    public void shouldContainNoMoreResponses() {
        currentResponseIndex++;
        assertThat(responses.size()).isEqualTo(currentResponseIndex);
    }

    private ZscriptCommandBuilder<?> getBuilder(Class<?> currentModule, String cmdName) throws IllegalAccessException, InvocationTargetException, NoSuchMethodException {
        return (ZscriptCommandBuilder<?>) currentModule.getMethod(lc(cmdName) + "Builder").invoke(null);
    }

    private Class<?> getModule(String moduleName, String pkg) throws ClassNotFoundException {
        return Class.forName(pkg + "." + uc(moduleName) + "Module");
    }

    // Text-bashing to ensure conformance with naming convention - change first char to lower case
    private String lc(String s) {
        return s.isBlank() ? s : Character.toLowerCase(s.charAt(0)) + s.substring(1);
    }

    // Text-bashing to ensure conformance with naming convention - change first char to upper case
    private String uc(String s) {
        return s.isBlank() ? s : Character.toUpperCase(s.charAt(0)) + s.substring(1);
    }
}
