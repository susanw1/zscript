package net.zscript.javaclient.commandpaths;

import java.util.List;

import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.ZscriptParseException;
import net.zscript.tokenizer.TokenBuffer;

class CompleteAddressedResponseTest {
    @Test
    public void shouldParseUnaddressedResponse() {
        CompleteAddressedResponse r = getAndCheckCommandExecutionPath("!2 A1S\n", "!2A1S");
        assertThat(r.getResponseSequence().getResponseFieldValue()).isEqualTo(2);
        assertThat(r.hasAddress()).isFalse();
    }

    @Test
    public void shouldParseCompoundAddressedResponse() {
        CompleteAddressedResponse r = getAndCheckCommandExecutionPath("@3.2 ! A1S\n", "!A1S");
        assertThat(r.getAddressSection(0)).isEqualTo(ZscriptAddress.from(3, 2));
        assertThat(r.hasAddress()).isTrue();
        assertThat(r.getAddressSectionCount()).isEqualTo(1);
        assertThatThrownBy(() -> r.getAddressSection(2))
                .isInstanceOf(IndexOutOfBoundsException.class).hasMessageContaining("out of bounds");
    }

    @Test
    public void shouldParseMultipleCompoundAddressedResponse() {
        CompleteAddressedResponse r = getAndCheckCommandExecutionPath("@3.2 @6.7 ! A1S\n", "!A1S");
        assertThat(r.getAddressSection(0)).isEqualTo(ZscriptAddress.from(3, 2));
        assertThat(r.getAddressSection(1)).isEqualTo(ZscriptAddress.from(6, 7));
        assertThat(r.getAddressSectionCount()).isEqualTo(2);
        assertThat(r.hasAddress()).isTrue();
    }

    @Test
    public void shouldParseEmptyResponse() {
        CompleteAddressedResponse r = getAndCheckCommandExecutionPath("!4 \n", "!4");
        assertThat(r.getResponseSequence().getResponseFieldValue()).isEqualTo(4);
        assertThat(r.hasAddress()).isFalse();

        final List<ResponseElement> responses = r.getResponseSequence().getExecutionPath().getResponses();
        assertThat(responses).hasSize(1);
        assertThat(responses.get(0).getNext()).isNull();
    }

    @Test
    public void shouldParseAndedEmptyResponses() {
        CompleteAddressedResponse r = getAndCheckCommandExecutionPath("!6 & \n", "!6&");
        assertThat(r.getResponseSequence().getResponseFieldValue()).isEqualTo(6);
        assertThat(r.hasAddress()).isFalse();
    }

    @Test
    public void shouldParseEmptyResponsesWithAddress() {
        CompleteAddressedResponse r = getAndCheckCommandExecutionPath("@12 !", "!");
        assertThat(r.getResponseSequence().getResponseFieldValue()).isEqualTo(0);
        assertThat(r.hasAddress()).isTrue();
        assertThat(r.getAddressSection(0)).isEqualTo(ZscriptAddress.from(0x12));
    }

    @Test
    public void shouldFailResponseWithTokenizerError() {
        assertThatThrownBy(() -> {
            final TokenBuffer.TokenReader.ReadToken token = tokenize(byteStringUtf8("!2 A\""), true).getTokenReader().getFirstReadToken();
            CompleteAddressedResponse.parse(token);
        }).isInstanceOf(ZscriptParseException.class).hasMessageContaining("Tokenizer error");
    }

    @Test
    public void shouldFailIncompleteResponse() {
        assertThatThrownBy(() -> {
            final TokenBuffer.TokenReader.ReadToken token = tokenize(byteStringUtf8("!2 \""), false).getTokenReader().getFirstReadToken();
            CompleteAddressedResponse.parse(token);
        }).isInstanceOf(ZscriptParseException.class).hasMessageContaining("no terminating sequence");
    }

    @Test
    public void shouldFailResponseWithoutResponseMark() {
        assertThatThrownBy(() -> {
            getAndCheckCommandExecutionPath("@1", "");
        }).isInstanceOf(ZscriptParseException.class).hasMessageContaining("without response marker");
    }

    private static CompleteAddressedResponse getAndCheckCommandExecutionPath(String cmds, String expectedSequence) {
        TokenBuffer.TokenReader.ReadToken token   = tokenize(byteStringUtf8(cmds), true).getTokenReader().getFirstReadToken();
        CompleteAddressedResponse         cmdPath = CompleteAddressedResponse.parse(token);
        assertThat(cmdPath.getResponseSequence().asStringUtf8()).isEqualTo(expectedSequence);
        return cmdPath;
    }
}
