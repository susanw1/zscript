package net.zscript.tokenizer;

import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.TokenBuffer.TokenWriter;

/**
 * General Tokenizer for handling a stream of incoming Zscript bytes. Responsibilities:
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
 * is alerted by writing error tokens. There are two approaches to processing a byte of zscript input with respect to token buffer capacity, depending on whether the source of
 * bytes is unbuffered or buffered.
 * <p/>
 * <h4>Unbuffered Channels</h4>
 *
 * If a channel is feeding unbuffered chars into the tokenizer, then running out of tokenizer buffer is fatal to the current sequence: chars are doomed to be dropped. This might
 * reflect an I2C input being read in an interrupt handler, and if the current char can't be accepted, then there's nowhere else to store it.
 * <p/>
 * In this case, the channel should call {@link #accept(byte)}, which will create the token or write an OVERRUN marker. Consequently, we must mark the buffer as OVERRUN at that
 * point, so that the executor gets signalled asap to stop executing commands in the sequence and to flag the error.
 * <p/>
 * <h4>Buffered</h4>
 * <p/>
 * If a buffered channel is feeding chars in, then it needs to be able to tell (up front) when its char won't fit, so it can avoid taking the char from its buffer and perhaps wait
 * for some command processing to happen. This might reflect a UDP channel or a hardware buffered serial port, where the input chars are happy where they are for now. Once some
 * commands in a sequence have been processed, there may be more room.
 * <p/>
 * Even in the buffered channel case, there may come a point where either the channel's data *must* be flushed before it was able to be presented, in which case {@link #dataLost()}
 * should be called to mark the sequence as OVERRUN.
 * <p/>
 * If an incoming single command is too long for the buffer, then it will become OVERRUN no matter what.
 */
public class Tokenizer {
    // Token for whole of rest of sequence following an address, representing the message to be passed downstream.
    public static final byte ADDRESSING_FIELD_KEY = (byte) 0x80;

    // Normal max length of a numeric field's value (eg 0-0xffff)
    public static final int DEFAULT_MAX_NUMERIC_BYTES = 2;

    /// //////////// Sequence marker values (top 4 bits set) - signify sequence end (either normal, or error during tokenization)

    public static final byte NORMAL_SEQUENCE_END              = (byte) 0xf0;
    public static final byte ERROR_BUFFER_OVERRUN             = (byte) 0xf1;
    public static final byte ERROR_CODE_ODD_BIGFIELD_LENGTH   = (byte) 0xf2;
    public static final byte ERROR_CODE_FIELD_TOO_LONG        = (byte) 0xf3;
    public static final byte ERROR_CODE_STRING_NOT_TERMINATED = (byte) 0xf4;
    public static final byte ERROR_CODE_STRING_ESCAPING       = (byte) 0xf5;
    public static final byte ERROR_CODE_ILLEGAL_TOKEN         = (byte) 0xf6;

    /// //////////// Normal marker values (top 3 bits set) - signify command/response end, implies read processing may proceed

    public static final byte CMD_END_ANDTHEN     = (byte) 0xe1;
    public static final byte CMD_END_ORELSE      = (byte) 0xe2;
    public static final byte CMD_END_OPEN_PAREN  = (byte) 0xe3;
    public static final byte CMD_END_CLOSE_PAREN = (byte) 0xe4;

    private final TokenWriter writer;
    private final int         maxNumericBytes;
    private final boolean     parseOutAddressing;

    private boolean skipToNL;
    private boolean bufferOvr;
    private boolean tokenizerError;
    private boolean numeric;
    private boolean addressing;

    private boolean isText;
    private boolean isNormalString;
    private int     escapingCount; // 2 bit counter, from 2 to 0

    /**
     * @param writer             the writer to write tokens to
     * @param parseOutAddressing true if addressed text should be written as normal tokens ("client"-style), false if they should be bundled into a singled ADDRESSING_FIELD_KEY
     *                           token ("receiver"-style) ready for forwarding to its destination
     */
    public Tokenizer(final TokenWriter writer, final boolean parseOutAddressing) {
        this(writer, DEFAULT_MAX_NUMERIC_BYTES, parseOutAddressing);
    }

    /**
     * Creates a new Tokenizer to write chars into tokens on the supplied writer. There are two primary modes: "client"-mode, where all tokens are written normally, and
     * "receiver"-mode. In receiver-mode, everything after the first address (eg "@a.b.c") is written as a single large text token marked with {@link #ADDRESSING_FIELD_KEY}
     *
     * @param writer             the writer to write tokens to
     * @param maxNumericBytes    the width of the largest numeric token to write (beyond which we write ERROR_CODE_FIELD_TOO_LONG)
     * @param parseOutAddressing true if addressed text should be written as normal tokens ("client"-mode), false if they should be bundled into a singled ADDRESSING_FIELD_KEY
     *                           token ("receiver"-mode) ready for forwarding to its destination
     */
    public Tokenizer(final TokenWriter writer, final int maxNumericBytes, final boolean parseOutAddressing) {
        this.writer = writer;
        this.maxNumericBytes = maxNumericBytes;
        this.parseOutAddressing = parseOutAddressing;
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
        // A single char might a) write previous token's nibble, b) write the key+len of new token - hence 3.
        final int MOST_BYTES_REQUIRED_BY_ONE_CHAR = 3;
        return writer.checkAvailableCapacity(MOST_BYTES_REQUIRED_BY_ONE_CHAR);
    }

