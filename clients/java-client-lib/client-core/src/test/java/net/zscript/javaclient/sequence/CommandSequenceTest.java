package net.zscript.javaclient.sequence;

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
    public void shouldCreateWithEchoValue() {
        CommandExecutionPath path = CommandExecutionPath.parse(tokenize(byteStringUtf8("A & B"), true)
                .getTokenReader().getFirstReadToken());

        final CommandSequence cmdSeq = CommandSequence.from(path, 0x1b);
        assertThat(cmdSeq.hasEchoField()).isTrue();
        assertThat(cmdSeq.getEchoValue()).isEqualTo(0x1b);
        assertThat(cmdSeq.asStringUtf8()).isEqualTo(">1bA&B");
        assertThat(cmdSeq.getExecutionPath().asStringUtf8()).isEqualTo("A&B");
    }

    @Test
    public void shouldCreateWithoutEchoValue() {
        CommandExecutionPath path = CommandExecutionPath.parse(tokenize(byteStringUtf8("A"), true)
                .getTokenReader().getFirstReadToken());

        final CommandSequence cmdSeq = CommandSequence.from(path, 0x100);
        assertThat(cmdSeq.hasEchoField()).isTrue();
        assertThat(cmdSeq.getEchoValue()).isEqualTo(0x100);
        assertThat(cmdSeq.asStringUtf8()).isEqualTo(">100A");
        assertThat(cmdSeq.getExecutionPath().asStringUtf8()).isEqualTo("A");
    }

    @Test
    public void shouldParseEmptySequence() {
        final CommandSequence cmdSeq = parseToSequence("");
        assertThat(cmdSeq.hasEchoField()).isFalse();
        assertThat(cmdSeq.getEchoValue()).isEqualTo(-1);
        assertThat(cmdSeq.asStringUtf8()).isEmpty();
        assertThat(cmdSeq.getExecutionPath().asStringUtf8()).isEmpty();

    }

    @Test
    public void shouldParseEchoValue() {
        final CommandSequence cmdSeq = parseToSequence(" >a AS");
        assertThat(cmdSeq.hasEchoField()).isTrue();
        assertThat(cmdSeq.getEchoValue()).isEqualTo(0x0a);
        assertThat(cmdSeq.asStringUtf8()).isEqualTo(">aAS");
        assertThat(cmdSeq.getExecutionPath().asStringUtf8()).isEqualTo("AS");
    }

    @Test
    public void shouldFailWithResponseMarker() {
        assertThatThrownBy(() -> parseToSequence("!"))
                .isInstanceOf(ZscriptParseException.class)
                .hasMessageContaining("Invalid numeric key");
    }

    @Test
    public void shouldFailWithMultipleEchoValues() {
        assertThatThrownBy(() -> parseToSequence(">2a >1 %3 A1"))
                .isInstanceOf(ZscriptParseException.class)
                .hasMessageContaining("two echo fields");
    }

    @Test
    public void shouldImplementToString() {
        assertThat(parseToSequence(" >2 A")).hasToString("CommandSequence:{'>2A'}");
    }

    private static CommandSequence parseToSequence(String s) {
        return CommandSequence.parse(ZscriptModel.standardModel(),
                tokenize(byteStringUtf8(s), true).getTokenReader().getFirstReadToken());
    }
}
