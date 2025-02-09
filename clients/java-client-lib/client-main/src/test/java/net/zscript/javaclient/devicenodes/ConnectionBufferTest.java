package net.zscript.javaclient.devicenodes;

import javax.annotation.Nullable;
import java.util.Collection;
import java.util.concurrent.TimeUnit;

import static net.zscript.javaclient.commandpaths.ZscriptAddress.address;
import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.verifyNoMoreInteractions;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.mockito.Mockito;

import net.zscript.javaclient.commandpaths.ZscriptAddress;
import net.zscript.javaclient.devicenodes.ConnectionBuffer.TimeSource;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javaclient.sequence.ResponseSequence;
import net.zscript.model.ZscriptModel;

class ConnectionBufferTest {
    Connection        connection   = Mockito.mock(Connection.class);
    EchoAssigner      echoAssigner = Mockito.spy(new EchoAssigner());
    int               bufSize      = 128;
    MutableTimeSource timeSource   = new MutableTimeSource();
    ConnectionBuffer  buffer;

    @BeforeEach
    public void setup() {
        buffer = new ConnectionBuffer(connection, echoAssigner, bufSize, timeSource);
    }

    @Test
    public void shouldEnqueueCommandSequencesWithNoEcho() {
        sendSeqToBuffer("Z1", true, 1);
        // This assumes there's an echo value already (of "unset" (-1), which clashes with previous message) - don't think this is correct though.
        sendSeqToBuffer("Z1", false, 1);

        verify(connection).send(any());
        verify(echoAssigner).moveEcho(); // why? no getEcho?
        verifyNoMoreInteractions(echoAssigner, connection);
    }

    @Test
    public void shouldNotEnqueueCommandSequenceWithDuplicateEchoValue() {
        sendSeqToBuffer("_100 Z1", true, 1);
        // 2nd one blocked
        sendSeqToBuffer("_100 Z1", false, 1);

        verify(connection).send(any());
        verify(echoAssigner).manualEchoUse(0x100);
        verifyNoMoreInteractions(echoAssigner, connection);
    }

    @Test
    public void shouldEnqueueCommandExecutionPath() {
        sendPathToBuffer("Z1", true, 1);
        // 2nd one allowed, because echo is assigned automatically
        sendPathToBuffer("Z1", true, 2);

        verify(connection, times(2)).send(any());
        verify(echoAssigner, times(2)).getEcho();
        verify(echoAssigner, times(2)).moveEcho();
        verifyNoMoreInteractions(echoAssigner, connection);
    }

    @Test
    public void shouldForwardAddressedCommandsWithoutEchoes() {
        forwardSeq("Z1", null, true, 1);
        // This assumes there's an echo value already (of "unset" (-1), which clashes with previous message) - don't think this is correct though.
        forwardSeq("Z1", null, false, 1);

        verify(connection).send(any());
        verify(echoAssigner).moveEcho(); // why? no getEcho?
        verifyNoMoreInteractions(echoAssigner, connection);
    }

    @Test
    public void shouldForwardAddressedCommandsWithAddresses() {
        forwardSeq("Z1", address(1, 2), true, 1);
        forwardSeq("Z1", address(2, 6), true, 2);

        verify(connection, times(2)).send(any());
        verify(echoAssigner, times(2)).moveEcho(); // why? again, no getEcho?
        verifyNoMoreInteractions(echoAssigner, connection);
    }

    @Test
    public void shouldMatchResponsesWithManualEcho() {
        sendSeqToBuffer("_100 Z1", true, 1);

        verify(connection).send(any());
        verify(echoAssigner).manualEchoUse(0x100);

        ResponseSequence rs              = parseToResponse("! _100 S");
        AddressedCommand matchingCommand = buffer.match(rs);
        assertThat(matchingCommand).isNotNull();
        assertThat(matchingCommand.getCommandSequence()).isNotNull();
        assertThat(buffer.getQueueLength()).isEqualTo(0);

        verify(echoAssigner).responseArrivedNormal(0x100);
        verifyNoMoreInteractions(echoAssigner, connection);
    }

