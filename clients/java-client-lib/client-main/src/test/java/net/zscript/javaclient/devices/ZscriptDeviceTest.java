package net.zscript.javaclient.devices;

import java.io.IOException;
import java.io.OutputStream;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.time.Duration;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicReference;

import static java.lang.String.format;
import static java.time.Duration.ofSeconds;
import static java.util.concurrent.TimeUnit.MILLISECONDS;
import static java.util.concurrent.TimeUnit.SECONDS;
import static java.util.stream.Collectors.toList;
import static net.zscript.model.modules.base.CoreModule.CapabilitiesCommand.Builder.VersionType.UserFirmware;
import static net.zscript.model.modules.base.CoreModule.activateBuilder;
import static net.zscript.model.modules.base.CoreModule.capabilitiesBuilder;
import static net.zscript.model.modules.base.CoreModule.echoBuilder;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatIllegalArgumentException;
import static org.assertj.core.api.Assertions.assertThatThrownBy;
import static org.awaitility.Awaitility.await;
import static org.mockito.Mockito.spy;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandbuilderapi.ZscriptResponse;
import net.zscript.javaclient.devicenodes.Connection;
import net.zscript.javaclient.devicenodes.ZscriptCallbackThreadpool;
import net.zscript.javaclient.devicenodes.ZscriptNode;
import net.zscript.javaclient.devices.ResponseSequenceCallback.CommandExecutionSummary;
import net.zscript.javaclient.local.LocalConnection;
import net.zscript.model.ZscriptModel;
import net.zscript.model.modules.base.CoreModule.ActivateCommand;
import net.zscript.model.modules.base.CoreModule.CapabilitiesCommand;
import net.zscript.model.modules.base.CoreModule.CapabilitiesCommand.CapabilitiesResponse;
import net.zscript.model.modules.base.CoreModule.EchoCommand;
import net.zscript.util.ByteString;

class ZscriptDeviceTest {
    private static final Duration AWAIT_TIMEOUT = ofSeconds(2);

    private ZscriptCallbackThreadpool pool;

    // use commandByteCapture.asStringUtf8() to intercept commands on their way to the target
    private final ByteString.ByteStringBuilder commandByteCapture = ByteString.builder();
    // write responses here to simulate device response
    private       OutputStream                 responseSender;

    private ZscriptDevice device;

    @BeforeEach
    public void setup() throws IOException {
        pool = spy(new ZscriptCallbackThreadpool());
        final OutputStream     commandOut = commandByteCapture.asOutputStream();
        final PipedInputStream responseIn = new PipedInputStream();
        responseSender = new PipedOutputStream(responseIn);

        Connection   connection = spy(new LocalConnection(commandOut, responseIn));
        ZscriptNode  node       = ZscriptNode.newNode(connection, 128, 3, SECONDS);
        ZscriptModel model      = ZscriptModel.rawModel();
        device = new ZscriptDevice(model, node);
    }

    @AfterEach
    public void teardown() throws IOException {
        responseSender.close();
    }

    @Test
    void shouldSendAsync() throws IOException {
        final AtomicReference<ResponseSequenceCallback> responseHolder = new AtomicReference<>();

        final CapabilitiesCommand builtCommand = capabilitiesBuilder().setVersionType(UserFirmware).build();
        device.sendAsync(builtCommand, responseHolder::set);
        assertThat(commandByteCapture.asStringUtf8()).isEqualTo("%=100ZV\n");

        byteStringUtf8("! =100 S C7 M1 V2 \"testing\"  \n").writeTo(responseSender);
        await().atMost(AWAIT_TIMEOUT).until(() -> responseHolder.get() != null);

        final ResponseSequenceCallback result = responseHolder.get();
        assertThat(result.getResponses()).hasSize(1);
        final ZscriptResponse resp0 = result.getResponses().get(0);
        assertThat(resp0.expression().fields()
                .map(f -> format("%c%x", f.getKey(), f.getValue())).collect(toList()))
                .containsExactlyInAnyOrder("C7", "M1", "S0", "V2");
        assertThat(result.getSucceeded()).hasSize(1);
        final CommandExecutionSummary<?> summary = result.getSucceeded().iterator().next();

        assertThat(summary.getCommand()).isSameAs(builtCommand);
        assertThat(summary.getResponseType()).isEqualTo(CapabilitiesResponse.class);
        assertThat(summary.getResponse()).isInstanceOf(CapabilitiesResponse.class);

        assertThat(result.getFailed()).isEmpty();
        assertThat(result.getAborted()).isEmpty();
        assertThat(result.getExecuted()).hasSize(1);
        assertThat(result.wasExecuted()).isTrue();
    }

