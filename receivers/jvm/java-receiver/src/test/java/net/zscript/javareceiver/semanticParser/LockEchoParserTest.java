package net.zscript.javareceiver.semanticParser;

import java.io.IOException;
import java.util.List;
import java.util.stream.Stream;

import static org.assertj.core.api.Assertions.assertThat;
import static org.junit.jupiter.params.provider.Arguments.of;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import net.zscript.javareceiver.core.LockSet;
import net.zscript.javareceiver.core.StringWriterOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.semanticParser.SemanticAction.ActionType;
import net.zscript.javareceiver.semanticParser.SemanticParser.State;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.TokenRingBuffer;
import net.zscript.tokenizer.Tokenizer;

class LockEchoParserTest {
    private final TokenBuffer buffer    = TokenRingBuffer.createBufferWithCapacity(256);
    private final Tokenizer   tokenizer = new Tokenizer(buffer.getTokenWriter(), 2);

    private final SemanticParser parser = new SemanticParser(buffer.getTokenReader(), new ExecutionActionFactory());

    private final Zscript zscript = new Zscript();

    private StringWriterOutStream outStream;
    private ParserActionTester    parserActionTester;

    @BeforeEach
    void setup() throws IOException {
        zscript.addModule(new ZscriptCoreModule());
        outStream = new StringWriterOutStream();
        parserActionTester = new ParserActionTester(zscript, buffer, tokenizer, parser, outStream);
    }

    @Test
    public void shouldEchoSequenceLevelValue() {
        parserActionTester.parseSnippet("_1234Z1\n", "!_1234S\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @ParameterizedTest
    @MethodSource
    public void shouldEchoSequenceLevelLineNumber(final String text, String finalOutput) {
        parserActionTester.parseSnippet(text, finalOutput);
        assertThat(outStream.isOpen()).isFalse();
    }

    private static Stream<Arguments> shouldEchoSequenceLevelLineNumber() {
        return Stream.of(
                of("_1234Z1\n", "!_1234S\n"),
                of("_\n", "!_\n"),
                of("_1\n_2\n", "!_1\n!_2\n"),
                of("__\n", "!_S31\n"),
                of("_12345\n", "!S20\n"));
    }

    @ParameterizedTest
    @MethodSource
    public void shouldHandleLocks(final String text, String finalOutput) {
        parserActionTester.parseSnippet(text, finalOutput);
        assertThat(outStream.isOpen()).isFalse();
    }

    private static Stream<Arguments> shouldHandleLocks() {
        return Stream.of(
                of("%1234Z1\n", "!S\n"),
                of("%\n", "!\n"),
                of("%ffff\n", "!\n"),
                of("%ffff\n%ffff\n", "!\n!\n"),
                of("%1 Z1A \n  %1 Z1B\n", "!AS\n!BS\n"),
                of("%1 %2 Z1A \n", "!S30\n"),
                of("_1234%f Z1A\n", "!_1234AS\n"));
    }

    @Test
    public void shouldApplyLocks() {
        "%16 Z1A &".chars().forEachOrdered(c -> tokenizer.accept((byte) c));
        SemanticAction action = parser.getAction();
        assertThat(action.getType()).isEqualTo(ActionType.RUN_FIRST_COMMAND);

        assertThat(zscript.canLock(LockSet.allLocked())).isTrue();
        action.lock(zscript);
        assertThat(zscript.canLock(LockSet.allLocked())).isFalse();
        assertThat(zscript.canLock(LockSet.from(List.of((byte) 0x1).iterator()))).isTrue();
        assertThat(zscript.canLock(LockSet.from(List.of((byte) 0x2).iterator()))).isFalse();
        assertThat(zscript.canLock(LockSet.from(List.of((byte) 0x4).iterator()))).isFalse();
        assertThat(zscript.canLock(LockSet.from(List.of((byte) 0x8).iterator()))).isTrue();
        assertThat(zscript.canLock(LockSet.from(List.of((byte) 0x10).iterator()))).isFalse();
        assertThat(zscript.canLock(LockSet.from(List.of((byte) 0x20).iterator()))).isTrue();

        action.performAction(zscript, outStream);
        assertThat(outStream.toString()).isEqualTo("!AS");

        parserActionTester.parseSnippet("Z1B", ActionType.WAIT_FOR_TOKENS, State.COMMAND_COMPLETE_NEEDS_TOKENS, "!AS");

        assertThat(zscript.canLock(LockSet.from(List.of((byte) 0x2).iterator()))).isFalse();

        parserActionTester.parseSnippet("\n", ActionType.RUN_COMMAND, State.COMMAND_COMPLETE, "!AS&BS");
        assertThat(zscript.canLock(LockSet.from(List.of((byte) 0x2).iterator()))).isFalse();

        parserActionTester.parseSnippet("", ActionType.END_SEQUENCE, State.PRESEQUENCE, "!AS&BS\n");
        assertThat(zscript.canLock(LockSet.allLocked())).isTrue();
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!AS&BS\n");
    }

}