    @Test
    public void shouldMatchResponsesWithAutoEchoAndSingleMessage() {
        sendPathToBuffer("Z1", true, 1);
        //        final CommandSequence cmdSeq     = parseToSequence("Z1");
        //        final boolean         wasQueued1 = buffer.send(cmdSeq.getExecutionPath(), false, 100, TimeUnit.MILLISECONDS);
        //        assertThat(wasQueued1).isTrue();

        verify(connection).send(any());
        verify(echoAssigner).getEcho();
        verify(echoAssigner).moveEcho();

        final ResponseSequence rs              = parseToResponse("! _100 S");
        final AddressedCommand matchingCommand = buffer.match(rs);
        assertThat(matchingCommand).isNotNull();
        assertThat(matchingCommand.getCommandSequence()).isNotNull();

        verify(echoAssigner).responseArrivedNormal(0x100);
        verifyNoMoreInteractions(echoAssigner, connection);
    }

    @Test
    public void shouldMatchResponsesWithAutoEchoAndMultipleQueuedMessages() {
        sendPathToBuffer("Z1", true, 1);
        sendPathToBuffer("Z1", true, 2);

        verify(connection, times(2)).send(any());
        verify(echoAssigner, times(2)).getEcho();
        verify(echoAssigner, times(2)).moveEcho();

        ResponseSequence rs              = parseToResponse("! _101 S");
        AddressedCommand matchingCommand = buffer.match(rs);
        assertThat(matchingCommand).isNotNull();
        assertThat(matchingCommand.getCommandSequence()).isNotNull();

        verify(echoAssigner).responseArrivedNormal(0x101);
        verifyNoMoreInteractions(echoAssigner, connection);
    }

    @Test
    public void shouldRemoveUnmatchedResponses() {
        sendPathToBuffer("Z1", true, 1);
        verify(connection).send(any());
        verify(echoAssigner).getEcho();
        verify(echoAssigner).moveEcho();

        ResponseSequence rs              = parseToResponse("! _100 S|S");
        AddressedCommand matchingCommand = buffer.match(rs);
        // this is odd - it failed the match but we return it anyway? Why?
        assertThat(matchingCommand).isNotNull();
        assertThat(matchingCommand.getCommandSequence()).isNotNull();
        assertThat(buffer.getQueueLength()).isEqualTo(1);

        verifyNoMoreInteractions(echoAssigner, connection);
    }

    @Test
    public void shouldNotMatchStrayResponses() {
        sendPathToBuffer("Z1", true, 1);
        sendPathToBuffer("Z1", true, 2);

        verify(connection, times(2)).send(any());
        verify(echoAssigner, times(2)).getEcho();
        verify(echoAssigner, times(2)).moveEcho();

        ResponseSequence rs              = parseToResponse("! _105 S|S");
        AddressedCommand matchingCommand = buffer.match(rs);
        // this is odd - it failed the match but we return it anyway? Why?
        assertThat(matchingCommand).isNull();
        assertThat(buffer.getQueueLength()).isEqualTo(2);

        verifyNoMoreInteractions(echoAssigner, connection);
    }

    @Test
    public void shouldIgnoreMatchesWithNoResponseEcho() {
        ResponseSequence rs = parseToResponse("! S");
        assertThat(buffer.match(rs)).isNull();

        verifyNoMoreInteractions(echoAssigner, connection);
    }

    @Test
    public void shouldTimeout() {
        enqueueCommands("Z1", 5);

        assertThat(buffer.getQueueLength()).isEqualTo(5);

        // 200ms means 0, 40ms, 80ms have timed out.
        final Collection<CommandSequence> timedoutCommands = buffer.checkTimeouts();
        assertThat(timedoutCommands).hasSize(3);

        verify(connection, times(5)).send(any());
        verify(echoAssigner, times(5)).getEcho();
        verify(echoAssigner, times(5)).moveEcho();

        for (int i = 0; i < 3; i++) {
            verify(echoAssigner).timeout(0x100 + i);
        }
        verifyNoMoreInteractions(echoAssigner, connection);
    }