    @Test
    void shouldSendWithFutureSucceeding() throws ExecutionException, InterruptedException, IOException {
        final CapabilitiesCommand capCmd1 = capabilitiesBuilder().setVersionType(UserFirmware).build();
        final CapabilitiesCommand capCmd2 = capabilitiesBuilder().setVersionType(4).build();

        final Future<ResponseSequenceCallback> fut = device.send(capCmd1.andThen(capCmd2));

        final ResponseSequenceCallback result
                = waitAndCheckResult(fut, "%=100ZV&ZV4\n", "! =100 S C7 M1 V2 \"testing\" & S1 \n", 2, 1, 1, false, 2, 0, true);

        final ZscriptResponse resp0 = result.getResponses().get(0);
        final ZscriptResponse resp1 = result.getResponses().get(1);

        assertThat(resp0.expression().fields()
                .map(f -> format("%c%x", f.getKey(), f.getValue())).collect(toList()))
                .containsExactlyInAnyOrder("C7", "M1", "S0", "V2");

        assertThat(resp1.expression().fields()
                .map(f -> format("%c%x", f.getKey(), f.getValue())).collect(toList()))
                .containsExactlyInAnyOrder("S1");

        final CommandExecutionSummary<?> summary = result.getSucceeded().iterator().next();
        assertThat(summary.getCommand()).isSameAs(capCmd1);
        assertThat(summary.getResponseType()).isEqualTo(CapabilitiesResponse.class);
        assertThat(summary.getResponse()).isInstanceOf(CapabilitiesResponse.class);
    }

    @Test
    void shouldSendWithFutureFailing() throws ExecutionException, InterruptedException, IOException {
        final CapabilitiesCommand              cmd1 = capabilitiesBuilder().setVersionType(UserFirmware).build();
        final ActivateCommand                  cmd2 = activateBuilder().setChallenge(4).build();
        final Future<ResponseSequenceCallback> fut  = device.send(cmd1.andThen(cmd2));
        final ResponseSequenceCallback result
                = waitAndCheckResult(fut, "%=100ZV&Z2K4\n", "! =100 S1\n", 1, 0, 1, false, 1, 1, true);

        assertThat(result.getFailed()).hasSize(1);
        assertThat(result.getFailed().iterator().next().getCommand()).isEqualTo(cmd1);
        assertThat(result.getNotExecuted()).hasSize(1).containsExactly(cmd2);
        assertThat(result.getExecuted().get(0)).isEqualTo(cmd1);
    }

    @Test
    void shouldSendWithFutureAborting() throws ExecutionException, InterruptedException, IOException {
        final CapabilitiesCommand              cmd1 = capabilitiesBuilder().setVersionType(UserFirmware).build();
        final ActivateCommand                  cmd2 = activateBuilder().setChallenge(4).build();
        final Future<ResponseSequenceCallback> fut  = device.send(cmd1.andThen(cmd2));

        final ResponseSequenceCallback result
                = waitAndCheckResult(fut, "%=100ZV&Z2K4\n", "! =100 S10\n", 1, 0, 0, true, 1, 1, true);

        assertThat(result.getAborted().orElseThrow().getCommand()).isNotNull().isEqualTo(cmd1);
        assertThat(result.getNotExecuted()).hasSize(1).containsExactly(cmd2);
        assertThat(result.getExecuted().get(0)).isEqualTo(cmd1);
    }

    @Test
    void shouldSendExpectSuccessWithSuccessfulExecution() throws IOException, ExecutionException, InterruptedException {
        final CapabilitiesCommand              cmd1 = capabilitiesBuilder().setVersionType(UserFirmware).build();
        final ActivateCommand                  cmd2 = activateBuilder().setChallenge(4).build();
        final Future<ResponseSequenceCallback> fut  = device.sendExpectSuccess(cmd1.andThen(cmd2));
        final ResponseSequenceCallback result
                = waitAndCheckResult(fut, "%=100ZV&Z2K4\n", "! =100 S&S\n", 2, 2, 0, false, 2, 0, true);

        assertThat(result.getExecuted()).containsExactly(cmd1, cmd2);
    }

