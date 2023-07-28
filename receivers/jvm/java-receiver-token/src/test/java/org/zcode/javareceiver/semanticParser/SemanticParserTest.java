package org.zcode.javareceiver.semanticParser;

import static org.assertj.core.api.Assertions.assertThat;
import static org.junit.jupiter.params.provider.Arguments.of;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.CLOSE_PAREN;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.END_SEQUENCE;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.RUN_COMMAND;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.RUN_FIRST_COMMAND;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.WAIT_FOR_TOKENS;

import java.io.IOException;
import java.util.List;
import java.util.stream.Stream;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;
import org.zcode.javareceiver.core.StringWriterOutStream;
import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.execution.ZcodeAction;
import org.zcode.javareceiver.modules.core.ZcodeCoreModule;
import org.zcode.javareceiver.semanticParser.SemanticParser.State;
import org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenRingBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

class SemanticParserTest {
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
    void shouldStartNoActionWithEmptyTokens() {
        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);
        final ZcodeSemanticAction a = parser.getAction();
        assertThat(a.getType()).isEqualTo(WAIT_FOR_TOKENS);
        assertThat(outStream.getString()).isEqualTo("");
        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);
    }

    @Test
    void shouldProduceActionForCommand() {
        "Z0\n".chars().forEachOrdered(c -> tokenizer.accept((byte) c));

        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);
        final ZcodeSemanticAction a1 = parser.getAction();
        assertThat(a1.getType()).isEqualTo(RUN_FIRST_COMMAND);
        a1.performAction(zcode, outStream);
        assertThat(outStream.getStringAndReset()).isEqualTo("!V1C3007M1S");
        assertThat(parser.getState()).isEqualTo(State.COMMAND_COMPLETE);

        final ZcodeSemanticAction a2 = parser.getAction();
        assertThat(a2.getType()).isEqualTo(END_SEQUENCE);
        a2.performAction(zcode, outStream);
        assertThat(outStream.getStringAndReset()).isEqualTo("\n");
        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);

        final ZcodeSemanticAction a3 = parser.getAction();
        assertThat(a3.getType()).isEqualTo(WAIT_FOR_TOKENS);
        assertThat(outStream.getString()).isEqualTo("");
        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);

        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    void shouldProduceActionForTwoCommands() {
        "Z1A & Z1B\n".chars().forEachOrdered(c -> tokenizer.accept((byte) c));

        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);
        final ZcodeSemanticAction a1 = parser.getAction();
        assertThat(a1.getType()).isEqualTo(RUN_FIRST_COMMAND);
        a1.performAction(zcode, outStream);
        assertThat(outStream.getStringAndReset()).isEqualTo("!AS");
        assertThat(parser.getState()).isEqualTo(State.COMMAND_COMPLETE);

        final ZcodeSemanticAction a2 = parser.getAction();
        assertThat(a2.getType()).isEqualTo(RUN_COMMAND);
