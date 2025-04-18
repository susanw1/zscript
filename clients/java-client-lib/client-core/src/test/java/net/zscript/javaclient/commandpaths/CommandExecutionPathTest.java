package net.zscript.javaclient.commandpaths;

import java.util.Iterator;
import java.util.List;
import java.util.stream.Stream;

import static java.util.Arrays.asList;
import static java.util.Spliterators.spliteratorUnknownSize;
import static java.util.stream.Collectors.toList;
import static java.util.stream.StreamSupport.stream;
import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenizeWithoutRejection;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.CsvSource;
import org.junit.jupiter.params.provider.MethodSource;
import org.junit.jupiter.params.provider.ValueSource;
import org.mockito.Mockito;

import net.zscript.javaclient.ZscriptMismatchedResponseException;
import net.zscript.javaclient.ZscriptParseException;
import net.zscript.javaclient.tokens.ExtendingTokenBuffer;
import net.zscript.model.ZscriptModel;
import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.util.ByteString.ByteAppendable;

class CommandExecutionPathTest {
    @ParameterizedTest
    @CsvSource({
            "'',                                ''",
            "Z1A1,                              Z1A1",
            "Z1BaA01,                           Z1A1Ba",
            "Z1A1 & Z1A2,                       Z1A1&Z1A2",
            "Z1A1 | Z1A2,                       Z1A1|Z1A2",
            "Z1A1Babcdef | Z1A2,                Z1A1Babcdef|Z1A2",
            "A,                                 A",
            "A & B,                             A&B",
            "A | B,                             A|B",
    })
    public void shouldParseSimpleCommandSequence(String cmds, String encoded) {
        getAndCheckCommandExecutionPath(cmds, encoded);
    }

    @ParameterizedTest
    @CsvSource({
            "(A | B) & C,               (A|B)&C",
            "(A | B) & (C | D),         (A|B)&(C|D)",
    })
    public void shouldParseCommandSequenceWithParens(String cmds, String encoded) {
        getAndCheckCommandExecutionPath(cmds, encoded);
    }

    @Test
    public void shouldLinkCommandSequenceWithNoFailure() {
        final CommandExecutionPath cmdPath = getAndCheckCommandExecutionPath("A & B", "A&B");

        final CommandElement commandA = cmdPath.getFirstCommand();
        assertThat(commandA).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("A");
        final CommandElement commandB = commandA.getOnSuccess();
        assertThat(commandB).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("B");

        assertThat(commandA.getOnFail()).isNull();
        assertThat(commandB.getOnSuccess()).isNull();
        assertThat(commandB.getOnFail()).isNull();
    }

    @Test
    public void shouldLinkCommandSequenceWithFallback() {
        // if A or B fail, then branch to failure handler C
        final CommandExecutionPath cmdPath = getAndCheckCommandExecutionPath("A & B | C", "A&B|C");

        final CommandElement commandA = cmdPath.getFirstCommand();
        assertThat(commandA).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("A");
        final CommandElement commandB = commandA.getOnSuccess();
        assertThat(commandB).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("B");
        final CommandElement commandC = commandA.getOnFail();
        assertThat(commandC).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("C");

        assertThat(commandB.getOnFail()).isSameAs(commandC);
        assertThat(commandB.getOnSuccess()).isNull();
        assertThat(commandC.getOnSuccess()).isNull();
        assertThat(commandC.getOnFail()).isNull();
    }

    @Test
    public void shouldLinkFallbackSequenceToFollowingCommand() {
        // try A (or-else B as fallback for A); if either succeeds, then do C
        final CommandExecutionPath cmdPath = getAndCheckCommandExecutionPath("(A | B) & C", "(A|B)&C");

        final CommandElement parenOpen = cmdPath.getFirstCommand();
        assertThat(parenOpen).isNotNull();
        final CommandElement commandA = parenOpen.getOnSuccess();
        assertThat(commandA).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("A");
        final CommandElement commandB = commandA.getOnFail();
        assertThat(commandB).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("B");
        final CommandElement parenClose = commandA.getOnSuccess();
        assertThat(parenClose).isNotNull();
        final CommandElement commandC = parenClose.getOnSuccess();
        assertThat(commandC).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("C");

        assertThat(parenOpen.getOnFail()).isNull();
        assertThat(commandB.getOnSuccess()).isSameAs(parenClose);
        assertThat(commandB.getOnFail()).isNull();
        assertThat(commandC.getOnSuccess()).isNull();
        assertThat(commandC.getOnFail()).isNull();
    }

