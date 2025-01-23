package net.zscript.javaclient.sequence;

import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;

class LockSetTest {
    @Test
    public void shouldSetAllLocks() {
        final LockSet locks16 = LockSet.allLocked(false);
        assertThat(locks16.isAllSet()).isTrue();
        assertThat(locks16.asStringUtf8()).isEmpty();

        final LockSet locks32 = LockSet.allLocked(true);
        assertThat(locks32.isAllSet()).isTrue();
        assertThat(locks32.asStringUtf8()).isEmpty();
    }

    @Test
    public void shouldSetNoLocks() {
        final LockSet locks16 = LockSet.noneLocked(false);
        assertThat(locks16.isAllSet()).isFalse();
        assertThat(locks16.asStringUtf8()).isEqualTo("%");

        final LockSet locks32 = LockSet.noneLocked(true);
        assertThat(locks32.isAllSet()).isFalse();
        assertThat(locks16.asStringUtf8()).isEqualTo("%");
    }

    @Test
    public void shouldSetSpecificLocks() {
        final LockSet locks16 = LockSet.noneLocked(false);
        LockCondition.createFromBits(1, 3, 6).apply(null, locks16);
        assertThat(locks16.isAllSet()).isFalse();
        assertThat(locks16.asStringUtf8()).isEqualTo("%4a");
        locks16.setLock(4);
        assertThat(locks16.asStringUtf8()).isEqualTo("%5a");

        final LockSet locks32 = LockSet.noneLocked(true);
        LockCondition.createFromBits(1, 3, 6, 25, 26).apply(null, locks32);
        assertThat(locks32.isAllSet()).isFalse();
        assertThat(locks32.asStringUtf8()).isEqualTo("%600004a");
        locks32.setLock(4);
        locks32.setLock(18);
        assertThat(locks32.asStringUtf8()).isEqualTo("%604005a");
    }

    @Test
    public void shouldParseNormalLocks() {
        final LockSet locks16 = LockSet.parse(parse("%52c"), false);
        assertThat(locks16.isAllSet()).isFalse();
        assertThat(locks16.asStringUtf8()).isEqualTo("%52c0");
    }

    @Test
    public void shouldParseEmptyLocks() {
        final LockSet locks16 = LockSet.parse(parse("%"), false);
        assertThat(locks16.isAllSet()).isFalse();
        assertThat(locks16.asStringUtf8()).isEqualTo("%");
    }

    @Test
    public void shouldRejectParsingNoLocks() {
        final ReadToken token = parse("A1");
        assertThatThrownBy(() -> LockSet.parse(token, false))
                .isInstanceOf(IllegalArgumentException.class).hasMessageContaining("Cannot parse lock set");
    }

    @Test
    public void shouldRejectParsingLongLockField() {
        final ReadToken token16 = parse("%12345");
        assertThatThrownBy(() -> LockSet.parse(token16, false))
                .isInstanceOf(IllegalArgumentException.class).hasMessageContaining("Lock field too long");

        final ReadToken token32 = parse("%12345678a");
        assertThatThrownBy(() -> LockSet.parse(token32, true))
                .isInstanceOf(IllegalArgumentException.class).hasMessageContaining("Lock field too long");
    }

    @Test
    public void shouldImplementToString() {
        final LockSet locks16 = LockSet.parse(parse("%52c"), false);
        assertThat(locks16).hasToString("LockSet:{'%52c0'}");
    }

    private static ReadToken parse(String s) {
        return tokenize(byteStringUtf8(s), true).getTokenReader().getFirstReadToken();
    }
}
