package net.zscript.tokenizer;

import java.nio.charset.StandardCharsets;
import java.util.stream.Stream;

import static net.zscript.tokenizer.Tokenizer.NORMAL_SEQUENCE_END;
import static org.assertj.core.api.Assertions.fail;
import static org.mockito.ArgumentMatchers.anyBoolean;
import static org.mockito.ArgumentMatchers.anyByte;
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

import net.zscript.model.components.Zchars;

@ExtendWith(MockitoExtension.class)
class TokenizerDeepParseTest {
    @Mock
    private TokenBuffer             buffer;
    @Mock
    private TokenBuffer.TokenWriter writer;

    private Tokenizer tokenizer;

    private static final char END         = (char) NORMAL_SEQUENCE_END;
    private static final char AND         = (char) Tokenizer.CMD_END_ANDTHEN;
    private static final char OR          = (char) Tokenizer.CMD_END_ORELSE;
    private static final char OPEN_PAREN  = (char) Tokenizer.CMD_END_OPEN_PAREN;
    private static final char CLOSE_PAREN = (char) Tokenizer.CMD_END_CLOSE_PAREN;

    private static final char FIELD_TOO_LONG      = (char) Tokenizer.ERROR_CODE_FIELD_TOO_LONG;
    private static final char ODD_BIGFIELD_LENGTH = (char) Tokenizer.ERROR_CODE_ODD_BIGFIELD_LENGTH;

    private static final int CAPACITY_CHECK_LENGTH = 3;

    @BeforeEach
    void setUp() {
        tokenizer = new Tokenizer(writer, 2, true);
        when(writer.checkAvailableCapacity(CAPACITY_CHECK_LENGTH)).thenReturn(true);
    }

