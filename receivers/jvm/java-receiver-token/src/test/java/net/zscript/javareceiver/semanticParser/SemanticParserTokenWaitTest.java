package net.zscript.javareceiver.semanticParser;

import static org.assertj.core.api.Assertions.assertThat;

import java.io.IOException;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import net.zscript.javareceiver.core.StringWriterOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.semanticParser.SemanticAction.ActionType;
import net.zscript.javareceiver.semanticParser.SemanticParser.State;
import net.zscript.javareceiver.tokenizer.TokenBuffer;
import net.zscript.javareceiver.tokenizer.TokenRingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;

class SemanticParserTokenWaitTest {
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
    public void shouldWaitWithAndThen() {
        parserActionTester.parseSnippet("Z1A", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "");
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("&", ActionType.RUN_FIRST_COMMAND, State.COMMAND_COMPLETE, "!AS");
        assertThat(outStream.isOpen()).isTrue();

        parserActionTester.parseSnippet("Z1B", ActionType.WAIT_FOR_TOKENS, State.COMMAND_COMPLETE_NEEDS_TOKENS, "!AS");
        assertThat(outStream.isOpen()).isTrue();

        parserActionTester.parseSnippet("\n", ActionType.RUN_COMMAND, State.COMMAND_COMPLETE, "!AS&BS");
        assertThat(outStream.isOpen()).isTrue();

        parserActionTester.parseSnippet("", ActionType.END_SEQUENCE, State.PRESEQUENCE, "!AS&BS\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    public void shouldWaitWithOrElse() {
        parserActionTester.parseSnippet("Z1A", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "");
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("|", ActionType.RUN_FIRST_COMMAND, State.COMMAND_COMPLETE, "!AS");
        assertThat(outStream.isOpen()).isTrue();

        parserActionTester.parseSnippet("Z1B", ActionType.WAIT_FOR_TOKENS, State.COMMAND_SKIP, "!AS");
        assertThat(outStream.isOpen()).isTrue();

        parserActionTester.parseSnippet("\n", ActionType.END_SEQUENCE, State.PRESEQUENCE, "!AS\n");
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!AS\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    public void shouldWaitWithParens() {
        // Z1A & (Z1B S1 | Z1C) | Z1D
        parserActionTester.parseSnippet("Z1A", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "");
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("&", ActionType.RUN_FIRST_COMMAND, State.COMMAND_COMPLETE, "!AS");
        assertThat(outStream.isOpen()).isTrue();

        parserActionTester.parseSnippet("(", ActionType.RUN_COMMAND, State.COMMAND_COMPLETE, "!AS&");
        assertThat(outStream.isOpen()).isTrue();

        parserActionTester.parseSnippet("Z1B S1", ActionType.WAIT_FOR_TOKENS, State.COMMAND_COMPLETE_NEEDS_TOKENS, "!AS&");
        assertThat(outStream.isOpen()).isTrue();

        parserActionTester.parseSnippet("|", ActionType.RUN_COMMAND, State.COMMAND_FAILED, "!AS&(BS1");
        assertThat(outStream.isOpen()).isTrue();

        parserActionTester.parseSnippet("Z1C", ActionType.WAIT_FOR_TOKENS, State.COMMAND_COMPLETE_NEEDS_TOKENS, "!AS&(BS1");
        assertThat(outStream.isOpen()).isTrue();

        parserActionTester.parseSnippet(")", ActionType.RUN_COMMAND, State.COMMAND_COMPLETE, "!AS&(BS1|CS");
        assertThat(outStream.isOpen()).isTrue();

        parserActionTester.parseSnippet("|", ActionType.RUN_COMMAND, State.COMMAND_COMPLETE, "!AS&(BS1|CS)");
        assertThat(outStream.isOpen()).isTrue();
        parserActionTester.parseSnippet("Z1D", ActionType.WAIT_FOR_TOKENS, State.COMMAND_SKIP, "!AS&(BS1|CS)");
        assertThat(outStream.isOpen()).isTrue();

        parserActionTester.parseSnippet("\n", ActionType.END_SEQUENCE, State.PRESEQUENCE, "!AS&(BS1|CS)\n");
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!AS&(BS1|CS)\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    public void shouldWaitWithMultipleCommands() {
        // Z1A\nZ1B\n - with lots of token-draining, too.
        parserActionTester.parseSnippet("Z1A", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "");
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "");
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("\n", ActionType.RUN_FIRST_COMMAND, State.COMMAND_COMPLETE, "!AS");
        assertThat(outStream.isOpen()).isTrue();

        parserActionTester.parseSnippet("", ActionType.END_SEQUENCE, State.PRESEQUENCE, "!AS\n");
        assertThat(outStream.isOpen()).isFalse();
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!AS\n");
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("Z1B", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!AS\n");
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("\n", ActionType.RUN_FIRST_COMMAND, State.COMMAND_COMPLETE, "!AS\n!BS");
        assertThat(outStream.isOpen()).isTrue();

        parserActionTester.parseSnippet("", ActionType.END_SEQUENCE, State.PRESEQUENCE, "!AS\n!BS\n");
        assertThat(outStream.isOpen()).isFalse();
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!AS\n!BS\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    public void shouldWaitWithErrors() {
        // Z1A+a & Z1B\n Z1C\n
        parserActionTester.parseSnippet("Z1A+a", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "");
        assertThat(outStream.isOpen()).isFalse();
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "");
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("&", ActionType.ERROR, State.PRESEQUENCE, "!S21\n");
        assertThat(outStream.isOpen()).isFalse();
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!S21\n");
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("Z1B", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!S21\n");
        assertThat(outStream.isOpen()).isFalse();
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!S21\n");
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("\n", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!S21\n");
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("Z1C", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!S21\n");
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("\n", ActionType.RUN_FIRST_COMMAND, State.COMMAND_COMPLETE, "!S21\n!CS");
        assertThat(outStream.isOpen()).isTrue();
        parserActionTester.parseSnippet("", ActionType.END_SEQUENCE, State.PRESEQUENCE, "!S21\n!CS\n");
        assertThat(outStream.isOpen()).isFalse();
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!S21\n!CS\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    public void shouldWaitWithOverrunError() {
        // Z1A & Z1B & Z1C *\n Z1D\n (overun shown as '*')
        parserActionTester.parseSnippet("Z1A ", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "");
        assertThat(outStream.isOpen()).isFalse();
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "");
        assertThat(outStream.isOpen()).isFalse();

        "& Z1B & Z1C".chars().forEachOrdered(c -> tokenizer.accept((byte) c));
        tokenizer.dataLost();

        parserActionTester.parseSnippet("", ActionType.ERROR, State.PRESEQUENCE, "!S10\n");
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("\n", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!S10\n");
        assertThat(outStream.isOpen()).isFalse();
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!S10\n");
        assertThat(outStream.isOpen()).isFalse();

        parserActionTester.parseSnippet("Z1D\n", ActionType.RUN_FIRST_COMMAND, State.COMMAND_COMPLETE, "!S10\n!DS");
        assertThat(outStream.isOpen()).isTrue();
        parserActionTester.parseSnippet("", ActionType.END_SEQUENCE, State.PRESEQUENCE, "!S10\n!DS\n");
        assertThat(outStream.isOpen()).isFalse();
        parserActionTester.parseSnippet("", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!S10\n!DS\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    public void shouldGiveErrorForFailedComment() {
        parserActionTester.parseSnippet("#pq", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "");
        tokenizer.dataLost();
        parserActionTester.parseSnippet("xyz", ActionType.ERROR, State.PRESEQUENCE, "!S10\n");
        parserActionTester.parseSnippet("\n", ActionType.WAIT_FOR_TOKENS, State.PRESEQUENCE, "!S10\n");
        parserActionTester.parseSnippet("Z1\n", ActionType.RUN_FIRST_COMMAND, State.COMMAND_COMPLETE, "!S10\n!S");
        parserActionTester.parseSnippet("", ActionType.END_SEQUENCE, State.PRESEQUENCE, "!S10\n!S\n");

        assertThat(outStream.isOpen()).isFalse();
    }

}
