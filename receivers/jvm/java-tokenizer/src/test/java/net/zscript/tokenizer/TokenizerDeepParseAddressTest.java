package net.zscript.tokenizer;

import java.util.stream.Stream;

import static net.zscript.tokenizer.Tokenizer.ERROR_CODE_ILLEGAL_TOKEN;
import static net.zscript.tokenizer.Tokenizer.NORMAL_SEQUENCE_END;
import static org.assertj.core.api.Assertions.assertThat;
import static org.mockito.Mockito.lenient;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.junit.jupiter.MockitoExtension;

/**
 * This test is almost identical to TokenizerTest, except it's in "client mode", where we tokenize the contents of addressed messages, rather than receiver-mode: sticking them all
 * in a single byte-string. Tests added here should also be added there.
 */
@ExtendWith(MockitoExtension.class)
class TokenizerDeepParseAddressTest {
    // determines that we parse addressed data
    private static final boolean PARSE_OUT_ADDRESSING_TRUE = true;

    @Mock
    private TokenBuffer.TokenWriter writer;

    private Tokenizer tokenizer;

    private static final char END         = (char) NORMAL_SEQUENCE_END;
    private static final char AND         = (char) Tokenizer.CMD_END_ANDTHEN;
    private static final char OR          = (char) Tokenizer.CMD_END_ORELSE;
    private static final char OPEN_PAREN  = (char) Tokenizer.CMD_END_OPEN_PAREN;
    private static final char CLOSE_PAREN = (char) Tokenizer.CMD_END_CLOSE_PAREN;

    private static final char FIELD_TOO_LONG           = (char) Tokenizer.ERROR_CODE_FIELD_TOO_LONG;
    private static final char ODD_HEXPAIR_FIELD_LENGTH = (char) Tokenizer.ERROR_CODE_ODD_HEXPAIR_LENGTH;

    private static final int CAPACITY_CHECK_LENGTH = 3;

    @BeforeEach
    void setUp() {
        tokenizer = new Tokenizer(writer, PARSE_OUT_ADDRESSING_TRUE);
        lenient().when(writer.checkAvailableCapacity(CAPACITY_CHECK_LENGTH)).thenReturn(true);
    }

    @Test
    void shouldMarkOverrunOnDataLostOnce() {
        tokenizer.dataLost();
        verify(writer).fail(Tokenizer.ERROR_BUFFER_OVERRUN);
        // do it again - should just record once
        Mockito.reset(writer);
        tokenizer.dataLost();
        verify(writer, Mockito.never()).fail(Tokenizer.ERROR_BUFFER_OVERRUN);
    }

    @ParameterizedTest(name = "{0}: {1}")
    @MethodSource
    public void shouldHandleSimpleNumericFields(String desc, String zscript, String bufferActions) {
        validateZscriptActions(zscript, bufferActions);
    }

    private static Stream<Arguments> shouldHandleSimpleNumericFields() {
        return Stream.of(
                Arguments.of("Single zero-valued key, no NL", "Z", "tZ"),
                Arguments.of("Single zero-valued key with NL", "Z\n", "tZm" + END),
                Arguments.of("3 empty keys", "A A A\n", "tA--tA--tAm" + END),
                Arguments.of("3 keys ", "AA1Afa\n", "tAtAn1tAnfnam" + END),
                Arguments.of("Key with 2-nibble value, spaced", "A 12", "tA--n1n2"),
                Arguments.of("Key with explicit zero", "A0", "tAn0"),
                Arguments.of("Key 1-nibble value with leading zero", "A0a\n", "tAn0nam" + END),
                Arguments.of("Key 2-nibble value with leading zero", "A0ab\n", "tAn0nanbm" + END),
                Arguments.of("Key 3-nibble value with leading zero", "A0abc\n", "tAn0nanbncm" + END),
                Arguments.of("Key 4-nibble value with leading zeros", "A000abcd\n", "tAn0n0n0nanbncndm" + END));
    }

    @ParameterizedTest(name = "{0}: {1}")
    @MethodSource
    public void shouldRejectInvalidKeys(String desc, String zscript, String bufferActions) {
        when(writer.isTokenComplete()).thenReturn(true, false, true, false, true);
        validateZscriptActions(zscript, bufferActions);
    }