    @Test
    void shouldMarkOverrunOnDataLostOnce() {
        tokenizer.dataLost();
        Mockito.verify(writer).fail(Tokenizer.ERROR_BUFFER_OVERRUN);
        // do it again - should just record once
        Mockito.reset(writer);
        tokenizer.dataLost();
        Mockito.verify(writer, Mockito.never()).fail(Tokenizer.ERROR_BUFFER_OVERRUN);
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
                Arguments.of("Key with explicit zero", "A0", "tA--"),
                Arguments.of("Key 1-nibble value with leading zero", "A0a\n", "tA--nam" + END),
                Arguments.of("Key 2-nibble value with leading zero", "A0ab\n", "tA--nanbm" + END),
                Arguments.of("Key 3-nibble value with leading zero", "A0abc\n", "tA--nanbncm" + END),
                Arguments.of("Key 4-nibble value with leading zeros", "A000abcd\n", "tA------nanbncndm" + END));
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
                Arguments.of("Illegal low-value key check", "A5\f1\n", "tAn5f" + (char) Tokenizer.ERROR_CODE_ILLEGAL_TOKEN + "----"),
                Arguments.of("Illegal high-value key check x80", "A5\u0080a\n", "tAn5f" + (char) Tokenizer.ERROR_CODE_ILLEGAL_TOKEN + "------"),
                Arguments.of("Illegal high-value key check xf0", "A5ða\n", "tAn5f" + (char) Tokenizer.ERROR_CODE_ILLEGAL_TOKEN + "------"),
                Arguments.of("Illegal hex key check 'a'", "A5\"\"a\n", "tAn5s\"--f" + (char) Tokenizer.ERROR_CODE_ILLEGAL_TOKEN + "------"),
                Arguments.of("Illegal hex key check 'f'", "A5\"\"f\n", "tAn5s\"--f" + (char) Tokenizer.ERROR_CODE_ILLEGAL_TOKEN + "------"),
                Arguments.of("Illegal hex key check '7'", "A5\"\"7\n", "tAn5s\"--f" + (char) Tokenizer.ERROR_CODE_ILLEGAL_TOKEN + "------"),
                Arguments.of("Illegal hex key check '7' at start", "7\n", "f" + (char) Tokenizer.ERROR_CODE_ILLEGAL_TOKEN + "------"));
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
    public void shouldHandleBigFields(String desc, String zscript, String bufferActions) {
        validateZscriptActions(zscript, bufferActions);
    }

    private static Stream<Arguments> shouldHandleBigFields() {
        return Stream.of(
                Arguments.of("Bigfield hex", "A0 +1234\n", "tA----s+n1n2n3n4m" + END),
                Arguments.of("Bigfield string", "A1 \"hello\"\n", "tAn1--s\"bhbeblblbo--m" + END),
                Arguments.of("Bigfield string with nulls", "A1 \"\000hel\000lo\"\000\n", "tAn1--s\"--bhbebl--blbo----m" + END),
                Arguments.of("Bigfield string with escape", "A1 \"h=65llo\"\n", "tAn1--s\"bh--n6n5blblbo--m" + END),
                Arguments.of("Bigfield hex and string", "+12\"a\"\n", "s+n1n2s\"ba--m" + END),
                Arguments.of("Bigfield string not terminated", "\"a\nA\n", "s\"baf" + (char) Tokenizer.ERROR_CODE_STRING_NOT_TERMINATED + "tAm" + END + "--"),
                Arguments.of("Bigfield invalid escape", "\"h=6Ao\"A\nB\n", "s\"bh--n6f" + (char) Tokenizer.ERROR_CODE_STRING_ESCAPING + "--------tBm" + END));
    }

    @ParameterizedTest(name = "{0}: {1}")
    @MethodSource
    public void shouldHandleComment(String desc, String zscript, String bufferActions) {
        validateZscriptActions(zscript, bufferActions);
    }

    private static Stream<Arguments> shouldHandleComment() {
        return Stream.of(
                Arguments.of("Comment is discarded", "A0#abc&|+\"\nB\n", "tA------------------m" + END + "tBm" + END),
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

    /**
     * Simple way to test lots of possibilities: bufferActions is a little 2-char language describing, for each input char, the (op, arg) on the buffer.
     *
     * @param zscript       some zscript text to tokenize, but where 'n' is replaced by '\n'
     * @param bufferActions a pair of chars for each zscript char, so express expected method calls
     */
    private void validateZscriptActions(String zscript, String bufferActions) {
        if (zscript.length() * 2 > bufferActions.length()) {
            fail("bufferActions length is too short (trimmed?) + zscript='" + zscript + "', actions='" + bufferActions + "'");
        }
        int index = 0;
        // ISO8859 to ensure chars are fed as just bytes
        for (byte c : zscript.getBytes(StandardCharsets.ISO_8859_1)) {
            tokenizer.accept(c);
            //            System.out.println("c = " + c);
            char action = bufferActions.charAt(index * 2);
            byte arg    = (byte) bufferActions.charAt(index * 2 + 1);
            try {
                switch (action) {
                case 's':
                    Mockito.verify(writer).startToken(arg, false);
                    break;
                case 't':
                    Mockito.verify(writer).startToken(arg, true);
                    break;
                case 'n':
                    Mockito.verify(writer).continueTokenNibble((byte) Character.digit((char) arg, 16));
                    break;
                case 'b':
                    Mockito.verify(writer).continueTokenByte(arg);
                    break;
                case 'm':
                    Mockito.verify(writer).writeMarker(arg);
                    break;
                case 'f':
                    Mockito.verify(writer).fail(arg);
                    break;
                case 'a':
                    Mockito.verify(writer).startToken(Tokenizer.ADDRESSING_FIELD_KEY, false);
                    Mockito.verify(writer).continueTokenByte(arg);
                    break;
                case '-':
                    Mockito.verify(writer, Mockito.never()).startToken(anyByte(), anyBoolean());
                    Mockito.verify(writer, Mockito.never()).continueTokenNibble(anyByte());
                    Mockito.verify(writer, Mockito.never()).continueTokenByte(anyByte());
                    break;
                default:
                    fail("Unknown action! '" + action + "'");
                }
                Mockito.clearInvocations(writer);
            } catch (Throwable t) {
                throw new AssertionError("At index " + index + " processing char " + Byte.toUnsignedInt(c), t);
            }
            index++;
        }
    }

    @Test
    public void shouldAcceptSingleKeyAtTokenStart() {
        when(writer.isTokenComplete()).thenReturn(true);
        validateZscriptActions("Z\n", "tZm" + END);
    }

    // Expected pattern of length/inNibble for successive nibbles
    // Before: 0/false
    // 1: 1/true
    // 2: 1/false
    // 3: 2/true
    // 4: 2/false <-- this is the condition we want to detect, before 5th.
    // 5: 3/true
    @Test
    public void shouldFailOnLongNumber() {
        when(writer.getCurrentWriteTokenLength()).thenReturn(0).thenReturn(1).thenReturn(1).thenReturn(2).thenReturn(2).thenThrow(RuntimeException.class);
        when(writer.isInNibble()).thenReturn(true).thenReturn(false).thenThrow(RuntimeException.class);
        validateZscriptActions("Z12345\n\n", "tZn1n2n3n4f" + (char) Tokenizer.ERROR_CODE_FIELD_TOO_LONG + "--m" + END + "--");
    }

    @Test
    public void shouldFailOnOddHexString() {
        when(writer.getCurrentWriteTokenKey()).thenReturn(Zchars.Z_BIGFIELD_HEX);
        when(writer.isInNibble()).thenReturn(false).thenReturn(true).thenReturn(false).thenReturn(true).thenThrow(RuntimeException.class);
        validateZscriptActions("+123A\nB\n", "s+n1n2n3f" + (char) Tokenizer.ERROR_CODE_ODD_BIGFIELD_LENGTH + "--tBm" + END + "--");
    }

    @Test
    public void shouldFailOnOddHexStringTerminatedByNL() {
        when(writer.getCurrentWriteTokenKey()).thenReturn(Zchars.Z_BIGFIELD_HEX);
        when(writer.isInNibble()).thenReturn(false).thenReturn(true).thenReturn(false).thenReturn(true).thenThrow(RuntimeException.class);
        validateZscriptActions("+12abc\nA\n", "s+n1n2nanbncf" + (char) Tokenizer.ERROR_CODE_ODD_BIGFIELD_LENGTH + "tAm" + END + "--");
    }

}
