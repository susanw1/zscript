package net.zscript.javaclient.commandpaths;

import java.util.List;

import static java.util.stream.Collectors.toList;
import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.CsvSource;

import net.zscript.tokenizer.TokenBuffer;
import net.zscript.util.ByteString;
import net.zscript.util.OptIterator;

class ResponseExecutionPathTest {
    @ParameterizedTest
    @CsvSource({
            "'',                                ''",
            "SA1,                               A1S",
            "BaA01S,                             A1BaS",
            "AS & BS,                             AS&BS",
            "AS | BS,                       AS|BS",
            "A+ab | B,                    A+ab|B",
            "A,                                 A",
            "A & B,                             A&B",
            "A | B,                             A|B",
            "(A | B) & C,               (A|B)&C",
            "(A | B) & (C | D),         (A|B)&(C|D)",
    })
    public void shouldParseSimpleResponseSequence(String resps, String encoded) {
        getAndCheckResponseExecutionPath(resps, encoded);
    }

    @Test
    public void shouldLinkResponses() {
        final ResponseExecutionPath respPath = getAndCheckResponseExecutionPath("AS & BS2", "AS&BS2");

        final ResponseElement responseA = respPath.getFirstResponse();
        assertThat(responseA).isNotNull().extracting(ByteString.ByteAppendable::asStringUtf8).isEqualTo("AS");
        final ResponseElement responseB = responseA.getNext();
        assertThat(responseB).isNotNull().extracting(ByteString.ByteAppendable::asStringUtf8).isEqualTo("BS2");

        assertThat(responseA.wasSuccess()).isTrue();
        // TODO - check if this should be false? Response status indicates it failed, but there's no '|' as it's the last response.
        assertThat(responseB.wasSuccess()).isTrue();
    }

    @Test
    public void shouldGetAllResponses() {
        final ResponseExecutionPath respPath1     = getAndCheckResponseExecutionPath("AS & BS", "AS&BS");
        final List<ResponseElement> responseList1 = OptIterator.of(respPath1).stream().collect(toList());
        assertThat(responseList1).hasSize(2);

        final ResponseExecutionPath respPath2     = getAndCheckResponseExecutionPath("(AS2 | BS) & (CS2 | DS2)", "(AS2|BS)&(CS2|DS2)");
        final List<ResponseElement> responseList2 = OptIterator.of(respPath2).stream().collect(toList());
        assertThat(responseList2).hasSize(8);
    }

    @Test
    public void shouldSupportToString() {
        TokenBuffer.TokenReader.ReadToken token    = tokenize(byteStringUtf8("A1+abB2"), true).getTokenReader().getFirstReadToken();
        ResponseExecutionPath             respPath = ResponseExecutionPath.parse(token);
        assertThat(respPath).hasToString("ResponseExecutionPath:{'A1B2+ab'}");
    }

    private static ResponseExecutionPath getAndCheckResponseExecutionPath(String resps, String expected) {
        TokenBuffer.TokenReader.ReadToken token    = tokenize(byteStringUtf8(resps), true).getTokenReader().getFirstReadToken();
        ResponseExecutionPath             respPath = ResponseExecutionPath.parse(token);
        assertThat(respPath.asStringUtf8()).isEqualTo(expected);
        return respPath;
    }

}
