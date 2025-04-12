package net.zscript.javaclient.devicenodes;

import java.io.IOException;
import java.io.OutputStream;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.time.Duration;
import java.util.concurrent.atomic.AtomicReference;
import java.util.function.Consumer;

import static java.time.Duration.ofSeconds;
import static java.util.concurrent.TimeUnit.SECONDS;
import static net.zscript.javaclient.commandpaths.ZscriptAddress.address;
import static net.zscript.javaclient.devicenodes.ConnectionBufferTest.parseToResponse;
import static net.zscript.javaclient.devicenodes.ConnectionBufferTest.parseToSequence;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.awaitility.Awaitility.await;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.verify;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandpaths.CommandExecutionPath;
import net.zscript.javaclient.commandpaths.ResponseExecutionPath;
import net.zscript.javaclient.devicenodes.ConnectionBufferTest.MutableTimeSource;
import net.zscript.javaclient.local.LocalConnection;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javaclient.sequence.ResponseSequence;
import net.zscript.util.ByteString;
import net.zscript.util.ByteString.ByteStringBuilder;

class ZscriptBasicNodeTest {
    private static final Duration AWAIT_TIMEOUT = ofSeconds(2);

    private ZscriptCallbackThreadpool pool;

    // use commandByteCapture.asStringUtf8() to intercept commands on their way to the target
    private final ByteStringBuilder commandByteCapture = ByteString.builder();
    // write responses here to simulate device response
    private       OutputStream      responseSender;

    private ZscriptBasicNode node;
    private Connection       connection;

    private MutableTimeSource mutableTimeSource = new MutableTimeSource();

    @BeforeEach
    public void setup() throws IOException {
        pool = spy(new ZscriptCallbackThreadpool());
        final OutputStream     commandOut = commandByteCapture.asOutputStream();
        final PipedInputStream responseIn = new PipedInputStream();
        responseSender = new PipedOutputStream(responseIn);
        connection = spy(new LocalConnection(commandOut, responseIn));
        node = new ZscriptBasicNode(pool, connection, 128, 3, SECONDS, mutableTimeSource);
    }

    @AfterEach
    public void teardown() throws IOException {
        responseSender.close();
    }

    @Test
    public void shouldSendSequence() {
        final AtomicReference<ResponseSequence> responseHolder = new AtomicReference<>();
        final CommandSequence                   seq            = parseToSequence("Z1");
        node.send(seq, responseHolder::set);

        verify(connection).send(any());
        assertThat(commandByteCapture.asStringUtf8()).isEqualTo("Z1\n");
        assertThat(responseHolder).hasNullValue();
    }

    @Test
    public void shouldReceiveSequenceResponse() throws IOException {
        final AtomicReference<ResponseSequence> responseHolder = new AtomicReference<>();

        final CommandSequence seq = parseToSequence(">100 Z1");
        node.send(seq, responseHolder::set);
        verify(connection).send(any());
        assertThat(commandByteCapture.asStringUtf8()).isEqualTo(">100Z1\n");

        byteStringUtf8("! >100 S \n").writeTo(responseSender);
        await().atMost(AWAIT_TIMEOUT).until(() -> responseHolder.get() != null);

        final ResponseSequence responseSequence = parseToResponse("! >100 S");
        assertThat(responseHolder).doesNotHaveNullValue()
                .hasValueMatching(v -> v.asStringUtf8().equals(responseSequence.asStringUtf8()));
    }

    @Test
    public void shouldSendPath() {
        // no response in this test
        setupHandlerAndSend("Z1", ">100Z1\n", r -> {
        });
    }

    @Test
    public void shouldReceivePathResponse() throws IOException {
        final AtomicReference<ResponseExecutionPath> responseHolder = new AtomicReference<>();
        setupHandlerAndSend("Z1", ">100Z1\n", responseHolder::set);

        byteStringUtf8("! >100 S \n").writeTo(responseSender);
        await().atMost(AWAIT_TIMEOUT).until(() -> responseHolder.get() != null);

        final ResponseExecutionPath responsePath = parseToResponse("!S").getExecutionPath();
        assertThat(responseHolder).doesNotHaveNullValue();
        assertThat(responseHolder.get().asStringUtf8()).isEqualTo(responsePath.asStringUtf8());
    }

    @Test
    public void shouldReceiveNotification() throws IOException {
        final AtomicReference<ResponseSequence> responseHolder = new AtomicReference<>();
        node.setNotificationHandler(3, responseHolder::set);

        byteStringUtf8("!3 S \n").writeTo(responseSender);
        await().atMost(AWAIT_TIMEOUT).until(() -> responseHolder.get() != null);

        final ResponseSequence responseSequence = parseToResponse("!3 S");
        assertThat(responseHolder).doesNotHaveNullValue();
        assertThat(responseHolder.get().asStringUtf8()).isEqualTo(responseSequence.asStringUtf8());
    }