    private static Stream<Arguments> shouldRejectInvalidKeys() {
        // Note, all these go bad-key,value or key,value,bad-key or key,value,"",bad-key ... to fit with the test's isTokenComplete setup
        return Stream.of(
                Arguments.of("Illegal low-value key check", "A5\f1\n", "tAn5f" + (char) ERROR_CODE_ILLEGAL_TOKEN + "----"),
                Arguments.of("Illegal high-value key check x80", "A5\u0080a\n", "tAn5f" + (char) ERROR_CODE_ILLEGAL_TOKEN + "------"),
                Arguments.of("Illegal high-value key check xf0", "A5ða\n", "tAn5f" + (char) ERROR_CODE_ILLEGAL_TOKEN + "------"),
                Arguments.of("Illegal hex key check 'a'", "A5\"\"a\n", "tAn5t\"--f" + (char) ERROR_CODE_ILLEGAL_TOKEN + "------"),
                Arguments.of("Illegal hex key check 'f'", "A5\"\"f\n", "tAn5t\"--f" + (char) ERROR_CODE_ILLEGAL_TOKEN + "------"),
                Arguments.of("Illegal hex key check '7'", "A5\"\"7\n", "tAn5t\"--f" + (char) ERROR_CODE_ILLEGAL_TOKEN + "------"),
                Arguments.of("Illegal hex key check '7' at start", "7\n", "f" + (char) ERROR_CODE_ILLEGAL_TOKEN + "------"));
    }

    @ParameterizedTest(name = "{0}: {1}")
    @MethodSource
    public void shouldHandleLogicalSequencesOfNumericFields(String desc, String zscript, String bufferActions) {
        validateZscriptActions(zscript, bufferActions);
    }

    private static Stream<Arguments> shouldHandleLogicalSequencesOfNumericFields() {
        return Stream.of(
                Arguments.of("Two keys sep with &", "Y&Z\n", "tYm" + AND + "tZm" + END),
                Arguments.of("Two keys sep with |", "Y|Z\n", "tYm" + OR + "tZm" + END),
                Arguments.of("Several keys sep with several & and |", "A|B&C|D\n", "tAm" + OR + "tBm" + AND + "tCm" + OR + "tDm" + END),
                Arguments.of("Two keys sep with (", "Y(Z\n", "tYm" + OPEN_PAREN + "tZm" + END),
                Arguments.of("Two keys sep with )", "Y)Z\n", "tYm" + CLOSE_PAREN + "tZm" + END),
                Arguments.of("Several keys sep with &, |, ( and )", "A(B|C)D&E\n", "tAm" + OPEN_PAREN + "tBm" + OR + "tCm" + CLOSE_PAREN + "tDm" + AND + "tEm" + END));
    }

    @ParameterizedTest(name = "{0}: {1}")
    @MethodSource
    public void shouldHandleStringFields(String desc, String zscript, String bufferActions) {
        validateZscriptActions(zscript, bufferActions);
    }

    private static Stream<Arguments> shouldHandleStringFields() {
        return Stream.of(
                Arguments.of("Stringfield hex", "A0 B1234\n", "tAn0--tBn1n2n3n4m" + END),
                Arguments.of("Stringfield string", "A1 B\"hello\"\n", "tAn1--tB--bhbeblblbo--m" + END),
                Arguments.of("Stringfield string with nulls", "A1 B\"\000hel\000lo\"\000\n", "tAn1--tB----bhbebl--blbo----m" + END),
                Arguments.of("Stringfield string with escape", "A1 B\"h=65llo\"\n", "tAn1--tB--bh--n6n5blblbo--m" + END),
                Arguments.of("Stringfield hex and string", "B12C\"a\"\n", "tBn1n2tC--ba--m" + END),
                Arguments.of("Stringfield string not terminated", "B\"a\nA\n", "tB--baf" + (char) Tokenizer.ERROR_CODE_STRING_NOT_TERMINATED + "tAm" + END + "--"),
                Arguments.of("Stringfield invalid escape", "C\"h=6Ao\"A\nB\n", "tC--bh--n6f" + (char) Tokenizer.ERROR_CODE_STRING_ESCAPING + "--------tBm" + END));
    }

    @ParameterizedTest(name = "{0}: {1}")
    @MethodSource
    public void shouldHandleComment(String desc, String zscript, String bufferActions) {
        validateZscriptActions(zscript, bufferActions);
    }

