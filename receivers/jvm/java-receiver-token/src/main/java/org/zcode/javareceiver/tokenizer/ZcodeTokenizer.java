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
    public static final byte ERROR_CODE_ODD_BIGFIELD_LENGTH   = (byte) 0xF1;
    public static final byte ERROR_CODE_FIELD_TOO_LONG        = (byte) 0xF2;
    public static final byte ERROR_CODE_STRING_NOT_TERMINATED = (byte) 0xF3;
    public static final byte ERROR_CODE_STRING_ESCAPING       = (byte) 0xF4;

    public static final byte ADDRESSING_FIELD_KEY = (byte) 0x80;

    private final static boolean DROP_COMMENTS = false;

    private final TokenWriter writer;

    private boolean skipToNL;
    private boolean bufferOvr;
    private boolean numeric;
    private boolean addressing;

    private boolean isText;
    private boolean isNormalString;
    private int     escapingCount; // 2 bit counter, from 2 to 0

    private ZcodeTokenizer(final TokenWriter writer) {
        this.writer = writer;

        this.skipToNL = false;
        this.bufferOvr = false;
        this.numeric = false;
        this.addressing = false;
        this.isText = false;
        this.isNormalString = false;
        this.escapingCount = 0;
    }

    // TODO: Give error on key with top bit set

    /**
     * If a channel becomes aware that it has lost (or is about to lose) data, either because the channel has run out of buffer, or because the TokenBuffer is out of room, then it
     * can signal the Tokenizer to mark the current command sequence as overrun.
     */
    public void dataLost() {
        if (!bufferOvr) {
            writer.fail(ZcodeTokenBuffer.BUFFER_OVERRUN_ERROR);
            bufferOvr = true;
            skipToNL = true;
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
     * 
     * @param b
     * @return
     */
    public boolean offer(byte b) {
        // TODO
        return true;
    }

    /**
     * There are two modes of processing a byte of zcode input with respect to overrun conditions.
     * 
     * 1) If an unbuffered (or minimally buffered) channel is feeding chars into the tokenizer, then running out of tokenizer buffer is fatal to the current sequence: we're going
     * to drop chars. Consequently, we must mark the buffer as OVERRUN at that point, so that the reader gets signalled asap to stop processing and eg return error.
     * 
     * 2) If a buffered channel is feeding chars in, then it needs to be able to tell (up front) when its char won't fit, so it can avoid taking the char from its buffer and
     * perhaps wait for some command processing to happen. Once some commands in a sequence have been processed, there may be more room.
     * 
     * 3) Even in the buffered channel case, there may come a point where either the channel's data *must* be flushed before it was able to be presented, in which case
     * {@link #dataLost()} should be called to mark the sequence as OVERRUN.
     * 
     * 4) If an incoming single command is too long for the buffer, then it will become OVERRUN no matter what.
     * 
     * @param b the new byte of zcode input
     */
    void accept(byte b) {
        if (!isText && Zchars.shouldIgnore(b)) {
            return;
        }

        if (bufferOvr || skipToNL && b != Zchars.Z_NEWLINE) {
            return;
        }

        if (bufferOvr) {
            if (!writer.checkAvailableCapacity(10)) {
                return;
            }
            bufferOvr = false;
        }

        skipToNL = false;

        if (writer.isTokenComplete()) {
            startToken(b);
            return;
        }

        if (isText) {
            acceptText(b);
            return;
        }

        byte hex = Zchars.parseHex(b);
        if (hex == Zchars.PARSE_NOT_HEX_0X10) {
            // Check big field odd length
            if (!numeric && writer.getCurrentWriteTokenKey() == Zchars.Z_BIGFIELD_HEX && writer.isInNibble()) {
                writer.fail(ERROR_CODE_ODD_BIGFIELD_LENGTH);
                skipToNL = true;
                if (b != Zchars.Z_NEWLINE) {
                    return;
                }
            }
            startToken(b);
            return;
        }

        if (numeric) {
            // Check field length
            if (writer.getCurrentWriteTokenNibbleLength() == 4) {
                writer.fail(ERROR_CODE_FIELD_TOO_LONG);
                skipToNL = true;
                return;
            }
            // Skip leading zeros
            if (writer.getCurrentWriteTokenLength() == 0 && hex == 0) {
                return;
            }
        }

        writer.continueTokenNibble(hex);
    }

    void acceptText(byte b) {
        if (b == Zchars.Z_NEWLINE) {
            if (isNormalString) {
                writer.fail(ERROR_CODE_STRING_NOT_TERMINATED);
                return;
            }
            writer.startToken(b, true);
            writer.endToken();
            isText = false;
        } else if (escapingCount > 0) {
            byte hex = Zchars.parseHex(b);
            if (hex != Zchars.PARSE_NOT_HEX_0X10) {
                writer.continueTokenNibble(hex);
                escapingCount--;
            } else {
                if (writer.isInNibble()) {
                    writer.continueTokenNibble((byte) 0);
                }
                writer.fail(ERROR_CODE_STRING_ESCAPING);
                escapingCount = 0;
                skipToNL = true;
            }
        } else if (isNormalString && b == Zchars.Z_BIGFIELD_STRING) {
            writer.endToken();
            isText = false;
        } else if (isNormalString && b == Zchars.Z_STRING_ESCAPE) {
            escapingCount = 2;
        } else {
            writer.continueTokenByte(b);
        }
    }

    void startToken(byte b) {
        if (addressing && (b != Zchars.Z_ADDRESSING_CONTINUE && b != Zchars.Z_NEWLINE)) {
            writer.startToken(ADDRESSING_FIELD_KEY, false);
            writer.continueTokenByte(b);
            addressing = false;
            isText = true;
            escapingCount = 0;
            isNormalString = false;
            return;
        }
        if (b == Zchars.Z_ADDRESSING) {
            addressing = true;
        }
        numeric = true;
        if (Zchars.isNonNumerical(b)) {
            numeric = false;
        }
        writer.startToken(b, numeric);

        if (b == Zchars.Z_COMMENT) {
            if (DROP_COMMENTS) {
                skipToNL = true;
            } else {
                isText = true;
                escapingCount = 0;
                isNormalString = false;
            }
            return;
        }
        if (b == Zchars.Z_BIGFIELD_STRING) {
            isText = true;
            escapingCount = 0;
            isNormalString = true;
            return;
        }
        isText = false;
        if (Zchars.isSeparator(b)) {
            writer.endToken();
        }
    }
}
