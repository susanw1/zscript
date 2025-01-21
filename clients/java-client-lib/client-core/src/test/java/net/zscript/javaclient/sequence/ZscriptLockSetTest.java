package net.zscript.javaclient.sequence;

import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.tokenizer.TokenBuffer;

class ZscriptLockSetTest {

    @Test
    public void shouldSetAllLocks() {
        final ZscriptLockSet locks16 = ZscriptLockSet.allLocked(false);
        assertThat(locks16.isAllSet()).isTrue();
        assertThat(locks16.asStringUtf8()).isEmpty();

        final ZscriptLockSet locks32 = ZscriptLockSet.allLocked(true);
        assertThat(locks32.isAllSet()).isTrue();
        assertThat(locks32.asStringUtf8()).isEmpty();
    }

    @Test
    public void shouldSetNoLocks() {
        final ZscriptLockSet locks16 = ZscriptLockSet.noneLocked(false);
        assertThat(locks16.isAllSet()).isFalse();
        assertThat(locks16.asStringUtf8()).isEqualTo("%");

        final ZscriptLockSet locks32 = ZscriptLockSet.noneLocked(true);
        assertThat(locks32.isAllSet()).isFalse();
        assertThat(locks16.asStringUtf8()).isEqualTo("%");
    }

    @Test
    public void shouldSetSpecificLocks() {
        final ZscriptLockSet locks16 = ZscriptLockSet.noneLocked(false);
        LockCondition.createFromBits(1, 3, 6).apply(null, locks16);
        assertThat(locks16.isAllSet()).isFalse();
        assertThat(locks16.asStringUtf8()).isEqualTo("%4a");
        locks16.setLock(4);
        assertThat(locks16.asStringUtf8()).isEqualTo("%5a");

        final ZscriptLockSet locks32 = ZscriptLockSet.noneLocked(true);
        LockCondition.createFromBits(1, 3, 6, 25, 26).apply(null, locks32);
        assertThat(locks32.isAllSet()).isFalse();
        assertThat(locks32.asStringUtf8()).isEqualTo("%600004a");
        locks32.setLock(4);
        locks32.setLock(18);
        assertThat(locks32.asStringUtf8()).isEqualTo("%604005a");
    }

    @Test
    public void shouldParseNormalLocks() {
        final ZscriptLockSet locks16 = ZscriptLockSet.parse(tokenize(byteStringUtf8("%52c"), true)
                .getTokenReader().getFirstReadToken(), false);
        assertThat(locks16.isAllSet()).isFalse();
        assertThat(locks16.asStringUtf8()).isEqualTo("%52c0");
    }

    @Test
    public void shouldParseEmptyLocks() {
        final ZscriptLockSet locks16 = ZscriptLockSet.parse(tokenize(byteStringUtf8("%"), true)
                .getTokenReader().getFirstReadToken(), false);
        assertThat(locks16.isAllSet()).isFalse();
        assertThat(locks16.asStringUtf8()).isEqualTo("%");
    }

    @Test
    public void shouldRejectParsingNoLocks() {
        final TokenBuffer.TokenReader.ReadToken token = tokenize(byteStringUtf8("A1"), true).getTokenReader().getFirstReadToken();
        assertThatThrownBy(() -> ZscriptLockSet.parse(token, false))
                .isInstanceOf(IllegalArgumentException.class).hasMessageContaining("Cannot parse lock set");
    }

    @Test
    public void shouldRejectParsingLongLockField() {
        final TokenBuffer.TokenReader.ReadToken token16 = tokenize(byteStringUtf8("%12345"), true).getTokenReader().getFirstReadToken();
        assertThatThrownBy(() -> ZscriptLockSet.parse(token16, false))
                .isInstanceOf(IllegalArgumentException.class).hasMessageContaining("Lock field too long");

        final TokenBuffer.TokenReader.ReadToken token32 = tokenize(byteStringUtf8("%12345678a"), true).getTokenReader().getFirstReadToken();
        assertThatThrownBy(() -> ZscriptLockSet.parse(token32, true))
                .isInstanceOf(IllegalArgumentException.class).hasMessageContaining("Lock field too long");
    }
}