    private static Stream<Arguments> shouldHandleComment() {
        return Stream.of(
                Arguments.of("Comment is discarded", "A0#abc&|+\"\nB\n", "tAn0----------------m" + END + "tBm" + END),
                Arguments.of("Empty comment", "#\nA\n", "--m" + END + "tAm" + END));
    }

    @ParameterizedTest(name = "{0}: {1}")
    @MethodSource
    public void shouldHandleAddressing(String desc, String zscript, String bufferActions) {
        validateZscriptActions(zscript, bufferActions);
    }

    private static Stream<Arguments> shouldHandleAddressing() {
        return Stream.of(
                Arguments.of("Simple address", "@2Z\n", "t@n2tZm" + END),
                Arguments.of("Simple address with nulls", "\000@\000a\000Z\000\n", "--t@--na--tZ--m" + END),
                Arguments.of("Simple address, complex content", "@2Z1234A123B23\n", "t@n2tZn1n2n3n4tAn1n2n3tBn2n3m" + END),
                Arguments.of("Multilevel address", "@2.123Z\n", "t@n2t.n1n2n3tZm" + END),
                Arguments.of("Compound multilevel addresses", "@2.3@4.5Z\n", "t@n2t.n3t@n4t.n5tZm" + END));
    }

    @Test
    public void shouldAcceptSingleKeyAtTokenStart() {
        when(writer.isTokenComplete()).thenReturn(true);
        validateZscriptActions("Z\n", "tZm" + END);
    }

    @Test
    public void shouldTokenizeFullSizeNumber() {
        TokenRingBuffer buf = TokenRingBuffer.createBufferWithCapacity(128);
        Tokenizer       tok = new Tokenizer(buf.getTokenWriter(), PARSE_OUT_ADDRESSING_TRUE);
        "Z1234\n\n".chars().forEachOrdered(c -> tok.accept((byte) c));
        // note second newline!
        assertThat(buf.getInternalData()).startsWith('Z', 2, 0x12, 0x34, 0xf0, 0xf0, 0);
    }

    @Test
    public void shouldFailOnLongNumber() {
        TokenRingBuffer buf = TokenRingBuffer.createBufferWithCapacity(128);
        Tokenizer       tok = new Tokenizer(buf.getTokenWriter(), PARSE_OUT_ADDRESSING_TRUE);
        "A12345\n\n".chars().forEachOrdered(c -> tok.accept((byte) c));
        // note second newline!
        assertThat(buf.getInternalData()).startsWith(ODD_HEXPAIR_FIELD_LENGTH, 0xf0, 0x12, 0x34, 0x50, 0);
    }

    @Test
    public void shouldFailOnOddHexStringField() {
        TokenRingBuffer buf = TokenRingBuffer.createBufferWithCapacity(128);
        Tokenizer       tok = new Tokenizer(buf.getTokenWriter(), PARSE_OUT_ADDRESSING_TRUE);
        "A12345B\n".chars().forEachOrdered(c -> tok.accept((byte) c));
        assertThat(buf.getInternalData()).startsWith(ODD_HEXPAIR_FIELD_LENGTH, 3, 0x12, 0x34, 0x50, 0);

        "C\n".chars().forEachOrdered(c -> tok.accept((byte) c));
        assertThat(buf.getInternalData()).startsWith(ODD_HEXPAIR_FIELD_LENGTH, 'C', 0, 0xf0, 0x50, 0);
    }

    @Test
    public void shouldFailOnOddHexStringFieldTerminatedByNL() {
        TokenRingBuffer buf = TokenRingBuffer.createBufferWithCapacity(128);
        Tokenizer       tok = new Tokenizer(buf.getTokenWriter(), PARSE_OUT_ADDRESSING_TRUE);
        "A12abc\n".chars().forEachOrdered(c -> tok.accept((byte) c));
        assertThat(buf.getInternalData()).startsWith(ODD_HEXPAIR_FIELD_LENGTH, 3, 0x12, 0xab, 0xc0, 0);

        "B\n".chars().forEachOrdered(c -> tok.accept((byte) c));
        assertThat(buf.getInternalData()).startsWith(ODD_HEXPAIR_FIELD_LENGTH, 'B', 0, 0xf0, 0xc0, 0);
    }