    @Test
    void shouldSendExpectSuccessHandlingFailureExecution() throws IOException, ExecutionException, InterruptedException {
        final EchoCommand                      cmd1 = echoBuilder().setAny('S', 1).build();
        final ActivateCommand                  cmd2 = activateBuilder().setChallenge(4).build();
        final Future<ResponseSequenceCallback> fut  = device.sendExpectSuccess(cmd1.onFail(cmd2));
        final ResponseSequenceCallback result
                = waitAndCheckResult(fut, "%=100Z1S1|Z2K4\n", "! =100 S1|S\n", 2, 1, 1, false, 2, 0, true);

        assertThat(result.getExecuted()).containsExactly(cmd1, cmd2);
    }

    @Test
    void shouldSendExpectSuccessEndingWithFailureExecution() throws IOException {
        final EchoCommand                      cmd1 = echoBuilder().setAny('S', 1).build();
        final ActivateCommand                  cmd2 = activateBuilder().setChallenge(4).build();
        final Future<ResponseSequenceCallback> fut  = device.sendExpectSuccess(cmd1.onFail(cmd2));

        assertThat(commandByteCapture.asStringUtf8()).isEqualTo("%=100Z1S1|Z2K4\n");
        assertThat(fut.isDone()).isFalse();

        byteStringUtf8("! =100 S1|S1\n").writeTo(responseSender);
        await().atMost(AWAIT_TIMEOUT).until(fut::isDone);

        assertThatThrownBy(fut::get).isInstanceOf(ExecutionException.class)
                .hasCauseInstanceOf(CommandFailedException.class).hasMessageContaining("CommandFailedException");
    }

    // lots of fiddly sanity checks
    private ResponseSequenceCallback waitAndCheckResult(Future<ResponseSequenceCallback> fut,
            String cmdTxt, String respText, int nr, int ns, int nf, boolean na, int ne, int nne, boolean exec) throws IOException, ExecutionException, InterruptedException {

        assertThat(commandByteCapture.asStringUtf8()).isEqualTo(cmdTxt);
        assertThat(fut.isDone()).isFalse();

        byteStringUtf8(respText).writeTo(responseSender);
        await().atMost(AWAIT_TIMEOUT).until(fut::isDone);

        final ResponseSequenceCallback result = fut.get();
        checkResult(result, nr, ns, nf, na, ne, nne, exec);
        return result;
    }

    // lots of fiddly sanity checks
    private void checkResult(ResponseSequenceCallback result, int nr, int ns, int nf, boolean na, int ne, int nne, boolean exec) {
        assertThat(result.getResponses()).hasSize(nr);
        assertThat(result.getSucceeded()).hasSize(ns);
        assertThat(result.getFailed()).hasSize(nf);
        assertThat(result.getAborted().isPresent()).isEqualTo(na);
        assertThat(result.getExecuted()).hasSize(ne);
        assertThat(result.getNotExecuted()).hasSize(nne);
        assertThat(result.wasExecuted()).isEqualTo(exec);
    }

    @Test
    void shouldSendAndWait() throws InterruptedException, ExecutionException {
        final Future<Object> responderTask = startResponder("%=100Z1S\n", "! =100 S\n");

        final EchoCommand              cmd1   = echoBuilder().setAny('S', 0).build();
        final ResponseSequenceCallback result = device.sendAndWait(cmd1);
        responderTask.get();
        checkResult(result, 1, 1, 0, false, 1, 0, true);
    }

    @Test
    void shouldSendAndWaitButWithFailure() throws InterruptedException, ExecutionException {
        final Future<Object> responderTask = startResponder("%=100Z1S1\n", "! =100 S1\n");

        final EchoCommand              cmd1   = echoBuilder().setAny('S', 1).build();
        final ResponseSequenceCallback result = device.sendAndWait(cmd1);
        responderTask.get();
        checkResult(result, 1, 0, 1, false, 1, 0, true);
    }

    @Test
    void shouldSendAndWaitWithTimeoutAndNoResponse() {
        startResponder("%=100Z1S\n", "");
        final EchoCommand cmd1 = echoBuilder().setAny('S', 0).build();
        assertThatThrownBy(() -> device.sendAndWait(cmd1, 20, MILLISECONDS))
                .isInstanceOf(TimeoutException.class);
    }

    @Test
    void shouldSendAndWaitExpectSuccess() throws InterruptedException, ExecutionException {
        final Future<Object> responderTask = startResponder("%=100Z1S\n", "! =100 S\n");

        final EchoCommand              cmd1   = echoBuilder().setAny('S', 0).build();
        final ResponseSequenceCallback result = device.sendAndWaitExpectSuccess(cmd1);
        responderTask.get();
        checkResult(result, 1, 1, 0, false, 1, 0, true);
    }

