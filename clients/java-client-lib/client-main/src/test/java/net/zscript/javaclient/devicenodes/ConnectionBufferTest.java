package net.zscript.javaclient.devicenodes;

import java.util.concurrent.TimeUnit;

import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.mockito.Mockito;

import net.zscript.javaclient.devicenodes.ConnectionBuffer.TimeSource;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.model.ZscriptModel;

class ConnectionBufferTest {
    Connection       connection   = Mockito.mock(Connection.class);
    EchoAssigner     echoAssigner = new EchoAssigner();
    int              bufSize      = 128;
    TimeSource       timeSource   = new MutableTimeSource();
    ConnectionBuffer buffer;

    @BeforeEach
    public void setup() {
        buffer = new ConnectionBuffer(connection, echoAssigner, bufSize, timeSource);
    }

    @Test
    public void shouldEnqueueCommandSequence() {
        boolean wasQueued1 = buffer.send(parseToSequence("Z1"), false, 100, TimeUnit.MILLISECONDS);
        assertThat(wasQueued1).isTrue();
        assertThat(buffer.getQueueLength()).isEqualTo(1);

        // We assume there's an echo value already (of "unset", which clashes with previous message) - don't think that's correct though.
        boolean wasQueued2 = buffer.send(parseToSequence("Z1"), false, 100, TimeUnit.MILLISECONDS);
        assertThat(wasQueued2).isFalse();
        assertThat(buffer.getQueueLength()).isEqualTo(1);
    }

    @Test
    public void shouldNotEnqueueCommandSequenceWithDuplicateEchoValue() {
        boolean wasQueued1 = buffer.send(parseToSequence("_100 Z1"), false, 100, TimeUnit.MILLISECONDS);
        assertThat(wasQueued1).isTrue();
        assertThat(buffer.getQueueLength()).isEqualTo(1);

        boolean wasQueued2 = buffer.send(parseToSequence("_100 Z1"), false, 100, TimeUnit.MILLISECONDS);
        assertThat(wasQueued2).isFalse();
        assertThat(buffer.getQueueLength()).isEqualTo(1);
    }

    @Test
    public void shouldEnqueueCommandExecutionPath() {
        boolean wasQueued = buffer.send(parseToSequence("Z1").getExecutionPath(), false, 100, TimeUnit.MILLISECONDS);
        assertThat(wasQueued).isTrue();
        assertThat(buffer.getQueueLength()).isEqualTo(1);
    }

    private static CommandSequence parseToSequence(String s) {
        return CommandSequence.parse(ZscriptModel.standardModel(),
                tokenize(byteStringUtf8(s), true).getTokenReader().getFirstReadToken(), false);
    }

    static class MutableTimeSource implements TimeSource {
        long t = 0;

        @Override
        public long nanoTime() {
            return t;
        }
    }
}