    @Test
    public void shouldLinkCommandToParenFallback() {
        // If A succeeds, do B (or-else C as fallback for B)
        final CommandExecutionPath cmdPath = getAndCheckCommandExecutionPath("A & (B | C)", "A&(B|C)");

        // Check main steps
        final CommandElement commandA = cmdPath.getFirstCommand();
        assertThat(commandA).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("A");
        final CommandElement parenOpen = commandA.getOnSuccess();
        assertThat(parenOpen).isNotNull();
        final CommandElement commandB = parenOpen.getOnSuccess();
        assertThat(commandB).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("B");
        final CommandElement parenClose = commandB.getOnSuccess();
        assertThat(parenClose).isNotNull();
        final CommandElement commandC = commandB.getOnFail();
        assertThat(commandC).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("C");

        // Check alternative
        assertThat(commandA.getOnFail()).isNull();
        assertThat(parenOpen.getOnFail()).isNull();
        assertThat(commandC.getOnSuccess()).isSameAs(parenClose);
        assertThat(commandC.getOnFail()).isNull();
    }

    @Test
    public void shouldLinkCommandSequenceWithParenedFailure() {
        final CommandExecutionPath cmdPath = getAndCheckCommandExecutionPath("A | B & (C|D)", "A|B&(C|D)");

        final CommandElement commandA = cmdPath.getFirstCommand();
        assertThat(commandA).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("A");
        final CommandElement commandB = commandA.getOnFail();
        assertThat(commandB).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("B");
        final CommandElement parenOpen = commandB.getOnSuccess();
        assertThat(parenOpen).isNotNull();
        final CommandElement commandC = parenOpen.getOnSuccess();
        assertThat(commandC).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("C");
        final CommandElement commandD = commandC.getOnFail();
        assertThat(commandD).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("D");
        final CommandElement parenClose = commandC.getOnSuccess();
        assertThat(parenClose).isNotNull();

        assertThat(commandA.getOnSuccess()).isNull();
        assertThat(commandB.getOnFail()).isNull();
        assertThat(parenOpen.getOnFail()).isNull();
        assertThat(commandC.getOnSuccess()).isSameAs(parenClose);
        assertThat(commandD.getOnSuccess()).isSameAs(parenClose);
        assertThat(commandD.getOnFail()).isNull();
    }

    @Test
    public void shouldIterateEmptyCommand() {
        final CommandExecutionPath cmdPath = CommandExecutionPath.parse(tokenize(byteStringUtf8(""), true).getTokenReader().getFirstReadToken());
        Iterator<CommandElement>   iter    = cmdPath.iterator();
        assertThat(iter.hasNext()).isTrue();
        assertThat(iter.next().isEmpty()).isTrue();
        assertThat(iter.hasNext()).isFalse();
    }

    @Test
    public void shouldIterateComplexCommand() {
        final CommandExecutionPath cmdPath = CommandExecutionPath.parse(tokenize(byteStringUtf8("A&(B|C)&D"), true).getTokenReader().getFirstReadToken());

        Iterator<CommandElement> iter = cmdPath.iterator();
        assertThat(iter.hasNext()).isTrue();
        final List<CommandElement> elements = stream(spliteratorUnknownSize(iter, 0), false).collect(toList());
        assertThat(elements).extracting(ByteAppendable::asStringUtf8).containsExactlyInAnyOrder("A", "", "B", "", "C", "D");
    }

    @ParameterizedTest
    @ValueSource(strings = {
            "x\n",
            "Z%\n",
    })
    public void shouldRejectInvalidFields(String badCmdSeqs) {
        final ReadToken token = tokenizeWithoutRejection(byteStringUtf8(badCmdSeqs)).getTokenReader().getFirstReadToken();
        assertThatThrownBy(() -> CommandExecutionPath.parse(token))
                .isInstanceOf(ZscriptParseException.class).hasMessageContaining("Invalid numeric key");
    }

    @ParameterizedTest
    @ValueSource(strings = {
            "Z12345\n",
            "Aabcde\n",
    })
    public void shouldRejectFailedTokenization(String badCmdSeqs) {
        final ReadToken token = tokenizeWithoutRejection(byteStringUtf8(badCmdSeqs)).getTokenReader().getFirstReadToken();
        assertThatThrownBy(() -> CommandExecutionPath.parse(token))
                .isInstanceOf(ZscriptParseException.class).hasMessageContaining("Syntax error");
    }

