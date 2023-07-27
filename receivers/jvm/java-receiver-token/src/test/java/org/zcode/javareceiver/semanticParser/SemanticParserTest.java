package org.zcode.javareceiver.semanticParser;

import static org.assertj.core.api.Assertions.assertThat;
import static org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType.END_SEQUENCE;
import static org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType.RUN_COMMAND;
import static org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType.RUN_FIRST_COMMAND;
import static org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType.WAIT_FOR_TOKENS;

import java.util.List;
import java.util.stream.Stream;

import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;
import org.zcode.javareceiver.core.StringBuilderOutStream;
import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.modules.ZcodeCommandFinder;
import org.zcode.javareceiver.modules.core.ZcodeCoreModule;
import org.zcode.javareceiver.modules.outerCore.ZcodeOuterCoreModule;
import org.zcode.javareceiver.semanticParser.SemanticParser.State;
import org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenRingBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

class SemanticParserTest {
    private final ZcodeTokenBuffer buffer    = ZcodeTokenRingBuffer.createBufferWithCapacity(256);
    private final ZcodeTokenizer   tokenizer = new ZcodeTokenizer(buffer.getTokenWriter(), 2);

    private final SemanticParser parser = new SemanticParser(buffer.getTokenReader(), new ExecutionActionFactory());

    private final Zcode zcode = Zcode.getZcode();

    private final StringBuilderOutStream outStream = new StringBuilderOutStream();

    @BeforeAll
    static void setUp() throws Exception {
        ZcodeCommandFinder.addModule(new ZcodeCoreModule());
        ZcodeCommandFinder.addModule(new ZcodeOuterCoreModule());
    }

