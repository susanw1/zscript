package org.zcode.javareceiver.tokenizer;

import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenWriter;

/**
 * General Tokenizer for handling a stream of incoming Zcode bytes. Responsibilities:
 * <ol>
 * <li>recording input into a Token Buffer</li>
 * <li>maintaining state at a Token level (eg are we in a string? hex nibbles? etc)</li>
 * <li>recording token-level errors</li>
 * <li>preventing buffer overruns</li>
 * <li>signalling when the execution system can attempt to progress</li>
 * </ol>
 * 
 * <h3>Token Buffer Overrun</h3>
 * 
 * If incoming characters arrive faster than they can be processed, or command-sequences are so long that they threaten to overrun the token buffer storage, then the execution side
 * is alerted by writing error tokens. There are two approaches to processing a byte of zcode input with respect to token buffer capacity, depending on whether the source of bytes
 * is unbuffered or buffered.
 * 
 * <h4>Unbuffered Channels</h4>
 * 
 * If an channel is feeding unbuffered chars into the tokenizer, then running out of tokenizer buffer is fatal to the current sequence: chars are doomed to be dropped. This might
 * reflect an I2C input being read in an interrupt handler, and if the current char can't be accepted, then there's nowhere else to store it.
 * 
 * In this case, the channel should call {@link #accept(byte)}, which will create the token or write an OVERRUN marker. Consequently, we must mark the buffer as OVERRUN at that
 * point, so that the executor gets signalled asap to stop executing commands in the sequence and to flag the error.
 * 
 * <h4>Buffered</h4>
 * 
 * If a buffered channel is feeding chars in, then it needs to be able to tell (up front) when its char won't fit, so it can avoid taking the char from its buffer and perhaps wait
 * for some command processing to happen. This might reflect a UDP channel or a hardware buffered serial port, where the input chars are happy where they are for now. Once some
 * commands in a sequence have been processed, there may be more room.
 * 
 * Even in the buffered channel case, there may come a point where either the channel's data *must* be flushed before it was able to be presented, in which case {@link #dataLost()}
 * should be called to mark the sequence as OVERRUN.
 * 
 * If an incoming single command is too long for the buffer, then it will become OVERRUN no matter what.
 * 
 * @param b the new byte of zcode input
 */
public class ZcodeTokenizer {
    public static final byte ADDRESSING_FIELD_KEY = (byte) 0x80;

    public static final byte NORMAL_SEQUENCE_END              = (byte) 0xf0;
    public static final byte ERROR_BUFFER_OVERRUN             = (byte) 0xf1;
    public static final byte ERROR_CODE_ODD_BIGFIELD_LENGTH   = (byte) 0xf2;
    public static final byte ERROR_CODE_FIELD_TOO_LONG        = (byte) 0xf3;
    public static final byte ERROR_CODE_STRING_NOT_TERMINATED = (byte) 0xf4;
    public static final byte ERROR_CODE_STRING_ESCAPING       = (byte) 0xf5;
    public static final byte ERROR_CODE_ILLEGAL_TOKEN         = (byte) 0xf6;

    public static final byte CMD_END_ANDTHEN     = (byte) 0xe1;
    public static final byte CMD_END_ORELSE      = (byte) 0xe2;
    public static final byte CMD_END_OPEN_PAREN  = (byte) 0xe3;
    public static final byte CMD_END_CLOSE_PAREN = (byte) 0xe4;

    private final static boolean DROP_COMMENTS = false;

    private final TokenWriter writer;
    private final int         maxNumericBytes;

    private boolean skipToNL;
    private boolean bufferOvr;
    private boolean tokenizerError;
    private boolean numeric;
    private boolean addressing;

    private boolean isText;
    private boolean isNormalString;
    private int     escapingCount; // 2 bit counter, from 2 to 0

    public ZcodeTokenizer(final TokenWriter writer, final int maxNumericBytes) {
        this.writer = writer;
        this.maxNumericBytes = maxNumericBytes;
        resetFlags();
    }

    private void resetFlags() {
        this.skipToNL = false;
        this.bufferOvr = false;
        this.tokenizerError = false;
        this.numeric = false;
        this.addressing = false;
        this.isText = false;
        this.isNormalString = false;
        this.escapingCount = 0;
    }

    /**
     * If a channel becomes aware that it has lost (or is about to lose) data, either because the channel has run out of buffer, or because the TokenBuffer is out of room, then it
     * can signal the Tokenizer to mark the current command sequence as overrun.
     */
    public void dataLost() {
        if (!bufferOvr) {
            writer.fail(ERROR_BUFFER_OVERRUN);
            bufferOvr = true;
        }
    }

    /**
     * Determine whether there is guaranteed capacity for another byte of input, in the worst case without knowing what that byte is.
     * 
     * @return true if capacity definitely exists, false otherwise.
     */
    public boolean checkCapacity() {
        // worst case is... TODO: review this!
        return writer.checkAvailableCapacity(3);
    }

    /**
     * TODO: Review: this seems a bit simple...
     * 
     * @param b
     * @return
     */
    public boolean offer(byte b) {
        if (checkCapacity() || writer.getFlags().isReaderBlocked()) {
            accept(b);
            return true;
        }
        return false;
    }
    // TODO: Discuss dataLost changes.