    @Test
    public void shouldUnqueueAddressedCommandsOnMatch() {
        forwardSeq("_100 Z1A", address(2), true, 1);
        forwardSeq("_101 Z1B", address(1), true, 2);
        // dummy one, just to force removal to not run to the end of the queue:
        forwardSeq("_102 Z1C", address(1), true, 3);

        assertThat(buffer.getQueueLength()).isEqualTo(3);
        assertThat(buffer.responseMatchedBySubnode(buffer.getQueueItem(1))).isTrue();
        assertThat(buffer.getQueueLength()).isEqualTo(1);
    }

    @Test
    public void shouldNotUnqueueUnknownAddressedCommandsOnMatch() {
        forwardSeq("_100 Z1A", address(2), true, 1);
        forwardSeq("_101 Z1B", address(1), true, 2);
        // dummy one, just to force removal to not run to the end of the queue:
        forwardSeq("_102 Z1C", address(1), true, 3);

        assertThat(buffer.getQueueLength()).isEqualTo(3);
        AddressedCommand cmd = new AddressedCommand(parseToSequence("_105 S"));
        assertThat(buffer.responseMatchedBySubnode(cmd)).isFalse();
        assertThat(buffer.getQueueLength()).isEqualTo(3);
    }

    @Test
    public void shouldDetectNonAddressedCommands() {
        forwardSeq("_100 Z1A", address(2), true, 1);
        forwardSeq("_101 Z1B", address(1), true, 2);
        assertThat(buffer.hasNonAddressedInBuffer()).isFalse();

        forwardSeq("_102 Z1B", null, true, 3);
        assertThat(buffer.hasNonAddressedInBuffer()).isTrue();
    }

    private void enqueueCommands(String z, int count) {
        for (int i = 0; i < count; i++) {
            // time: 0, 40, 80, 120, 160ms
            final CommandSequence z1         = parseToSequence(z);
            final boolean         wasQueued1 = buffer.send(z1.getExecutionPath(), false, 100, TimeUnit.MILLISECONDS);
            assertThat(wasQueued1).isTrue();
            timeSource.t = TimeUnit.NANOSECONDS.convert(40 * i, TimeUnit.MILLISECONDS);
        }
    }

    private void sendSeqToBuffer(String z, boolean expectQueued, int bufCCount) {
        boolean wasQueued = buffer.send(parseToSequence(z), false, 100, TimeUnit.MILLISECONDS);
        assertThat(wasQueued).isEqualTo(expectQueued);
        assertThat(buffer.getQueueLength()).isEqualTo(bufCCount);
    }

    private void sendPathToBuffer(String z, boolean expectQueued, int bufCCount) {
        boolean wasQueued = buffer.send(parseToSequence(z).getExecutionPath(), false, 100, TimeUnit.MILLISECONDS);
        assertThat(wasQueued).isEqualTo(expectQueued);
        assertThat(buffer.getQueueLength()).isEqualTo(bufCCount);
    }

    private void forwardSeq(String z, @Nullable ZscriptAddress address, boolean expectQueued, int bufCCount) {
        final AddressedCommand cmd = new AddressedCommand(parseToSequence(z));
        if (address != null) {
            cmd.prefixAddressLayer(address);
        }
        final boolean wasQueued = buffer.forward(cmd, false, 100, TimeUnit.MILLISECONDS);
        assertThat(wasQueued).isEqualTo(expectQueued);
        assertThat(buffer.getQueueLength()).isEqualTo(bufCCount);
    }

    static CommandSequence parseToSequence(String s) {
        return CommandSequence.parse(ZscriptModel.standardModel(),
                tokenize(byteStringUtf8(s), true).getTokenReader().getFirstReadToken(), false);
    }

    static ResponseSequence parseToResponse(String s) {
        return ResponseSequence.parse(tokenize(byteStringUtf8(s), true).getTokenReader().getFirstReadToken());
    }

    static class MutableTimeSource implements TimeSource {
        long t = 0;

        @Override
        public long nanoTime() {
            return t;
        }
    }
}