    @Test
    void shouldStartNoActionWithEmptyTokens() {
        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);
        final ZcodeAction a = parser.getAction();
        assertThat(a.getType()).isEqualTo(WAIT_FOR_TOKENS);
        assertThat(outStream.getString()).isEqualTo("");
        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);
    }

    @Test
    void shouldProduceActionForCommand() {
        "Z0\n".chars().forEachOrdered(c -> tokenizer.accept((byte) c));

        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);
        final ZcodeAction a1 = parser.getAction();
        assertThat(a1.getType()).isEqualTo(RUN_FIRST_COMMAND);
        a1.performAction(zcode, outStream);
        assertThat(outStream.getStringAndReset()).isEqualTo("!V1C3007M3S");
        assertThat(parser.getState()).isEqualTo(State.COMMAND_COMPLETE);

        final ZcodeAction a2 = parser.getAction();
        assertThat(a2.getType()).isEqualTo(END_SEQUENCE);
        a2.performAction(zcode, outStream);
        assertThat(outStream.getStringAndReset()).isEqualTo("\n");
        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);

        final ZcodeAction a3 = parser.getAction();
        assertThat(a3.getType()).isEqualTo(WAIT_FOR_TOKENS);
        assertThat(outStream.getString()).isEqualTo("");
        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);
    }

    @Test
    void shouldProduceActionForTwoCommands() {
        "Z1A & Z1B\n".chars().forEachOrdered(c -> tokenizer.accept((byte) c));

        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);
        final ZcodeAction a1 = parser.getAction();
        assertThat(a1.getType()).isEqualTo(RUN_FIRST_COMMAND);
        a1.performAction(zcode, outStream);
        assertThat(outStream.getStringAndReset()).isEqualTo("!AS");
        assertThat(parser.getState()).isEqualTo(State.COMMAND_COMPLETE);

        final ZcodeAction a2 = parser.getAction();
        assertThat(a2.getType()).isEqualTo(RUN_COMMAND);
        assertThat(a2.getInfo()).isEqualTo(ZcodeTokenizer.CMD_END_ANDTHEN);
        a2.performAction(zcode, outStream);
        assertThat(outStream.getStringAndReset()).isEqualTo("&BS");
        assertThat(parser.getState()).isEqualTo(State.COMMAND_COMPLETE);

        final ZcodeAction a3 = parser.getAction();
        assertThat(a3.getType()).isEqualTo(END_SEQUENCE);
        a3.performAction(zcode, outStream);
        assertThat(outStream.getStringAndReset()).isEqualTo("\n");
        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);

        final ZcodeAction a4 = parser.getAction();
        assertThat(a4.getType()).isEqualTo(WAIT_FOR_TOKENS);
        assertThat(outStream.getString()).isEqualTo("");
        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);
    }

    @ParameterizedTest
    @MethodSource
    public void shouldProduceActions(final String text, final List<ActionType> actionTypes, String finalOutput) {
        // feed all chars into tokens/buffer
        text.chars().forEachOrdered(c -> tokenizer.accept((byte) c));

        buffer.getTokenReader().iterator().forEach(t -> System.out.print(t + " "));
        System.out.println();

        actionTypes.forEach(t -> {
            System.out.println("Expecting actionType=" + t);
            ZcodeAction a1 = parser.getAction();

            System.out.println("  Received action: actionType=" + a1 + "; state=" + parser.getState());
            assertThat(a1.getType()).isEqualTo(t);
            a1.performAction(zcode, outStream);

            System.out.println("   - After execute action: outStream=" + outStream.getString().replaceAll("\\n", "\\\\n") + "; state=" + parser.getState());
        });
        assertThat(outStream.getString()).isEqualTo(finalOutput);
    }

    private static Stream<Arguments> shouldProduceActions() {
        return Stream.of(
                Arguments.of("Z1A\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS\n"),
                Arguments.of("Z1A S1\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1\n"),
                // Arguments.of("Z1A S10\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS10\n"),
                Arguments.of("Z1A & Z1B\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS&BS\n"),

                Arguments.of("Z1A & Z1B & Z1C\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS&BS&CS\n"),
                Arguments.of("Z1A S1 & Z1B\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1\n"),
                Arguments.of("Z1A | Z1B\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS\n"),
                Arguments.of("Z1A S1 | Z1B\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|BS\n"),
                Arguments.of("Z1A S1 | Z1B S2\n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|BS2\n"),

//                Arguments.of("Z1A S10 | Z1B\n", List.of(RUN_FIRST_COMMAND, WAIT_FOR_TOKENS), "!AS10\n"), // Error: this fails to produce END_SEQUENCE!
                Arguments.of("Z1A | (Z1B | Z1C) | Z1D \n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS\n"),
//                Arguments.of("Z1A & (Z1B | Z1C) | Z1D \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS&(BS)\n"),

                // These need serious checking!!
//                Arguments.of("Z1A S1 | (Z1B | Z1C) | Z1D \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!AS1|(BS)\n"),
//                Arguments.of("Z1A S1 | (Z1B S1 | Z1C) | Z1D \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, WAIT_FOR_TOKENS), "!AS1|(BS1|CS)"), // !S1|(S1|S)\n
//                Arguments.of("Z1A S1 | (Z1B S1 | Z1C S1) | Z1D \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, WAIT_FOR_TOKENS),
//                        "!AS1|(S1|S1)S\n"), // !S1|(S1|S)\n
//                Arguments.of("Z1A S1 | (Z1B S10 | Z1C) | Z1D \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, WAIT_FOR_TOKENS), "!S1|(S10"), // !S1|(S10)\n
//                Arguments.of("Z1A S1 | (Z1B | Z1C S10) | Z1D \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!S\n"),
//                Arguments.of("Z1A S1 | (Z1B | Z1C S10) | Z1D \n", List.of(RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!S\n"));
//                Arguments.of("Z1A | (Z1B | Z1C) | Z1D S10 \n", List.of(FIRST_COMMAND, COMMAND, END_SEQUENCE, NEEDS_TOKENS)), "!S\n"));
//                Arguments.of("Z1A | (Z1B | Z1C) & Z1D \n", List.of(FIRST_COMMAND, COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!S\n"),
//                Arguments.of("Z1A | (Z1B | Z1C S10) & Z1D \n", List.of(FIRST_COMMAND, COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!S\n"));
                Arguments.of("Z0\n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS), "!V1C3007M3S\n"));
    }
}
