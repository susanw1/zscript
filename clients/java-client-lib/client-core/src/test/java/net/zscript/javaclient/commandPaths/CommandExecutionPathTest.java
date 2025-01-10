package net.zscript.javaclient.commandPaths;

import java.util.List;

import static java.util.Arrays.asList;
import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenizeWithoutRejection;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.CsvSource;
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
            "Z1A1+ab | Z1A2,                    Z1A1+ab|Z1A2",
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

        final Command commandA = cmdPath.getFirstCommand();
        assertThat(commandA).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("A");
        final Command commandB = commandA.getOnSuccess();
        assertThat(commandB).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("B");

        assertThat(commandA.getOnFail()).isNull();
        assertThat(commandB.getOnSuccess()).isNull();
        assertThat(commandB.getOnFail()).isNull();
    }

    @Test
    public void shouldLinkCommandSequenceWithFallback() {
        // if A or B fail, then branch to failure handler C
        final CommandExecutionPath cmdPath = getAndCheckCommandExecutionPath("A & B | C", "A&B|C");

        final Command commandA = cmdPath.getFirstCommand();
        assertThat(commandA).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("A");
        final Command commandB = commandA.getOnSuccess();
        assertThat(commandB).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("B");
        final Command commandC = commandA.getOnFail();
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

        final Command parenOpen = cmdPath.getFirstCommand();
        assertThat(parenOpen).isNotNull();
        final Command commandA = parenOpen.getOnSuccess();
        assertThat(commandA).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("A");
        final Command commandB = commandA.getOnFail();
        assertThat(commandB).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("B");
        final Command parenClose = commandA.getOnSuccess();
        assertThat(parenClose).isNotNull();
        final Command commandC = parenClose.getOnSuccess();
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
        final Command commandA = cmdPath.getFirstCommand();
        assertThat(commandA).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("A");
        final Command parenOpen = commandA.getOnSuccess();
        assertThat(parenOpen).isNotNull();
        final Command commandB = parenOpen.getOnSuccess();
        assertThat(commandB).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("B");
        final Command parenClose = commandB.getOnSuccess();
        assertThat(parenClose).isNotNull();
        final Command commandC = commandB.getOnFail();
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

        final Command commandA = cmdPath.getFirstCommand();
        assertThat(commandA).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("A");
        final Command commandB = commandA.getOnFail();
        assertThat(commandB).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("B");
        final Command parenOpen = commandB.getOnSuccess();
        assertThat(parenOpen).isNotNull();
        final Command commandC = parenOpen.getOnSuccess();
        assertThat(commandC).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("C");
        final Command commandD = commandC.getOnFail();
        assertThat(commandD).isNotNull().extracting(ByteAppendable::asStringUtf8).isEqualTo("D");
        final Command parenClose = commandC.getOnSuccess();
        assertThat(parenClose).isNotNull();

        assertThat(commandA.getOnSuccess()).isNull();
        assertThat(commandB.getOnFail()).isNull();
        assertThat(parenOpen.getOnFail()).isNull();
        assertThat(commandC.getOnSuccess()).isSameAs(parenClose);
        assertThat(commandD.getOnSuccess()).isSameAs(parenClose);
        assertThat(commandD.getOnFail()).isNull();
    }

    @ParameterizedTest
    @ValueSource(strings = {
            "x\n",
            "Z%\n",
    })
    public void shouldRejectInvalidFields(String badCmdSeqs) {
        ReadToken token = tokenizeWithoutRejection(byteStringUtf8(badCmdSeqs)).getTokenReader().getFirstReadToken();
        assertThatThrownBy(() -> CommandExecutionPath.parse(token))
                .isInstanceOf(ZscriptParseException.class).hasMessageContaining("Invalid numeric key");
    }

    @ParameterizedTest
    @ValueSource(strings = {
            "Z12345\n",
            "+123\n",
    })
    public void shouldRejectFailedTokenization(String badCmdSeqs) {
        ReadToken token = tokenizeWithoutRejection(byteStringUtf8(badCmdSeqs)).getTokenReader().getFirstReadToken();
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
    @CsvSource({
            "A,                         AS,                 'A:AS'",
            "A,                         AS2,                'A:AS2'",
            "A & B,                     AS2,                'A:AS2'",
            "A | B,                     AS,                 'A:AS'",
            "A | B,                     AS2 | BS,           'A:AS2; B:BS'",
            "A | B,                     AS2 | BS2,          'A:AS2; B:BS2'",
            "A & B & C,                 AS & BS & CS,       'A:AS; B:BS; C:CS'",
            "A & B & C,                 AS & BS & CS2,      'A:AS; B:BS; C:CS2'",
            "A & B & C,                 AS & BS2,           'A:AS; B:BS2'",
            "A & B & C,                 AS2,                'A:AS2'",
            "A | B | C,                 AS2 | BS,           'A:AS2; B:BS'",
            "A | B | C,                 AS2 | BS2 | CS,     'A:AS2; B:BS2; C:CS'",

            "A & B | C,                 AS & BS,            'A:AS; B:BS'",
            "A & B | C,                 AS & BS2 | CS,      'A:AS; B:BS2; C:CS'",
            "A & B | C,                 AS2 | CS,           'A:AS2; C:CS'",
            "A & B | C,                 AS2 | CS2,          'A:AS2; C:CS2'",

            "A | B & C,                 AS,                 'A:AS'",
            "A | B & C,                 AS2 | BS & CS,      'A:AS2; B:BS; C:CS'",
            "A | B & C,                 AS2 | BS & CS2,     'A:AS2; B:BS; C:CS2'",
            "A | B & C,                 AS2 | BS2,          'A:AS2; B:BS2'",

            "A & (B | C),                AS & (BS),          'A:AS; :; B:BS; :'",
            "A & (B | C),                AS & (BS2 | CS),    'A:AS; :; B:BS2; C:CS;  :'",
            "A & (B | C),                AS & (BS2 | CS2),   'A:AS; :; B:BS2; C:CS2; :'",
            "A & (B | C),                AS2,                'A:AS2'",

            "A & (B | C,                 AS & (BS,           'A:AS; :; B:BS'",
            "A & (B | C,                 AS & (BS2 | CS,     'A:AS; :; B:BS2; C:CS'",
            "A & (B | C,                 AS & (BS2 | CS2,    'A:AS; :; B:BS2; C:CS2'",
            "A & (B | C,                 AS2,                'A:AS2'",

            "(A | B) & C,                (AS) & CS,          ':; A:AS; :; C:CS'",
            "(A | B) & C,                (AS) & CS2,         ':; A:AS; :; C:CS2'",
            "(A | B) & C,                (AS2 | BS) & CS,    ':; A:AS2; B:BS; :; C:CS'",
            "(A | B) & C,                (AS2 | BS2),        ':; A:AS2; B:BS2; :'",

            "A | B) & C,                 AS) & CS,           'A:AS; :; C:CS'",
            "A | B) & C,                 AS) & CS2,          'A:AS; :; C:CS2'",
            "A | B) & C,                 AS2 | BS) & CS,     'A:AS2; B:BS; :; C:CS'",
            "A | B) & C,                 AS2 | BS2),         'A:AS2; B:BS2; :'",

            "(A | B) & (C | D) & E,      (AS) & (CS) & ES,    ':; A:AS; :; :; C:CS; :; E:ES'",
            "(A | B) & (C | D) & E,      (AS2 | BS) & (CS),   ':; A:AS2; B:BS; :; :; C:CS; :;'",
            "(A | B) & (C | D) & E,      (AS2|BS2),           ':; A:AS2; B:BS2; :;'",
    })
    public void shouldMatchCommandToResponse(String cmd, String resp, String matchExpr) {
        final List<MatchedCommandResponse> result = compare(cmd, resp);

        // matchExpr is ';'-sep'd list of 'cmd:resp' pairs
        List<String> elements = asList(matchExpr.split("; *"));
        assertThat(result).hasSize(elements.size());

        for (int i = 0; i < elements.size(); i++) {
            List<String>  cmdResp = asList(elements.get(i).split(":", 2));
            final Command command = result.get(i).getCommand();
            assertThat(command.asStringUtf8()).isEqualTo(cmdResp.get(0));
            final Response response = result.get(i).getResponse();
            assertThat(response.asStringUtf8()).isEqualTo(cmdResp.get(1));
        }
    }

    @ParameterizedTest
    @CsvSource({
            "A,                         AS & S,                 ended before response",
            //            "A & B,                     AS2 & BS,               ", // impossible, but uncaught
            //            "A | B,                     AS | BS,                ", // impossible, but uncaught
            "A | B,                     AS & BS,                ended before response",
            "A & B,                     AS | BS,                ended before response",
            "A & B,                     AS),                    Response has ')' without valid opening '('",
    })
    public void shouldRejectUnmatchedCommandToResponse(String cmd, String resp, String errorText) {
        assertThatThrownBy(() -> compare(cmd, resp))
                .isInstanceOf(ZscriptMismatchedResponseException.class)
                .hasMessageContaining(errorText);
    }

    private static CommandExecutionPath getAndCheckCommandExecutionPath(String cmds, String expected) {
        ReadToken            token   = tokenize(byteStringUtf8(cmds), true).getTokenReader().getFirstReadToken();
        CommandExecutionPath cmdPath = CommandExecutionPath.parse(token);
        assertThat(cmdPath.asStringUtf8()).isEqualTo(expected);
        return cmdPath;
    }

    private static List<MatchedCommandResponse> compare(String cmd, String resp) {
        final CommandExecutionPath  cmdPath  = CommandExecutionPath.parse(tokenize(byteStringUtf8(cmd), true).getTokenReader().getFirstReadToken());
        final ResponseExecutionPath respPath = ResponseExecutionPath.parse(tokenize(byteStringUtf8(resp), true).getTokenReader().getFirstReadToken());
        return cmdPath.compareResponses(respPath);
    }

    @Test
    public void shouldSupportNullTokenAndAModel() {
        ZscriptModel         model   = Mockito.mock(ZscriptModel.class);
        CommandExecutionPath cmdPath = CommandExecutionPath.from(model, null);
        assertThat(cmdPath.getModel()).isSameAs(model);
        assertThat(cmdPath.asStringUtf8()).isEqualTo("");
    }

    @Test
    public void shouldSupportToString() {
        ReadToken            token   = tokenize(byteStringUtf8("A1B2"), true).getTokenReader().getFirstReadToken();
        CommandExecutionPath cmdPath = CommandExecutionPath.parse(token);
        assertThat(cmdPath).hasToString("CommandExecutionPath:{'A1B2'}");
    }
}