    // This test is a bit of a stretch, as the tokenizer only places known markers. Here we force a (currently) unknown one!
    @Test
    public void shouldRejectUnknownMarkers() {
        final ExtendingTokenBuffer buffer = new ExtendingTokenBuffer();
        buffer.getTokenWriter().writeMarker((byte) 0xE9);
        final ReadToken token = buffer.getTokenReader().getFirstReadToken();
        assertThatThrownBy(() -> CommandExecutionPath.parse(token))
                .isInstanceOf(ZscriptParseException.class).hasMessageContaining("Unknown separator");
    }

    @ParameterizedTest
    @MethodSource("validCommandResponses")
    public void shouldMatchCommandToResponse(String cmd, String resp, String matchExpr) {
        final List<MatchedCommandResponse> result = compare(cmd, resp, true);

        // matchExpr is ';'-sep'd list of 'cmd:resp' pairs
        final List<String> elements = asList(matchExpr.split("; *"));
        assertThat(result).hasSize(elements.size());

        for (int i = 0; i < elements.size(); i++) {
            final List<String>   cmdResp = asList(elements.get(i).split(":", 2));
            final CommandElement command = result.get(i).getCommand();
            assertThat(command.asStringUtf8()).isEqualTo(cmdResp.get(0));
            final ResponseElement response = result.get(i).getResponse();
            assertThat(response.asStringUtf8()).isEqualTo(cmdResp.get(1));
        }
    }

    private static Stream<Arguments> validCommandResponses() {
        return Stream.of(
                Arguments.of("A", "AS", "A:AS"),
                Arguments.of("A", "AS2", "A:AS2"),
                Arguments.of("A & B", "AS2", "A:AS2"),
                Arguments.of("A | B", "AS", "A:AS"),
                Arguments.of("A | B", "AS2 | BS", "A:AS2; B:BS"),
                Arguments.of("A | B", "AS2 | BS2", "A:AS2; B:BS2"),
                Arguments.of("A & B & C", "AS & BS & CS", "A:AS; B:BS; C:CS"),
                Arguments.of("A & B & C", "AS & BS & CS2", "A:AS; B:BS; C:CS2"),
                Arguments.of("A & B & C", "AS & BS2", "A:AS; B:BS2"),
                Arguments.of("A & B & C", "AS2", "A:AS2"),
                Arguments.of("A | B | C", "AS2 | BS", "A:AS2; B:BS"),
                Arguments.of("A | B | C", "AS2 | BS2 | CS", "A:AS2; B:BS2; C:CS"),

                Arguments.of("A & B | C", "AS & BS", "A:AS; B:BS"),
                Arguments.of("A & B | C", "AS & BS2 | CS", "A:AS; B:BS2; C:CS"),
                Arguments.of("A & B | C", "AS2 | CS", "A:AS2; C:CS"),
                Arguments.of("A & B | C", "AS2 | CS2", "A:AS2; C:CS2"),

                Arguments.of("A | B & C", "AS", "A:AS"),
                Arguments.of("A | B & C", "AS2 | BS & CS", "A:AS2; B:BS; C:CS"),
                Arguments.of("A | B & C", "AS2 | BS & CS2", "A:AS2; B:BS; C:CS2"),
                Arguments.of("A | B & C", "AS2 | BS2", "A:AS2; B:BS2"),

                Arguments.of("A & (B | C)", "AS & (BS)", "A:AS; :; B:BS; :"),
                Arguments.of("A & (B | C)", "AS & (BS2 | CS)", "A:AS; :; B:BS2; C:CS;  :"),
                Arguments.of("A & (B | C)", "AS & (BS2 | CS2)", "A:AS; :; B:BS2; C:CS2"),
                Arguments.of("A & (B | C)", "AS2", "A:AS2"),

                Arguments.of("A & (B | C", "AS & (BS", "A:AS; :; B:BS"),
                Arguments.of("A & (B | C", "AS & (BS2 | CS", "A:AS; :; B:BS2; C:CS"),
                Arguments.of("A & (B | C", "AS & (BS2 | CS2", "A:AS; :; B:BS2; C:CS2"),
                Arguments.of("A & (B | C", "AS2", "A:AS2"),

                Arguments.of("(A | B) & C", "(AS) & CS", ":; A:AS; :; C:CS"),
                Arguments.of("(A | B) & C", "(AS) & CS2", ":; A:AS; :; C:CS2"),
                Arguments.of("(A | B) & C", "(AS2 | BS) & CS", ":; A:AS2; B:BS; :; C:CS"),
                Arguments.of("(A | B) & C", "(AS2 | BS2)", ":; A:AS2; B:BS2"),

                Arguments.of("A | B) & C", "AS) & CS", "A:AS; :; C:CS"),
                Arguments.of("A | B) & C", "AS) & CS2", "A:AS; :; C:CS2"),
                Arguments.of("A | B) & C", "AS2 | BS) & CS", "A:AS2; B:BS; :; C:CS"),
                Arguments.of("A | B) & C", "AS2 | BS2)", "A:AS2; B:BS2"),

                Arguments.of("(A | B) & (C | D) & E", "(AS) & (CS) & ES", ":; A:AS; :; :; C:CS; :; E:ES"),
                Arguments.of("(A | B) & (C | D) & E", "(AS2 | BS) & (CS)", ":; A:AS2; B:BS; :; :; C:CS; :;"),
                Arguments.of("(A | B) & (C | D) & E", "(AS2|BS2)", ":; A:AS2; B:BS2")
        );
    }

