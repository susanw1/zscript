package net.zscript.javareceiver.semanticParser;

import static org.assertj.core.api.Assertions.assertThat;
import static org.junit.jupiter.params.provider.Arguments.of;

import java.io.IOException;
import java.util.List;
import java.util.stream.Stream;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import net.zscript.javareceiver.core.StringWriterOutStream;
import net.zscript.javareceiver.core.Zcode;
import net.zscript.javareceiver.core.ZcodeLockSet;
import net.zscript.javareceiver.modules.core.ZcodeCoreModule;
import net.zscript.javareceiver.semanticParser.ExecutionActionFactory;
import net.zscript.javareceiver.semanticParser.SemanticParser;
import net.zscript.javareceiver.semanticParser.ZcodeSemanticAction;
import net.zscript.javareceiver.semanticParser.SemanticParser.State;
import net.zscript.javareceiver.semanticParser.ZcodeSemanticAction.ActionType;
import net.zscript.javareceiver.tokenizer.ZcodeTokenBuffer;
import net.zscript.javareceiver.tokenizer.ZcodeTokenRingBuffer;
import net.zscript.javareceiver.tokenizer.ZcodeTokenizer;

class LockEchoParserTest {
    private final ZcodeTokenBuffer buffer    = ZcodeTokenRingBuffer.createBufferWithCapacity(256);
    private final ZcodeTokenizer   tokenizer = new ZcodeTokenizer(buffer.getTokenWriter(), 2);

    private final SemanticParser parser = new SemanticParser(buffer.getTokenReader(), new ExecutionActionFactory());

    private final Zcode zcode = new Zcode();

    private StringWriterOutStream outStream;
    private ParserActionTester    parserActionTester;

    @BeforeEach
    void setup() throws IOException {
        zcode.addModule(new ZcodeCoreModule());
        outStream = new StringWriterOutStream();
        parserActionTester = new ParserActionTester(zcode, buffer, tokenizer, parser, outStream);
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
                of("__\n", "!10_S31\n"),
                of("_12345\n", "!10S20\n"));
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
                of("%1 %2 Z1A \n", "!10S30\n"),
                of("_1234%f Z1A\n", "!_1234AS\n"));
    }

    @Test
    public void shouldApplyLocks() {
        "%16 Z1A &".chars().forEachOrdered(c -> tokenizer.accept((byte) c));
        ZcodeSemanticAction action = parser.getAction();
        assertThat(action.getType()).isEqualTo(ActionType.RUN_FIRST_COMMAND);

        assertThat(zcode.canLock(ZcodeLockSet.allLocked())).isTrue();
        action.lock(zcode);
        assertThat(zcode.canLock(ZcodeLockSet.allLocked())).isFalse();
        assertThat(zcode.canLock(ZcodeLockSet.from(List.of((byte) 0x1).iterator()))).isTrue();
        assertThat(zcode.canLock(ZcodeLockSet.from(List.of((byte) 0x2).iterator()))).isFalse();
        assertThat(zcode.canLock(ZcodeLockSet.from(List.of((byte) 0x4).iterator()))).isFalse();
        assertThat(zcode.canLock(ZcodeLockSet.from(List.of((byte) 0x8).iterator()))).isTrue();
        assertThat(zcode.canLock(ZcodeLockSet.from(List.of((byte) 0x10).iterator()))).isFalse();
        assertThat(zcode.canLock(ZcodeLockSet.from(List.of((byte) 0x20).iterator()))).isTrue();

        action.performAction(zcode, outStream);
        assertThat(outStream.toString()).isEqualTo("!AS");

        parserActionTester.parseSnippet("Z1B", ActionType.WAIT_FOR_TOKENS, State.COMMAND_COMPLETE_NEEDS_TOKENS, "!AS");

        assertThat(zcode.canLock(ZcodeLockSet.from(List.of((byte) 0x2).iterator()))).isFalse();

        parserActionTester.parseSnippet("\n", ActionType.RUN_COMMAND, State.COMMAND_COMPLETE, "!AS&BS");
        assertThat(zcode.canLock(ZcodeLockSet.from(List.of((byte) 0x2).iterator()))).isFalse();

        parserActionTester.parseSnippet("", ActionType.END_SEQUENCE, State.PRESEQUENCE, "!AS&BS\n");
        assertThat(zcode.canLock(ZcodeLockSet.allLocked())).isTrue();
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!AS&BS\n");
    }

    @Test
    public void shouldGiveErrorForLocksWithComment() {
        parserActionTester.parseSnippet("%1#Z1\n", ActionType.ERROR, State.PRESEQUENCE, "!10S32\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    public void shouldGiveErrorForLineNumberWithComment() {
        parserActionTester.parseSnippet("_ff#Z1\n", ActionType.ERROR, State.PRESEQUENCE, "!10_ffS32\n");
        assertThat(outStream.isOpen()).isFalse();
    }

}
