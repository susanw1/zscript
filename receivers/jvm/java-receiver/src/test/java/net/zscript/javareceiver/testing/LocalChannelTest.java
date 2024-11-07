package net.zscript.javareceiver.testing;

import java.io.IOException;
import java.io.InputStream;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.io.PrintStream;
import java.io.UncheckedIOException;
import java.util.Optional;

import static java.time.Duration.ofSeconds;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;
import static org.awaitility.Awaitility.await;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;

import net.zscript.javareceiver.core.CommandOutStream;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.TokenBuffer.TokenReader;
import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.tokenizer.TokenBufferIterator;
import net.zscript.tokenizer.TokenRingBuffer;
import net.zscript.tokenizer.Tokenizer;

class LocalChannelTest {
    private TokenBuffer        buffer;
    private PrintStream        commands;
    private CollectingConsumer responseCollector;
    private LocalChannel       channel;

    @BeforeEach
    public void setup() throws IOException {
        buffer = TokenRingBuffer.createBufferWithCapacity(20);

        // You don't see these often:
        final PipedInputStream commandStream = new PipedInputStream();
        commands = new PrintStream(new PipedOutputStream(commandStream), true);

        responseCollector = new CollectingConsumer();
        channel = new LocalChannel(buffer, commandStream, responseCollector);
    }

    // utility method for advancing the channel state
    private static boolean moveChannelAlong(LocalChannel channel, TokenReader tokenReader) {
        channel.moveAlong();
        return tokenReader.hasReadToken();
    }

    @Test
    public void shouldTokenizeSuppliedBytes() throws IOException {
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

        // Second bit of test script
        commands.println("Y1");
        commands.close();
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
    public void shouldThrowOnReadError() throws IOException {
        // Force the InputStream to fail
        final InputStream cmdStream = mock(InputStream.class);
        when(cmdStream.read()).thenReturn((int) 'Z').thenThrow(IOException.class);

        LocalChannel brokenChannel = new LocalChannel(buffer, cmdStream, null);
        assertThatThrownBy(() -> await().atMost(ofSeconds(5)).until(() -> moveChannelAlong(brokenChannel, buffer.getTokenReader())))
                .isInstanceOf(UncheckedIOException.class);
    }

    @Disabled("channel isn't feeding back yet")
    @Test
    public void shouldOutputChannelInfo() throws InterruptedException {
        final CommandContext   ctx              = mock(CommandContext.class);
        final CommandOutStream commandOutStream = channel.getOutStream(null).asCommandOutStream();
        when(ctx.getOutStream()).thenReturn(commandOutStream);
        channel.channelInfo(ctx);
        Thread.sleep(1000);
        assertThat(responseCollector.next()).isEqualTo("\"LocalChannel\"");
    }
}
