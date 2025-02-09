package net.zscript.javaclient.local;

import java.io.IOException;
import java.io.OutputStream;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.time.Duration;
import java.util.concurrent.atomic.AtomicReference;

import static java.time.Duration.ofSeconds;
import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatIllegalStateException;
import static org.awaitility.Awaitility.await;
import static org.mockito.Mockito.spy;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import net.zscript.javaclient.devicenodes.AddressedCommand;
import net.zscript.javaclient.devicenodes.AddressedResponse;
import net.zscript.javaclient.devicenodes.DirectConnection;
import net.zscript.javaclient.devicenodes.ZscriptCallbackThreadpool;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.model.ZscriptModel;
import net.zscript.util.ByteString;

/**
 * Testing {@link DirectConnection} too.
 */
class LocalConnectionTest {
    private static final Duration AWAIT_TIMEOUT = ofSeconds(2);

    private ZscriptCallbackThreadpool pool;

    // use commandByteCapture.asStringUtf8() to intercept commands on their way to the target
    private final ByteString.ByteStringBuilder commandByteCapture = ByteString.builder();
    // write responses here to simulate device response
    private       OutputStream                 responseSender;

    private LocalConnection connection;

    @BeforeEach
    public void setup() throws IOException {
        pool = new ZscriptCallbackThreadpool();
        final OutputStream     commandOut = commandByteCapture.asOutputStream();
        final PipedInputStream responseIn = new PipedInputStream();
        responseSender = new PipedOutputStream(responseIn);
        connection = spy(new LocalConnection(commandOut, responseIn));
    }

    @AfterEach
    public void teardown() throws IOException {
        responseSender.close();
    }

    @Test
    public void shouldSendAddressedCommand() {
        connection.send(new AddressedCommand(parseToSequence("Z1A")));
        assertThat(commandByteCapture.asStringUtf8()).isEqualTo("Z1A\n");
    }

    @Test
    public void shouldReceiveAddressedResponse() throws IOException {
        AtomicReference<AddressedResponse> responseCaptor = new AtomicReference<>();
        connection.onReceive(responseCaptor::set);
        byteStringUtf8("!S \n").writeTo(responseSender);
        await().atMost(AWAIT_TIMEOUT).until(() -> responseCaptor.get() != null);
        assertThat(responseCaptor.get().getResponseSequence().asStringUtf8()).isEqualTo("!S");
    }

    @Test
    public void shouldReceiveAddressedResponseInPartialBlocks() throws IOException {
        AtomicReference<AddressedResponse> responseCaptor = new AtomicReference<>();
        connection.onReceive(responseCaptor::set);

        byteStringUtf8("!S").writeTo(responseSender);
        byteStringUtf8(" A1").writeTo(responseSender);
        assertThat(responseCaptor).hasNullValue();
        byteStringUtf8("\n!S B").writeTo(responseSender);
        await().atMost(AWAIT_TIMEOUT).until(() -> responseCaptor.get() != null);
        assertThat(responseCaptor.get().getResponseSequence().asStringUtf8()).isEqualTo("!A1S");

        // clear responseCaptor so we can tell when next response arrives
        responseCaptor.set(null);
        byteStringUtf8("2\n").writeTo(responseSender);
        await().atMost(AWAIT_TIMEOUT).until(() -> responseCaptor.get() != null);
        assertThat(responseCaptor.get().getResponseSequence().asStringUtf8()).isEqualTo("!B2S");
    }

    @Test
    public void shouldFlushOnClose() throws IOException {
        AtomicReference<AddressedResponse> responseCaptor = new AtomicReference<>();
        connection.onReceive(responseCaptor::set);
        // ensure receiver thread is alive
        byteStringUtf8("!S\n").writeTo(responseSender);
        await().atMost(AWAIT_TIMEOUT).until(() -> responseCaptor.get() != null);
        responseCaptor.set(null);
        byteStringUtf8("!AS\n").writeTo(responseSender);
        connection.close();
        await().atMost(AWAIT_TIMEOUT).until(() -> responseCaptor.get() != null);
        assertThat(responseCaptor.get().getResponseSequence().asStringUtf8()).isEqualTo("!AS");
    }

    @Test
    public void shouldRejectMultipleOnReceives() {
        AtomicReference<AddressedResponse> dummyCaptor = new AtomicReference<>();
        connection.onReceive(dummyCaptor::set);
        assertThatIllegalStateException().isThrownBy(() -> connection.onReceive(dummyCaptor::set));
    }

    static CommandSequence parseToSequence(String s) {
        return CommandSequence.parse(ZscriptModel.standardModel(),
                tokenize(byteStringUtf8(s), true).getTokenReader().getFirstReadToken(), false);
    }

}