    @Test
    public void shouldResetHexModeAfterHexToken() {
        TokenRingBuffer buf = TokenRingBuffer.createBufferWithCapacity(128);
        Tokenizer       tok = new Tokenizer(buf.getTokenWriter(), PARSE_OUT_ADDRESSING_TRUE);
        "A12 B345\n".chars().forEachOrdered(c -> tok.accept((byte) c));
        assertThat(buf.getInternalData()).startsWith('A', 1, 0x12, 'B', 2, 0x3, 0x45, 0xf0);

        "C\n".chars().forEachOrdered(c -> tok.accept((byte) c));
        assertThat(buf.getInternalData()).startsWith('A', 1, 0x12, 'B', 2, 0x3, 0x45, 0xf0, 'C', 0, 0xf0, 0);
    }

    @Test
    public void shouldResetStringModeAfterStringToken() {
        TokenRingBuffer buf = TokenRingBuffer.createBufferWithCapacity(128);
        Tokenizer       tok = new Tokenizer(buf.getTokenWriter(), PARSE_OUT_ADDRESSING_TRUE);
        "A\"x\" B345\n".chars().forEachOrdered(c -> tok.accept((byte) c));
        assertThat(buf.getInternalData()).startsWith('A', 1, 'x', 'B', 2, 0x3, 0x45, 0xf0);

        "C\n".chars().forEachOrdered(c -> tok.accept((byte) c));
        assertThat(buf.getInternalData()).startsWith('A', 1, 'x', 'B', 2, 0x3, 0x45, 0xf0, 'C', 0, 0xf0, 0);
    }

    @Test
    public void shouldRejectHexAfterStringToken() {
        TokenRingBuffer buf = TokenRingBuffer.createBufferWithCapacity(128);
        Tokenizer       tok = new Tokenizer(buf.getTokenWriter(), PARSE_OUT_ADDRESSING_TRUE);
        "A\"x\"12 B345\n".chars().forEachOrdered(c -> tok.accept((byte) c));
        assertThat(buf.getInternalData()).startsWith('A', 1, 'x', ERROR_CODE_ILLEGAL_TOKEN);
    }

    @Test
    public void shouldRejectOverLongAddressComponents() {
        TokenRingBuffer buf = TokenRingBuffer.createBufferWithCapacity(128);
        Tokenizer       tok = new Tokenizer(buf.getTokenWriter(), PARSE_OUT_ADDRESSING_TRUE);
        "@12345\n".chars().forEachOrdered(c -> tok.accept((byte) c));
        assertThat(buf.getInternalData()).startsWith(FIELD_TOO_LONG, 2, 0x12, 0x34, 0);
    }

    @Test
    public void shouldRejectOverLongAddressSubComponents() {
        TokenRingBuffer buf = TokenRingBuffer.createBufferWithCapacity(128);
        Tokenizer       tok = new Tokenizer(buf.getTokenWriter(), PARSE_OUT_ADDRESSING_TRUE);
        "@1234.12345\n".chars().forEachOrdered(c -> tok.accept((byte) c));
        assertThat(buf.getInternalData()).startsWith('@', 2, 0x12, 0x34, FIELD_TOO_LONG, 2, 0x12, 0x34, 0);
    }

    @Test
    public void shouldRejectOverLongEcho() {
        TokenRingBuffer buf = TokenRingBuffer.createBufferWithCapacity(128);
        Tokenizer       tok = new Tokenizer(buf.getTokenWriter(), PARSE_OUT_ADDRESSING_TRUE);
        ">12345Z\n".chars().forEachOrdered(c -> tok.accept((byte) c));
        assertThat(buf.getInternalData()).startsWith(FIELD_TOO_LONG, 2, 0x12, 0x34, 0);
    }

    /**
     * Simple way to test lots of possibilities: bufferActions is a little 2-char language describing, for each input char, the (op, arg) on the buffer.
     *
     * @param zscript       some zscript text to tokenize, but where 'n' is replaced by '\n'
     * @param bufferActions a pair of chars for each zscript char, so express expected method calls
     */
    private void validateZscriptActions(String zscript, String bufferActions) {
        TokenizerTest.validateZscriptActions(tokenizer, writer, zscript, bufferActions);
    }

}
