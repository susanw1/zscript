package net.zscript.tokenizer;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

class TokenBufferFlagsTest {
    TokenBufferFlags flags = new TokenBufferFlags();

    @Test
    public void shouldInitializeCorrectlyWithReaderBlockerAndWriterUnblocked() {
        assertThat(flags.isReaderBlocked()).isTrue();

        // double-check that clear works
        assertThat(flags.getAndClearMarkerWritten()).isFalse();
        assertThat(flags.getAndClearMarkerWritten()).isFalse();

        assertThat(flags.getAndClearSeqMarkerWritten()).isFalse();
        assertThat(flags.getAndClearSeqMarkerWritten()).isFalse();
    }

    @Test
    public void shouldSetAndClearMarkerState() {
        assertThat(flags.getAndClearMarkerWritten()).isFalse();
        flags.setMarkerWritten();

        // verify that SeqMarker is unaffected
        assertThat(flags.getAndClearSeqMarkerWritten()).isFalse();

        // double-check that clear works
        assertThat(flags.getAndClearMarkerWritten()).isTrue();
        assertThat(flags.getAndClearMarkerWritten()).isFalse();
    }

    @Test
    public void shouldSetAndClearSeqMarkerState() {
        assertThat(flags.getAndClearSeqMarkerWritten()).isFalse();
        flags.setSeqMarkerWritten();

        // verify that non SeqMarker is unaffected
        assertThat(flags.getAndClearMarkerWritten()).isFalse();

        // double-check that clear works
        assertThat(flags.getAndClearSeqMarkerWritten()).isTrue();
        assertThat(flags.getAndClearSeqMarkerWritten()).isFalse();
    }

    @Test
    public void shouldSetAndClearReaderBlockedState() {
        assertThat(flags.isReaderBlocked()).isTrue();
        flags.clearReaderBlocked();
        assertThat(flags.isReaderBlocked()).isFalse();
        flags.setReaderBlocked();
        assertThat(flags.isReaderBlocked()).isTrue();
    }
}
