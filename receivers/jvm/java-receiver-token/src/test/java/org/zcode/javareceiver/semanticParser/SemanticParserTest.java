package org.zcode.javareceiver.semanticParser;

import static org.assertj.core.api.Assertions.assertThat;
import static org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType.COMMAND;
import static org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType.END_SEQUENCE;
import static org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType.FIRST_COMMAND;
import static org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType.NEEDS_TOKENS;
import static org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType.NO_ACTION;

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
        final ZcodeAction a = parser.getAction();
        assertThat(a.getType()).isEqualTo(NEEDS_TOKENS);
        assertThat(outStream.getString()).isEqualTo("");
    }

    @Test
    void shouldProduceActionForCommand() {
        "Z0\n".chars().forEachOrdered(c -> tokenizer.accept((byte) c));

        final ZcodeAction a1 = parser.getAction();
        assertThat(a1.getType()).isEqualTo(FIRST_COMMAND);
        assertThat(a1.getInfo()).isEqualTo((byte) 0);
        a1.performAction(zcode, outStream);
        assertThat(outStream.getStringAndReset()).isEqualTo("!V1C3007M3S");

        final ZcodeAction a2 = parser.getAction();
        assertThat(a2.getType()).isEqualTo(END_SEQUENCE);
        assertThat(a2.getInfo()).isEqualTo((byte) 0);
        a2.performAction(zcode, outStream);
        assertThat(outStream.getStringAndReset()).isEqualTo("\n");

        final ZcodeAction a3 = parser.getAction();
        assertThat(a3.getType()).isEqualTo(NEEDS_TOKENS);
        assertThat(a3.getInfo()).isEqualTo((byte) 0);
        assertThat(outStream.getString()).isEqualTo("");
    }

    @ParameterizedTest
    @MethodSource
    public void shouldProduceActions(final String text, final List<ActionType> actionTypes, String finalOutput) {
        text.chars().forEachOrdered(c -> tokenizer.accept((byte) c));
        actionTypes.forEach(t -> {
            System.out.println("Expecting actionType: " + t);
            ZcodeAction a1;
            while ((a1 = parser.getAction()).getType() == NO_ACTION) {
                // skip, unimportant
            }

            System.out.println("  Received actionType: " + a1);
            assertThat(a1.getType()).isEqualTo(t);
            a1.performAction(zcode, outStream);
        });
        assertThat(outStream.getString()).isEqualTo(finalOutput);
    }

    private static Stream<Arguments> shouldProduceActions() {
        return Stream.of(
                Arguments.of("Z0\n", List.of(FIRST_COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!V1C3007M3S\n"),
                Arguments.of("Z1A\n", List.of(FIRST_COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!AS\n"),
                Arguments.of("Z1A S1\n", List.of(FIRST_COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!AS1\n"),
                Arguments.of("Z1A S10\n", List.of(FIRST_COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!AS10\n"),
                Arguments.of("Z1A & Z1B\n", List.of(FIRST_COMMAND, COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!AS&BS\n"),

                Arguments.of("Z1A & Z1B & Z1C\n", List.of(FIRST_COMMAND, COMMAND, COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!AS&BS&CS\n"),
                Arguments.of("Z1A S1 & Z1B\n", List.of(FIRST_COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!AS1\n"),
                Arguments.of("Z1A | Z1B\n", List.of(FIRST_COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!AS\n"),
                Arguments.of("Z1A S1 | Z1B\n", List.of(FIRST_COMMAND, COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!AS1|BS\n"),
                Arguments.of("Z1A S1 | Z1B S2\n", List.of(FIRST_COMMAND, COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!AS1|BS2\n"),

                Arguments.of("Z1A S10 | Z1B\n", List.of(FIRST_COMMAND, NEEDS_TOKENS), "!AS10\n"), // Error: this fails to produce END_SEQUENCE!
                Arguments.of("Z1A | (Z1B | Z1C) | Z1D \n", List.of(FIRST_COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!AS\n"),
                Arguments.of("Z1A & (Z1B | Z1C) | Z1D \n", List.of(FIRST_COMMAND, COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!AS&(BS)\n"),

                // These need serious checking!!
                Arguments.of("Z1A S1 | (Z1B | Z1C) | Z1D \n", List.of(FIRST_COMMAND, COMMAND, COMMAND, COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!AS1|(BS)\n"),
                Arguments.of("Z1A S1 | (Z1B S1 | Z1C) | Z1D \n", List.of(FIRST_COMMAND, COMMAND, COMMAND, COMMAND, COMMAND, NEEDS_TOKENS), "!AS1|(BS1|CS)"), // !S1|(S1|S)\n
                Arguments.of("Z1A S1 | (Z1B S1 | Z1C S1) | Z1D \n", List.of(FIRST_COMMAND, COMMAND, COMMAND, COMMAND, COMMAND, NEEDS_TOKENS), "!AS1|(S1|S1)S\n"), // !S1|(S1|S)\n
                Arguments.of("Z1A S1 | (Z1B S10 | Z1C) | Z1D \n", List.of(FIRST_COMMAND, COMMAND, COMMAND, NEEDS_TOKENS), "!S1|(S10"), // !S1|(S10)\n
                Arguments.of("Z1A S1 | (Z1B | Z1C S10) | Z1D \n", List.of(FIRST_COMMAND, COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!S\n"),
                Arguments.of("Z1A S1 | (Z1B | Z1C S10) | Z1D \n", List.of(FIRST_COMMAND, COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!S\n"));
//                Arguments.of("Z1A | (Z1B | Z1C) | Z1D S10 \n", List.of(FIRST_COMMAND, COMMAND, END_SEQUENCE, NEEDS_TOKENS)), "!S\n"));
//                Arguments.of("Z1A | (Z1B | Z1C) & Z1D \n", List.of(FIRST_COMMAND, COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!S\n"),
//                Arguments.of("Z1A | (Z1B | Z1C S10) & Z1D \n", List.of(FIRST_COMMAND, COMMAND, END_SEQUENCE, NEEDS_TOKENS), "!S\n"));
    }
}
