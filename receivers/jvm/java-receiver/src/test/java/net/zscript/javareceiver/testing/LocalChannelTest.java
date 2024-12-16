package net.zscript.javareceiver.testing;

import java.io.IOException;
import java.io.InputStream;
import java.io.PrintStream;
import java.util.Optional;

import static java.nio.charset.StandardCharsets.UTF_8;
import static java.time.Duration.ofSeconds;
import static org.assertj.core.api.Assertions.assertThat;
import static org.awaitility.Awaitility.await;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.mockito.Mockito;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javareceiver.core.SequenceOutStream;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.TokenBuffer.TokenReader;
import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.tokenizer.TokenBufferIterator;
import net.zscript.tokenizer.TokenRingBuffer;
import net.zscript.tokenizer.Tokenizer;

class LocalChannelTest {
    private static final Logger LOG = LoggerFactory.getLogger(LocalChannelTest.class);

    private TokenBuffer                buffer;
    private PrintStream                commands;
    private CollectingConsumer<byte[]> responseCollector;
    private LocalChannel               channel;

    @BeforeEach
    public void setup() throws IOException {
        buffer = TokenRingBuffer.createBufferWithCapacity(20);

        responseCollector = new CollectingConsumer<>();
        channel = new LocalChannel(buffer, responseCollector);

        commands = new PrintStream(channel.getCommandOutputStream(), true);
    }

    // utility method for advancing the channel state
    private static boolean moveChannelAlong(LocalChannel channel, TokenReader tokenReader) {
        channel.moveAlong();
        return tokenReader.hasReadToken();
    }

    @Test
    public void shouldTokenizeSuppliedBytes() {
        final TokenReader tokenReader = buffer.getTokenReader();

        // First bit of test script
        commands.print("Z123 &"); // not println

        await().atMost(ofSeconds(5)).until(() -> moveChannelAlong(channel, tokenReader));

        final TokenBufferIterator tokenBufferIterator = tokenReader.tokenIterator();

        final Optional<ReadToken> token1 = tokenBufferIterator.next();
        assertThat(token1).get()
                .hasFieldOrPropertyWithValue("key", (byte) 'Z')
                .hasFieldOrPropertyWithValue("data16", 0x123)
                .hasFieldOrPropertyWithValue("marker", false);

        final Optional<ReadToken> token2 = tokenBufferIterator.next();
        assertThat(token2).get()
                .hasFieldOrPropertyWithValue("key", Tokenizer.CMD_END_ANDTHEN)
                .hasFieldOrPropertyWithValue("marker", true);

        assertThat(tokenBufferIterator.next()).isNotPresent();

        tokenBufferIterator.flushBuffer();

        // Second bit of test script
        LOG.trace("Sending Y1\\n...");

        commands.println("Y1");
        await().atMost(ofSeconds(5)).until(() -> moveChannelAlong(channel, tokenReader));

        // This is a bit dodgy. Is it ok to call anything on an iterator after it's returned empty? It works for good reason, but...
        final Optional<ReadToken> token3 = tokenBufferIterator.next();
        assertThat(token3).isPresent()
                .get()
                .hasFieldOrPropertyWithValue("key", (byte) 'Y')
                .hasFieldOrPropertyWithValue("data16", 0x1)
                .hasFieldOrPropertyWithValue("marker", false);

        final Optional<ReadToken> token4 = tokenBufferIterator.next();
        assertThat(token4).get()
                .hasFieldOrPropertyWithValue("key", Tokenizer.NORMAL_SEQUENCE_END)
                .hasFieldOrPropertyWithValue("marker", true);
        assertThat(tokenBufferIterator.next()).isNotPresent();
    }

    @Test
    public void shouldRegisterFailureOnReadError() throws IOException {
        // Force the InputStream to fail
        final InputStream cmdStream = mock(InputStream.class);
        when(cmdStream.read(Mockito.any(byte[].class))).thenReturn(0).thenThrow(new IOException("my test exception"));
        makeItRun(cmdStream);
        await().atMost(ofSeconds(5)).until(() -> !responseCollector.isEmpty());
        // E3 implies ERROR
        assertThat(responseCollector.next()).hasValue("\"LocalChannel\"E3".getBytes(UTF_8));
    }

    @Test
    public void shouldRegisterClosureOnEOF() throws IOException {
        // Force the InputStream to fail
        final InputStream emptyStream = mock(InputStream.class);
        when(emptyStream.read(Mockito.any(byte[].class))).thenReturn(-1);

        makeItRun(emptyStream);
        await().atMost(ofSeconds(5)).until(() -> !responseCollector.isEmpty());
        // E1 implies CLOSED
        assertThat(responseCollector.next()).hasValue("\"LocalChannel\"E1".getBytes(UTF_8));
    }

    private void makeItRun(InputStream cmdStream) {
        LocalChannel testChannel = new LocalChannel(buffer, responseCollector) {
            @Override
            void readBytesToQueue(InputStream s) throws IOException {
                super.readBytesToQueue(cmdStream);
            }
        };
        testChannel.moveAlong();
        testChannel.moveAlong();

        final CommandContext    ctx               = mock(CommandContext.class);
        final SequenceOutStream sequenceOutStream = testChannel.getOutStream(null);
        when(ctx.getOutStream()).thenReturn(sequenceOutStream.asCommandOutStream());

        sequenceOutStream.open();
        testChannel.channelInfo(ctx);
        sequenceOutStream.close();
    }

    @Test
    public void shouldOutputChannelInfo() {
        final CommandContext    ctx               = mock(CommandContext.class);
        final SequenceOutStream sequenceOutStream = channel.getOutStream(null);
        when(ctx.getOutStream()).thenReturn(sequenceOutStream.asCommandOutStream());

        sequenceOutStream.open();
        channel.channelInfo(ctx);
        sequenceOutStream.close();

        await().atMost(ofSeconds(5)).until(() -> !responseCollector.isEmpty());
        assertThat(responseCollector.next()).hasValue("\"LocalChannel\"E".getBytes(UTF_8));
    }
}