//        assertThat(a2.getInfo()).isEqualTo(ZcodeTokenizer.CMD_END_ANDTHEN);
        a2.performAction(zcode, outStream);
        assertThat(outStream.getStringAndReset()).isEqualTo("&BS");
        assertThat(parser.getState()).isEqualTo(State.COMMAND_COMPLETE);

        final ZcodeSemanticAction a3 = parser.getAction();
        assertThat(a3.getType()).isEqualTo(END_SEQUENCE);
        a3.performAction(zcode, outStream);
        assertThat(outStream.getStringAndReset()).isEqualTo("\n");
        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);

        final ZcodeSemanticAction a4 = parser.getAction();
        assertThat(a4.getType()).isEqualTo(WAIT_FOR_TOKENS);
        assertThat(outStream.getString()).isEqualTo("");
        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);

        assertThat(outStream.isOpen()).isFalse();
    }

    private void parseSnippet(final String text, final List<ActionType> actionTypes) {
        // feed all chars into tokens/buffer
        text.chars().forEachOrdered(c -> tokenizer.accept((byte) c));

        buffer.getTokenReader().iterator().forEach(t -> System.out.print(t + " "));
        System.out.println();

        actionTypes.forEach(t -> {
            System.out.println("Expecting actionType=" + t);
            ZcodeAction a1 = parser.getAction();

            System.out.println("  Received action: actionType=" + a1 + "; state=" + parser.getState());
            assertThat(((ZcodeSemanticAction) a1).getType()).isEqualTo(t);
            a1.performAction(zcode, outStream);

            System.out.println("   - After execute action: outStream=" + outStream.getString().replaceAll("\\n", "\\\\n") + "; state=" + parser.getState());
        });
    }

    @ParameterizedTest
    @MethodSource
    public void shouldProduceActionsForSingleCommands(final String text, final List<ActionType> actionTypes, String finalOutput) {
        parserActionTester.parseSnippet(text, actionTypes);
        assertThat(outStream.getString()).isEqualTo(finalOutput);
        assertThat(outStream.isOpen()).isFalse();
    }

    private static Stream<Arguments> shouldProduceActionsForSingleCommands() {
        return Stream.of(
                of("Z0\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!V1C3007M1S\n"),
                of("Z1A\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS\n"),
                of("Z1A S1\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1\n"),
                of("Z1A S10\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"));
    }

    @ParameterizedTest
    @MethodSource
    public void shouldProduceActionsForLogicalCommandSeries(final String text, final List<ActionType> actionTypes, String finalOutput) {
        parserActionTester.parseSnippet(text, actionTypes);
        assertThat(outStream.getString()).isEqualTo(finalOutput);
        assertThat(outStream.isOpen()).isFalse();
    }

    private static Stream<Arguments> shouldProduceActionsForLogicalCommandSeries() {
        return Stream.of(
                of("Z1A & Z1B\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS&BS\n"),
                of("Z1A | Z1B\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS\n"),
                of("Z1A & Z1B & Z1C\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS&BS&CS\n"),
                of("Z1A | Z1B | Z1C\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS\n"),
                of("Z1A & Z1B | Z1C\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS&BS\n"),
                of("Z1A | Z1B & Z1C\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS\n"));
    }

    @ParameterizedTest
    @MethodSource
    public void shouldProduceActionsForLogicalCommandSeriesWithFailures(final String text, final List<ActionType> actionTypes, String finalOutput) {
        parserActionTester.parseSnippet(text, actionTypes);
        assertThat(outStream.getString()).isEqualTo(finalOutput);
        assertThat(outStream.isOpen()).isFalse();
    }

    private static Stream<Arguments> shouldProduceActionsForLogicalCommandSeriesWithFailures() {
        return Stream.of(
                // 2 cmds with ANDTHEN
                of("Z1A S1 & Z1B\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1\n"),
                of("Z1A S1 & Z1B S2\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1\n"),

                // 2 cmds with ORELSE
                of("Z1A S1 | Z1B\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|BS\n"),
                of("Z1A S1 | Z1B S2\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|BS2\n"),

                // 3 cmds with ANDTHEN
                of("Z1A S1 & Z1B & Z1C\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1\n"),
                of("Z1A & Z1B S1 & Z1C\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS&BS1\n"),
                of("Z1A S1 & Z1B S1 & Z1C\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1\n"),
                of("Z1A S1 & Z1B S1 & Z1C S1\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1\n"),

                // 3 cmds with ORELSE
                of("Z1A S1 | Z1B | Z1C\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|BS\n"),
                of("Z1A | Z1B S1 | Z1C\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS\n"),
                of("Z1A S1 | Z1B S1 | Z1C\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|BS1|CS\n"),
                of("Z1A S1 | Z1B S1 | Z1C S1\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|BS1|CS1\n"),

                // 3 cmds with ANDTHEN/ORELSE combo
                of("Z1A S1 & Z1B | Z1C\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|CS\n"),
                of("Z1A & Z1B S1 | Z1C\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS&BS1|CS\n"),
                of("Z1A S1 | Z1B S1 & Z1C\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|BS1\n"),
                of("Z1A S1 | Z1B & Z1C S2\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|BS&CS2\n"));
    }

    @ParameterizedTest
    @MethodSource
    public void shouldProduceActionsForLogicalCommandSeriesWithErrors(final String text, final List<ActionType> actionTypes, String finalOutput) {
        parserActionTester.parseSnippet(text, actionTypes);
        assertThat(outStream.getString()).isEqualTo(finalOutput);
        assertThat(outStream.isOpen()).isFalse();
    }

    private static Stream<Arguments> shouldProduceActionsForLogicalCommandSeriesWithErrors() {
        return Stream.of(
                of("Z1A S10\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"),

                // 2 cmds with ANDTHEN
                of("Z1A S10 & Z1B\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"),
                of("Z1A & Z1B S20\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS&BS20\n"),
                of("Z1A S10 & Z1B S20\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"),

                // 2 cmds with ORELSE
                of("Z1A S10 | Z1B\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"),
                of("Z1A S1 | Z1B S20\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|BS20\n"),
                of("Z1A S10 | Z1B S20\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"),

                // 3 cmds with ANDTHEN
                of("Z1A S10 & Z1B & Z1C\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"),
                of("Z1A & Z1B S20 & Z1C\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS&BS20\n"),
                of("Z1A & Z1B & Z1C S30\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS&BS&CS30\n"),
                of("Z1A S10 & Z1B S20 & Z1C\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"),
                of("Z1A S1 & Z1B S20 & Z1C S30\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1\n"),

                // 3 cmds with ORELSE
                of("Z1A S10 | Z1B | Z1C\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"),
                of("Z1A | Z1B S20 | Z1C\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS\n"),
                of("Z1A | Z1B | Z1C S30\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS\n"),
                of("Z1A S10 | Z1B S20 | Z1C\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"),
                of("Z1A S1 | Z1B S20 | Z1C S1\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|BS20\n"),

                // 3 cmds with ANDTHEN/ORELSE combo
                of("Z1A S10 & Z1B | Z1C\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"),
                of("Z1A S10 & Z1B S1| Z1C\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"),
                of("Z1A S10 & Z1B S20 | Z1C\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"),
                of("Z1A S1 & Z1B S20 | Z1C\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|CS\n"),
                of("Z1A S1 & Z1B S20 | Z1CS30\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|CS30\n"),

                of("Z1A S10 | Z1B & Z1C\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"),
                of("Z1A S10 | Z1B S1 & Z1C\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"),
                of("Z1A S10 | Z1B S20 & Z1C\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"),
                of("Z1A S1 | Z1B S20 & Z1C\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|BS20\n"),
                of("Z1A S1 | Z1B S20 & Z1CS30\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|BS20\n"));
    }

    @ParameterizedTest
    @MethodSource
    public void shouldProduceActionsForParenthesizedLogicalCommandSeries(final String text, final List<ActionType> actionTypes, String finalOutput) {
        parserActionTester.parseSnippet(text, actionTypes);
        assertThat(outStream.getString()).isEqualTo(finalOutput);
        assertThat(outStream.isOpen()).isFalse();
    }

    private static Stream<Arguments> shouldProduceActionsForParenthesizedLogicalCommandSeries() {
        return Stream.of(
                of("(Z1A) \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!(AS)\n"),
                of("(Z1A)Z1B \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!(AS)BS\n"),
                of("(Z1A S1)Z1B \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, CLOSE_PAREN, END_SEQUENCE, WAIT_FOR_TOKENS), "!(AS1)\n"),
                of("Z1A(Z1B) \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS(BS)\n"),
                of("Z1A S1(Z1B) \n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1\n"),

                of("Z1A & (Z1B & Z1C) & Z1D \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS),
                        "!AS&(BS&CS)&DS\n"),
                of("Z1A S1 & (Z1B & Z1C) & Z1D \n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1\n"),
                of("Z1A S1 & (Z1B & Z1C) | Z1D \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|DS\n"),

                of("Z1A | (Z1B | Z1C) | Z1D \n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS\n"),
                of("Z1A & (Z1B | Z1C) | Z1D \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS&(BS)\n"),

                of("Z1A S1 | (Z1B | Z1C) | Z1D \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|(BS)\n"),
                of("Z1A S1 | (Z1B S1 | Z1C) | Z1D \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS),
                        "!AS1|(BS1|CS)\n"),
                of("Z1A S1 | (Z1B S1 | Z1C) & Z1D \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS),
                        "!AS1|(BS1|CS)&DS\n"),
                of("Z1A S1 | (Z1B S1 | Z1C S1) | Z1D \n",
                        List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, CLOSE_PAREN, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|(BS1|CS1)|DS\n"),
                of("Z1A S1 | (Z1B S10 | Z1C) | Z1D \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|(BS10\n"),
                of("Z1A S1 | (Z1B | Z1C S10) | Z1D \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|(BS)\n"),
                of("Z1A S1 | (Z1B | Z1C S10) | Z1D \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|(BS)\n"),
                of("Z1A | (Z1B | Z1C) | Z1D S10 \n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS\n"),
                of("Z1A | (Z1B | Z1C) & Z1D \n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS\n"),
                of("Z1A | (Z1B | Z1C S10) & Z1D \n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS\n"));
    }

    @ParameterizedTest
    @MethodSource
    public void shouldProduceActionsForNestedParenthesizedLogicalCommandSeries(final String text, final List<ActionType> actionTypes, String finalOutput) {
        parserActionTester.parseSnippet(text, actionTypes);
        assertThat(outStream.getString()).isEqualTo(finalOutput);
        assertThat(outStream.isOpen()).isFalse();
    }

    private static Stream<Arguments> shouldProduceActionsForNestedParenthesizedLogicalCommandSeries() {
        return Stream.of(
                of("((Z1A)) \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!((AS))\n"),
                of("((Z1A)Z1B) \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!((AS)BS)\n"),
                of("(Z1A S1)Z1B \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, CLOSE_PAREN, END_SEQUENCE, WAIT_FOR_TOKENS), "!(AS1)\n"),
                of("(Z1A(Z1B)) \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!(AS(BS))\n"),
                of("(Z1A S1(Z1B)) \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, CLOSE_PAREN, END_SEQUENCE, WAIT_FOR_TOKENS), "!(AS1)\n"),

                of("Z1A & ((Z1B & Z1C)) & Z1D \n",
                        List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS),
                        "!AS&((BS&CS))&DS\n"));
    }
}
