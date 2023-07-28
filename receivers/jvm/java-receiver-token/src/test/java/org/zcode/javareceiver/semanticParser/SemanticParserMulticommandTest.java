package org.zcode.javareceiver.semanticParser;

import static org.assertj.core.api.Assertions.assertThat;
import static org.junit.jupiter.params.provider.Arguments.of;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.CLOSE_PAREN;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.END_SEQUENCE;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.RUN_COMMAND;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.RUN_FIRST_COMMAND;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.WAIT_FOR_TOKENS;

import java.util.List;
import java.util.stream.Stream;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;
import org.zcode.javareceiver.core.StringBuilderOutStream;
import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.execution.ZcodeAction;
import org.zcode.javareceiver.modules.core.ZcodeCoreModule;
import org.zcode.javareceiver.modules.outerCore.ZcodeOuterCoreModule;
import org.zcode.javareceiver.semanticParser.SemanticParser.State;
import org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenRingBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

class SemanticParserMulticommandTest {
    private final ZcodeTokenBuffer buffer    = ZcodeTokenRingBuffer.createBufferWithCapacity(256);
    private final ZcodeTokenizer   tokenizer = new ZcodeTokenizer(buffer.getTokenWriter(), 2);

    private final SemanticParser parser = new SemanticParser(buffer.getTokenReader(), new ExecutionActionFactory());

    private final Zcode zcode = new Zcode();

    private final StringBuilderOutStream outStream = new StringBuilderOutStream();

    ParserActionTester parserActionTester = new ParserActionTester(zcode, buffer, tokenizer, parser, outStream);

    @BeforeEach
    void setUp() throws Exception {
        zcode.addModule(new ZcodeCoreModule());
        zcode.addModule(new ZcodeOuterCoreModule());
    }

    @Test
    void shouldStartNoActionWithEmptyTokens() {
        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);
        final ZcodeAction a = parser.getAction();
        assertThat(((ZcodeSemanticAction) a).getType()).isEqualTo(WAIT_FOR_TOKENS);
        assertThat(outStream.getString()).isEqualTo("");
        assertThat(parser.getState()).isEqualTo(State.PRESEQUENCE);
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
