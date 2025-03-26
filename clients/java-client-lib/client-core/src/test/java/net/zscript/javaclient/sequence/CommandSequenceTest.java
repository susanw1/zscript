package net.zscript.javaclient.sequence;

import java.util.Collection;
import java.util.List;

import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.ZscriptParseException;
import net.zscript.javaclient.commandpaths.CommandExecutionPath;
import net.zscript.model.ZscriptModel;

class CommandSequenceTest {
    @Test
    public void shouldCreateWithLocksAndEchoValue() {
        CommandExecutionPath path = CommandExecutionPath.parse(tokenize(byteStringUtf8("A & B"), true)
                .getTokenReader().getFirstReadToken());
        Collection<LockCondition> lockConditions = List.of(LockCondition.createFromBits(0, 4, 5, 11)); // equals 0x831

        final CommandSequence cmdSeq = CommandSequence.from(path, 0x1b, false, lockConditions);
        assertThat(cmdSeq.hasLockField()).isTrue();
        assertThat(cmdSeq.getLocks().asStringUtf8()).isEqualTo("%831");
        assertThat(cmdSeq.hasEchoField()).isTrue();
        assertThat(cmdSeq.getEchoValue()).isEqualTo(0x1b);
        assertThat(cmdSeq.asStringUtf8()).isEqualTo("%831>1bA&B");
        assertThat(cmdSeq.getExecutionPath().asStringUtf8()).isEqualTo("A&B");
    }

    @Test
    public void shouldCreateWithoutLocksOrEchoValue() {
        CommandExecutionPath path = CommandExecutionPath.parse(tokenize(byteStringUtf8("A"), true)
                .getTokenReader().getFirstReadToken());

        final CommandSequence cmdSeq = CommandSequence.from(path, 0x100);
        assertThat(cmdSeq.hasLockField()).isTrue();
        assertThat(cmdSeq.getLocks().isAllSet()).isTrue();
        assertThat(cmdSeq.getLocks().asStringUtf8()).isEmpty();
        assertThat(cmdSeq.hasEchoField()).isTrue();
        assertThat(cmdSeq.getEchoValue()).isEqualTo(0x100);
        assertThat(cmdSeq.asStringUtf8()).isEqualTo(">100A");
        assertThat(cmdSeq.getExecutionPath().asStringUtf8()).isEqualTo("A");
    }

    @Test
    public void shouldParseEmptySequence() {
        final CommandSequence cmdSeq = parse("");
        assertThat(cmdSeq.hasLockField()).isFalse();
        assertThat(cmdSeq.hasEchoField()).isFalse();
        assertThat(cmdSeq.getEchoValue()).isEqualTo(-1);
        assertThat(cmdSeq.asStringUtf8()).isEmpty();
        assertThat(cmdSeq.getExecutionPath().asStringUtf8()).isEmpty();

    }

    @Test
    public void shouldParseLocksAndEchoValue() {
        final CommandSequence cmdSeq = parse(" >a %12a AS");
        assertThat(cmdSeq.hasLockField()).isTrue();
        assertThat(cmdSeq.getLocks().asStringUtf8()).isEqualTo("%12a0");
        assertThat(cmdSeq.hasEchoField()).isTrue();
        assertThat(cmdSeq.getEchoValue()).isEqualTo(0x0a);
        assertThat(cmdSeq.asStringUtf8()).isEqualTo("%12a0>aAS");
        assertThat(cmdSeq.getExecutionPath().asStringUtf8()).isEqualTo("AS");
    }

    @Test
    public void shouldParseFullLocksAndMapThemAsDefault() {
        final CommandSequence cmdSeq = parse(" %ffff AS");
        assertThat(cmdSeq.hasLockField()).isTrue();
        assertThat(cmdSeq.getLocks().asStringUtf8()).isEqualTo("");
        assertThat(cmdSeq.hasEchoField()).isFalse();
        assertThat(cmdSeq.asStringUtf8()).isEqualTo("AS");
        assertThat(cmdSeq.getExecutionPath().asStringUtf8()).isEqualTo("AS");
    }

    @Test
    public void shouldFailWithResponseMarker() {
        assertThatThrownBy(() -> parse("!"))
                .isInstanceOf(ZscriptParseException.class)
                .hasMessageContaining("Invalid numeric key");
    }

    @Test
    public void shouldFailWithMultipleLockValues() {
        assertThatThrownBy(() -> parse("%2a >1 %3"))
                .isInstanceOf(ZscriptParseException.class)
                .hasMessageContaining("two lock fields");
    }

    @Test
    public void shouldFailWithMultipleEchoValues() {
        assertThatThrownBy(() -> parse(">2a >1 %3 A1"))
                .isInstanceOf(ZscriptParseException.class)
                .hasMessageContaining("two echo fields");
    }

    @Test
    public void shouldImplementToString() {
        assertThat(parse(" >2 A")).hasToString("CommandSequence:{'>2A'}");
    }

    private static CommandSequence parse(String s) {
        return CommandSequence.parse(ZscriptModel.standardModel(),
                tokenize(byteStringUtf8(s), true).getTokenReader().getFirstReadToken(), false);
    }
}