    @Test
    void shouldSendAndWaitExpectSuccessButWithFailure() {
        startResponder("%=100Z1S1\n", "! =100 S2\n");

        final EchoCommand cmd1 = echoBuilder().setAny('S', 1).build();
        assertThatThrownBy(() -> device.sendAndWaitExpectSuccess(cmd1)).isInstanceOf(CommandFailedException.class);
    }

    @Test
    void shouldSendAndWaitExpectSuccessWithTimeout() throws InterruptedException, ExecutionException, TimeoutException {
        final Future<Object> responderTask = startResponder("%=100Z1S\n", "! =100 S\n");

        final EchoCommand              cmd1   = echoBuilder().setAny('S', 0).build();
        final ResponseSequenceCallback result = device.sendAndWaitExpectSuccess(cmd1, 3, SECONDS);
        responderTask.get();
        checkResult(result, 1, 1, 0, false, 1, 0, true);
    }

    @Test
    void shouldSendAndWaitExpectSuccessWithTimeoutAndNoResponse() {
        final EchoCommand cmd1 = echoBuilder().setAny('S', 0).build();
        assertThatThrownBy(() -> device.sendAndWaitExpectSuccess(cmd1, 20, MILLISECONDS))
                .isInstanceOf(TimeoutException.class);
    }

    @Test
    void shouldSendAndWaitExpectSuccessButWithAbort() {
        startResponder("%=100Z1S1\n", "! =100 S12\n");

        final EchoCommand cmd1 = echoBuilder().setAny('S', 1).build();
        assertThatThrownBy(() -> device.sendAndWaitExpectSuccess(cmd1)).isInstanceOf(CommandFailedException.class);
    }

    @Test
    void shouldSendBytesWithoutEcho() throws IOException {
        final AtomicReference<ByteString> responseHolder = new AtomicReference<>();
        device.send(byteStringUtf8("Z1\n"), responseHolder::set);
        assertThat(commandByteCapture.asStringUtf8()).isEqualTo("%=100Z1\n");
        byteStringUtf8("!=100S1\n").writeTo(responseSender);
        await().atMost(AWAIT_TIMEOUT).until(() -> responseHolder.get() != null);

        final ByteString result = responseHolder.get();
        assertThat(result.asString()).isEqualTo("S1");
    }

    @Test
    void shouldSendBytesWithEcho() throws IOException {
        final AtomicReference<ByteString> responseHolder = new AtomicReference<>();
        device.send(byteStringUtf8("=104 Z1\n"), responseHolder::set);
        assertThat(commandByteCapture.asStringUtf8()).isEqualTo("=104Z1\n");
        byteStringUtf8("!=104S1\n").writeTo(responseSender);
        await().atMost(AWAIT_TIMEOUT).until(() -> responseHolder.get() != null);

        final ByteString result = responseHolder.get();
        assertThat(result.asString()).isEqualTo("!=104S1");
    }

    @Test
    void shouldRejectBytesWithMultipleSequences() {
        final AtomicReference<ByteString> responseHolder = new AtomicReference<>();
        assertThatIllegalArgumentException().isThrownBy(() -> device.send(byteStringUtf8("Z1\nZ2\n"), responseHolder::set))
                .withMessageContaining("multiple sequences");
    }

    @Test
    void shouldRejectBytesWithParseFailures() {
        final AtomicReference<ByteString> responseHolder = new AtomicReference<>();
        assertThatIllegalArgumentException().isThrownBy(() -> device.send(byteStringUtf8("Z\"\n"), responseHolder::set))
                .withMessageContaining("tokenization failure");
    }

    @Test
    void shouldRejectBytesWithIncompleteSequence() {
        final AtomicReference<ByteString> responseHolder = new AtomicReference<>();
        assertThatIllegalArgumentException().isThrownBy(() -> device.send(byteStringUtf8("Z"), responseHolder::set))
                .withMessageContaining("command sequence incomplete");
    }

    // Handy background action that waits for the command to come out of the connection before replying with the response. Don't forget to call get() on its Future in case it failed.
    private Future<Object> startResponder(String expectedCmd, String resp) {
        return Executors.newSingleThreadExecutor().submit(() -> {
            await().atMost(AWAIT_TIMEOUT).until(() -> commandByteCapture.size() > 0);
            assertThat(commandByteCapture.asStringUtf8()).isEqualTo(expectedCmd);
            byteStringUtf8(resp).writeTo(responseSender);
            return null;
        });
    }
}