    @Test
    public void shouldCallUnknownResponseHandler() throws IOException {
        final AtomicReference<AddressedResponse> unknownResponseHandler = new AtomicReference<>();
        node.setUnknownResponseHandler(unknownResponseHandler::set);

        final AtomicReference<ResponseExecutionPath> responseHolder = new AtomicReference<>();
        setupHandlerAndSend("Z1", ">100Z1\n", responseHolder::set);

        // Note non-matching echo
        byteStringUtf8("! >999 S \n").writeTo(responseSender);

        await().atMost(AWAIT_TIMEOUT).until(() -> unknownResponseHandler.get() != null);
        assertThat(responseHolder).hasNullValue();

        final ResponseExecutionPath responsePath = parseToResponse("!S").getExecutionPath();
        assertThat(unknownResponseHandler).doesNotHaveNullValue();
        assertThat(unknownResponseHandler.get().getResponseSequence().getExecutionPath().asStringUtf8())
                .isEqualTo(responsePath.asStringUtf8());
    }

    @Test
    public void shouldCallUnknownResponseHandlerOnUnknownAddress() throws IOException {
        final AtomicReference<AddressedResponse> unknownResponseHandler = new AtomicReference<>();
        node.setUnknownResponseHandler(unknownResponseHandler::set);

        final AtomicReference<ResponseExecutionPath> responseHolder = new AtomicReference<>();
        setupHandlerAndSend("Z1", ">100Z1\n", responseHolder::set);

        // Note unregistered address
        byteStringUtf8("@1 ! >100 S\n").writeTo(responseSender);

        await().atMost(AWAIT_TIMEOUT).until(() -> unknownResponseHandler.get() != null);
        assertThat(responseHolder).hasNullValue();

        final ResponseExecutionPath responsePath = parseToResponse("!S").getExecutionPath();
        assertThat(unknownResponseHandler).doesNotHaveNullValue();
        assertThat(unknownResponseHandler.get().getResponseSequence().getExecutionPath().asStringUtf8())
                .isEqualTo(responsePath.asStringUtf8());
    }

    @Test
    public void shouldCallBadCommandResponseMatchHandler() throws IOException {
        final AtomicReference<AddressedResponse> badMatchHandler = new AtomicReference<>();
        node.setBadCommandResponseMatchHandler((cmd, resp) -> badMatchHandler.set(resp));

        final AtomicReference<ResponseExecutionPath> responseHolder = new AtomicReference<>();
        setupHandlerAndSend("Z1", ">100Z1\n", responseHolder::set);

        // Note mismatching response syntax
        byteStringUtf8("! >100 S|S \n").writeTo(responseSender);

        await().atMost(AWAIT_TIMEOUT).until(() -> badMatchHandler.get() != null);
        assertThat(responseHolder).hasNullValue();

        final ResponseSequence responseSeq = parseToResponse("!>100S|S");
        assertThat(badMatchHandler).doesNotHaveNullValue();
        assertThat(badMatchHandler.get().getResponseSequence().asStringUtf8()).isEqualTo(responseSeq.asStringUtf8());
    }

    @Test
    public void shouldCallCallbackExceptionHandler() throws IOException {
        final AtomicReference<Exception> exceptionHandler = new AtomicReference<>();
        node.setCallbackExceptionHandler(exceptionHandler::set);

        // response handler throws exception to trigger exception handler
        setupHandlerAndSend("Z1", ">100Z1\n", r -> {
            throw new RuntimeException("dummy exception (broken response handler)");
        });

        byteStringUtf8("! >100 S \n").writeTo(responseSender);

        await().atMost(AWAIT_TIMEOUT).until(() -> exceptionHandler.get() != null);

        assertThat(exceptionHandler).doesNotHaveNullValue();
        assertThat(exceptionHandler.get()).isInstanceOf(RuntimeException.class);
    }