    /**
     * Requests to process a byte of Zscript input into the tokenizer buffer, if there's capacity. If the offer returns true, then the byte has been consumed; otherwise the byte
     * was rejected, and it should be kept so that it can be presented again.
     *
     * @param b the new byte of zscript input
     * @return true if the byte was processed, false otherwise
     */
    public boolean offer(final byte b) {
        if (checkCapacity() || writer.getFlags().isReaderBlocked()) {
            accept(b);
            return true;
        }
        return false;
    }

    /**
     * Process a byte of Zscript input into the tokenizer buffer.
     *
     * @param b the new byte of zscript input
     */
    public void accept(final byte b) {
        if (!isText && Zchars.shouldIgnore(b) || Zchars.alwaysIgnore(b)) {
            return;
        }

        if (bufferOvr || tokenizerError || skipToNL) {
            if (b == Zchars.Z_EOL_SYMBOL) {
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

        if (writer.isTokenComplete()) {
            startNewToken(b);
            return;
        }

        if (isText) {
            // "text" is broadly interpreted: we're pushing non-numeric bytes into a current token
            acceptText(b);
            return;
        }

        final byte hex = Zchars.parseHex(b);
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
            if (b == Zchars.Z_EOL_SYMBOL) {
                // interesting case: the error above could be caused by b==Z_EOL_SYMBOL, but we've written an error marker, so just reset and return
                resetFlags();
            }
            return;
        }

        startNewToken(b);
    }

    private void acceptText(final byte b) {
        if (b == Zchars.Z_EOL_SYMBOL) {
            if (isNormalString) {
                writer.fail(ERROR_CODE_STRING_NOT_TERMINATED);
            } else {
                writer.writeMarker(NORMAL_SEQUENCE_END);
            }
            // we've written some marker, so reset as per the newline:
            resetFlags();
        } else if (escapingCount > 0) {
            final byte hex = Zchars.parseHex(b);
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

    private void startNewToken(final byte b) {
        if (b == Zchars.Z_EOL_SYMBOL) {
            writer.writeMarker(NORMAL_SEQUENCE_END);
            resetFlags();
            return;
        }

        if (!parseOutAddressing && addressing && b != Zchars.Z_ADDRESSING_CONTINUE) {
            writer.startToken(ADDRESSING_FIELD_KEY, false);
            writer.continueTokenByte(b);
            addressing = false;
            isText = true;
            escapingCount = 0;
            isNormalString = false;
            return;
        }

        if (Zchars.isSeparator(b)) {
            byte marker = charToMarker(b);
            if (marker != 0) {
                writer.writeMarker(marker);
                return;
            }
            throw new IllegalStateException("Unknown separator: " + (char) b);
        }

        if (b == Zchars.Z_ADDRESSING) {
            addressing = true;
        }

        if (!Zchars.isAllowableKey(b)) {
            writer.fail(ERROR_CODE_ILLEGAL_TOKEN);
            tokenizerError = true;
            return;
        }

        numeric = !Zchars.isNonNumericalKey(b);
        isText = false;
        if (b == Zchars.Z_COMMENT) {
            skipToNL = true;
            return;
        }

        writer.startToken(b, numeric);

        if (b == Zchars.Z_BIGFIELD_QUOTED) {
            isText = true;
            isNormalString = true;
            escapingCount = 0;
        }
    }

    /**
     * Translates a byte character in the Zscript stream to a corresponding marker. Inverse function to {@link #markerToChar(byte)}.
     *
     * @param ch the character to map
     * @return the Tokenizer marker
     */
    public static byte charToMarker(final byte ch) {
        switch (ch) {
        case Zchars.Z_ANDTHEN:
            return CMD_END_ANDTHEN;
        case Zchars.Z_ORELSE:
            return CMD_END_ORELSE;
        case Zchars.Z_OPENPAREN:
            return CMD_END_OPEN_PAREN;
        case Zchars.Z_CLOSEPAREN:
            return CMD_END_CLOSE_PAREN;
        }
        return 0;
    }

    /**
     * Translates a marker to a corresponding byte character. Inverse function to {@link #charToMarker(byte)}.
     *
     * @param m the Tokenizer marker to map
     * @return the corresponding character
     */
    public static byte markerToChar(final byte m) {
        switch (m) {
        case CMD_END_ANDTHEN:
            return Zchars.Z_ANDTHEN;
        case CMD_END_ORELSE:
            return Zchars.Z_ORELSE;
        case CMD_END_OPEN_PAREN:
            return Zchars.Z_OPENPAREN;
        case CMD_END_CLOSE_PAREN:
            return Zchars.Z_CLOSEPAREN;
        }
        return 0;
    }
}