    @ParameterizedTest
    @MethodSource("invalidCommandResponses")
    public void shouldRejectUnmatchedCommandToResponse(String cmd, String resp, String errorText) {
        assertThatThrownBy(() -> compare(cmd, resp, false))
                .isInstanceOf(ZscriptMismatchedResponseException.class)
                .hasMessageContaining(errorText);
    }

    private static Stream<Arguments> invalidCommandResponses() {
        return Stream.of(
                Arguments.of("A", "AS & S", "ended before response"),
                Arguments.of("A & B", "AS2 & BS", "ended before response"),
                //                Arguments.of("A | B", "AS | BS", "ended before response"), // impossible, but uncaught

                Arguments.of("A | B", "AS & BS", "ended before response"),
                Arguments.of("A & B", "AS | BS", "ended before response"),
                Arguments.of("A & B", "AS)", "Response has ')' without valid opening '('")
        );
    }

    private static CommandExecutionPath getAndCheckCommandExecutionPath(String cmds, String expected) {
        final ReadToken            token   = tokenize(byteStringUtf8(cmds), true).getTokenReader().getFirstReadToken();
        final CommandExecutionPath cmdPath = CommandExecutionPath.parse(token);
        assertThat(cmdPath.asStringUtf8()).isEqualTo(expected);
        return cmdPath;
    }

    private static List<MatchedCommandResponse> compare(String cmd, String resp, boolean expectMatch) {
        final CommandExecutionPath  cmdPath  = CommandExecutionPath.parse(tokenize(byteStringUtf8(cmd), true).getTokenReader().getFirstReadToken());
        final ResponseExecutionPath respPath = ResponseExecutionPath.parse(tokenize(byteStringUtf8(resp), true).getTokenReader().getFirstReadToken());
        assertThat(cmdPath.matchesResponses(respPath)).isEqualTo(expectMatch);
        return cmdPath.compareResponses(respPath);
    }

    @Test
    public void shouldSupportNullTokenAndAModel() {
        final ZscriptModel         model   = Mockito.mock(ZscriptModel.class);
        final CommandExecutionPath cmdPath = CommandExecutionPath.from(model, null);
        assertThat(cmdPath.getModel()).isSameAs(model);
        assertThat(cmdPath.asStringUtf8()).isEqualTo("");
    }

    @Test
    public void shouldCreateBlank() {
        final ZscriptModel         model   = Mockito.mock(ZscriptModel.class);
        final CommandExecutionPath cmdPath = CommandExecutionPath.blank(model);
        assertThat(cmdPath.getModel()).isSameAs(model);
        assertThat(cmdPath.asStringUtf8()).isEqualTo("");
        assertThat(cmdPath.getFirstCommand()).isNull();
    }

    @Test
    public void shouldSupportToString() {
        final ReadToken            token   = tokenize(byteStringUtf8("A1B2"), true).getTokenReader().getFirstReadToken();
        final CommandExecutionPath cmdPath = CommandExecutionPath.parse(token);
        assertThat(cmdPath).hasToString("CommandExecutionPath:{'A1B2'}");
    }
}