    @Test
    public void shouldCallUnknownNotificationHandler() throws IOException {
        final AtomicReference<AddressedResponse> badMatchHandler = new AtomicReference<>();
        node.setUnknownNotificationHandler(badMatchHandler::set);

        // Add a normal notification handler - and then remove it and reassign it to a different notification id!
        final AtomicReference<ResponseSequence> responseHolder = new AtomicReference<>();
        node.setNotificationHandler(3, responseHolder::set);
        node.removeNotificationHandler(3);

        node.setNotificationHandler(432, responseHolder::set);

        byteStringUtf8("!3 S \n").writeTo(responseSender);
        await().atMost(AWAIT_TIMEOUT).until(() -> badMatchHandler.get() != null);
        assertThat(responseHolder).hasNullValue();

        final ResponseSequence responseSequence = parseToResponse("!3 S");
        assertThat(badMatchHandler).doesNotHaveNullValue();
        assertThat(badMatchHandler.get().getResponseSequence().asStringUtf8()).isEqualTo(responseSequence.asStringUtf8());
    }

    @Test
    public void shouldSendOverMultipleNodeLayers() {
        final ZscriptBasicNode subSubNode = makeNestedNodes();

        final AtomicReference<ResponseExecutionPath> subSubHandler = new AtomicReference<>();
        final CommandExecutionPath                   seq           = parseToSequence("Z1").getExecutionPath();
        subSubNode.send(seq, subSubHandler::set);

        verify(connection).send(any());
        assertThat(commandByteCapture.asStringUtf8()).isEqualTo("@1.2@3.4>100Z1\n");
        assertThat(subSubHandler).hasNullValue();
        assertThat(node.isAttached(address(3, 4))).isFalse();
        assertThat(node.isAttached(address(1, 2))).isTrue();
    }

    @Test
    public void shouldReceiveOverMultipleNodeLayers() throws IOException {
        final ZscriptBasicNode subSubNode = makeNestedNodes();

        final AtomicReference<ResponseExecutionPath> responseHolder = new AtomicReference<>();

        final CommandExecutionPath path = parseToSequence("Z1").getExecutionPath();
        subSubNode.send(path, responseHolder::set);
        verify(connection).send(any());
        assertThat(commandByteCapture.asStringUtf8()).isEqualTo("@1.2@3.4>100Z1\n");

        byteStringUtf8("@1.2 @3.4 ! >100 S \n").writeTo(responseSender);

        await().atMost(AWAIT_TIMEOUT).until(() -> responseHolder.get() != null);

        final ResponseExecutionPath responsePath = parseToResponse("!S").getExecutionPath();
        assertThat(responseHolder).doesNotHaveNullValue();
        assertThat(responseHolder.get().asStringUtf8()).isEqualTo(responsePath.asStringUtf8());
    }

    @Test
    public void shouldSetStrategy() {
        QueuingStrategy s = mock(QueuingStrategy.class);
        node.setStrategy(s);
        verify(s).setBuffer(any(ConnectionBuffer.class));
    }

    @Test
    public void shouldCheckTimeouts() {
        final AtomicReference<ResponseExecutionPath> responseHolder = new AtomicReference<>();
        setupHandlerAndSend("Z1A", ">100Z1A\n", responseHolder::set);
        verify(connection).send(any());
        assertThat(commandByteCapture.asStringUtf8()).isEqualTo(">100Z1A\n");

        ConnectionBuffer buffer = node.getConnectionBuffer();
        assertThat(buffer.getQueueLength()).isEqualTo(1);

        // verify that checking timeouts doesn't affect things until time has moved on
        mutableTimeSource.t = SECONDS.toNanos(4);
        node.checkTimeouts();
        assertThat(buffer.getQueueLength()).isEqualTo(1);
        mutableTimeSource.t = SECONDS.toNanos(6);
        node.checkTimeouts();
        assertThat(buffer.getQueueLength()).isEqualTo(0);

        await().atMost(AWAIT_TIMEOUT).until(() -> responseHolder.get() != null);

        assertThat(responseHolder).doesNotHaveNullValue();
        assertThat(responseHolder.get().asStringUtf8()).isEmpty();
        assertThat(responseHolder.get().isBlank()).isTrue();
    }

    private ZscriptBasicNode makeNestedNodes() {
        final Connection       subConn    = node.attach(address(1, 2));
        final ZscriptBasicNode subNode    = new ZscriptBasicNode(node.getParentConnection().getAssociatedThread().getCallbackPool(), subConn, 128, 3, SECONDS);
        final Connection       subSubConn = subNode.attach(address(3, 4));
        return new ZscriptBasicNode(subNode.getParentConnection().getAssociatedThread().getCallbackPool(), subSubConn, 128, 3, SECONDS);
    }

    private void setupHandlerAndSend(String z, String expected, Consumer<ResponseExecutionPath> responseHolder) {
        final CommandExecutionPath path = parseToSequence(z).getExecutionPath();
        node.send(path, responseHolder);
        verify(connection).send(any());
        assertThat(commandByteCapture.asStringUtf8()).isEqualTo(expected);
    }

}