    /**
     * Process a byte of Zcode input into the parser.
     * 
     * @param b the new byte of zcode input
     */
    public void accept(byte b) {
        if (!isText && Zchars.shouldIgnore(b) || Zchars.alwaysIgnore(b)) {
            return;
        }

        if (bufferOvr || tokenizerError || skipToNL) {
            if (b == Zchars.Z_NEWLINE) {
                if (skipToNL) {
                    if (!checkCapacity()) {
                        dataLost();
                        return;
                    }
                    writer.writeMarker(NORMAL_SEQUENCE_END);
                }
                resetFlags();
            }
            return;
        }
        if (!checkCapacity()) {
            dataLost();
            return;
        }

        // TODO: Hysteresis on bufferOvr - review this approach given we're rewinding current token on failure marker
//        if (bufferOvr) {
//            if (!writer.checkAvailableCapacity(10)) {
//                return;
//            }
//            bufferOvr = false;
//        }

        if (writer.isTokenComplete()) {
            startNewToken(b);
            return;
        }

        if (isText) {
            // "text" is broadly interpreted: we're pushing non-numeric bytes into a current token
            acceptText(b);
            return;
        }

        byte hex = Zchars.parseHex(b);
        if (hex != Zchars.PARSE_NOT_HEX_0X10) {
            if (numeric) {
                // Check field length
                final int currentLength = writer.getCurrentWriteTokenLength();
                if (currentLength == maxNumericBytes && !writer.isInNibble()) {
                    writer.fail(ERROR_CODE_FIELD_TOO_LONG);
                    tokenizerError = true;
                    return;
                }
                // Skip leading zeros
                if (currentLength == 0 && hex == 0) {
                    return;
                }
            }
            writer.continueTokenNibble(hex);
            return;
        }

        // Check big field odd length
        if (!numeric && writer.getCurrentWriteTokenKey() == Zchars.Z_BIGFIELD_HEX && writer.isInNibble()) {
            writer.fail(ERROR_CODE_ODD_BIGFIELD_LENGTH);
            tokenizerError = true;
            if (b == Zchars.Z_NEWLINE) {
                // interesting case: the error above could be caused by b==Z_NEWLINE, but we've written an error marker, so just reset and return
                resetFlags();
            }
            return;
        }

        startNewToken(b);
    }

    private void acceptText(byte b) {
        if (b == Zchars.Z_NEWLINE) {
            if (isNormalString) {
                writer.fail(ERROR_CODE_STRING_NOT_TERMINATED);
            } else {
                writer.writeMarker(NORMAL_SEQUENCE_END);
            }
            // we've written some marker, so reset as per the newline:
            resetFlags();
        } else if (escapingCount > 0) {
            byte hex = Zchars.parseHex(b);
            if (hex == Zchars.PARSE_NOT_HEX_0X10) {
                writer.fail(ERROR_CODE_STRING_ESCAPING);
                tokenizerError = true;
            } else {
                writer.continueTokenNibble(hex);
                escapingCount--;
            }
        } else if (isNormalString && b == Zchars.Z_BIGFIELD_QUOTED) {
            writer.endToken();
            isText = false;
        } else if (isNormalString && b == Zchars.Z_STRING_ESCAPE) {
            escapingCount = 2;
        } else {
            writer.continueTokenByte(b);
        }
    }

    private void startNewToken(byte b) {
        if (b == Zchars.Z_NEWLINE) {
            writer.writeMarker(NORMAL_SEQUENCE_END);
            resetFlags();
            return;
        }

        if (addressing && b != Zchars.Z_ADDRESSING_CONTINUE) {
            writer.startToken(ADDRESSING_FIELD_KEY, false);
            writer.continueTokenByte(b);
            addressing = false;
            isText = true;
            escapingCount = 0;
            isNormalString = false;
            return;
        }

        if (Zchars.isSeparator(b)) {
            byte marker = 0;
            switch (b) {
            case Zchars.Z_ANDTHEN:
                marker = CMD_END_ANDTHEN;
                break;
            case Zchars.Z_ORELSE:
                marker = CMD_END_ORELSE;
                break;
            case Zchars.Z_OPENPAREN:
                marker = CMD_END_OPEN_PAREN;
                break;
            case Zchars.Z_CLOSEPAREN:
                marker = CMD_END_CLOSE_PAREN;
                break;
            // more for other constructs? '(', ')'
            }
            if (marker != 0) {
                writer.writeMarker(marker);
                return;
            } else {
                throw new IllegalStateException("Unknown seperator: " + (char) b);
            }
        }

        if (b == Zchars.Z_ADDRESSING) {
            addressing = true;
        }

        if (!Zchars.isAllowableKey(b)) {
            writer.fail(ERROR_CODE_ILLEGAL_TOKEN);
            tokenizerError = true;
            return;
        }

        numeric = !Zchars.isNonNumerical(b);
        isText = false;
        writer.startToken(b, numeric);

        if (b == Zchars.Z_COMMENT) {
            if (DROP_COMMENTS) {
                skipToNL = true;
            } else {
                isText = true;
                isNormalString = false;
                escapingCount = 0;
            }
            return;
        }
        if (b == Zchars.Z_BIGFIELD_QUOTED) {
            isText = true;
            isNormalString = true;
            escapingCount = 0;
            return;
        }
    }
}
