package net.zscript.javaclient.tokens;

import static net.zscript.util.ByteString.byteString;

import net.zscript.javaclient.ZscriptParseException;
import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.AbstractArrayTokenBuffer;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.tokenizer.TokenBufferFlags;
import net.zscript.tokenizer.Tokenizer;
import net.zscript.util.ByteString;

/**
 * A TokenBuffer that automatically expands on being filled, for handling Zscript responses in a Client.
 */
public class ExtendingTokenBuffer extends AbstractArrayTokenBuffer {

    /**
     * Tokenizes the supplied bytes and returns the resulting buffer. If the result does NOT create a full sequence, then a {@code Zchars#Z_NEWLINE} is appended automatically.
     *
     * @param sequence the sequence to tokenize
     * @return the buffer containing the tokens
     */
    public static ExtendingTokenBuffer tokenize(ByteString sequence) {
        return tokenize(sequence, true);
    }

    /**
     * Tokenizes the supplied bytes representing a single Zscript sequence, and returns the resulting buffer. If autoNewline is set and the tokens do NOT end a sequence, then a
     * {@code Zchars#Z_NEWLINE} may be appended automatically, for convenience.
     *
     * @param sequence    the sequence to tokenize
     * @param autoNewline if true, then check to see if a newline might be needed and add it; if false, then treat an incomplete sequence as an error
     * @return the buffer containing the tokens
     * @throws ZscriptParseException if this buffer does not contain a single valid sequence
     */
    public static ExtendingTokenBuffer tokenize(ByteString sequence, boolean autoNewline) {
        return tokenizeImpl(sequence, autoNewline, true, true);
    }

    /**
     * Tokenizes the supplied bytes representing a single Zscript sequence with no auto-newline, and _without_ {@code parseOutAddressing} mode, so once an address field is read,
     * the rest is parsed as a single large token. and returns the resulting buffer.
     *
     * @param sequence the sequence to tokenize
     * @throws ZscriptParseException if this buffer does not contain a single valid sequence
     */
    public static ExtendingTokenBuffer tokenizeAsReceiver(ByteString sequence) {
        return tokenizeImpl(sequence, false, true, false);
    }

    /**
     * As per {@link #tokenize(ByteString, boolean)}, except parse errors are not rejected. ZscriptParseException is not thrown even if the sequence is malformed, in which case the
     * buffer is returned with the tokenization error marker intact. A NORMAL_SEQUENCE_END is appended only if the sequence contains a newline after normal tokenization (ie
     * 'autoNewline' is treated as false).
     *
     * @param sequence the sequence to tokenize, including newline if that is intended
     * @return the buffer containing the tokens (possibly including error markers)
     */
    public static ExtendingTokenBuffer tokenizeWithoutRejection(ByteString sequence) {
        return tokenizeImpl(sequence, false, false, true);
    }

    private static ExtendingTokenBuffer tokenizeImpl(ByteString sequence, boolean autoNewline, boolean rejectErrors, boolean parseOutAddressing) {
        final ExtendingTokenBuffer buf = new ExtendingTokenBuffer();
        final Tokenizer            tok = new Tokenizer(buf.getTokenWriter(), Tokenizer.DEFAULT_MAX_NUMERIC_BYTES, parseOutAddressing);

        final TokenBufferFlags flags = buf.getTokenReader().getFlags();
        for (int i = 0, n = sequence.size(); i < n; i++) {
            tok.accept(sequence.get(i));
            if (rejectErrors && flags.getAndClearSeqMarkerWritten()) {
                final byte key = buf.getTokenReader().tokenIterator().stream()
                        .filter(ReadToken::isSequenceEndMarker)
                        .findFirst().orElseThrow().getKey();
                if (key != Tokenizer.NORMAL_SEQUENCE_END) {
                    throw new ZscriptParseException("Syntax error [index=%d, err=%02x, text='%s']", i, key, sequence);
                } else if (i < n - 1) {
                    // is this ok? There could be insignificant chars (eg spaces) after the '\n'. Better to read whole thing and check if there are following tokens.
                    throw new ZscriptParseException("Extra characters found beyond first complete sequence[text='%s']", sequence);
                }
                return buf;
            }
        }
        if (autoNewline) {
            tok.accept(Zchars.Z_EOL_SYMBOL);
        } else {
            buf.getTokenWriter().endToken();
        }
        return buf;
    }

    /**
     * Initializes a new Buffer with exactly the bytes supplied, representing data in token form (validated). The writer is preset to the end of the written bytes ready for
     * appending.
     *
     * @param tokenBytes bytes representing a token sequence
     * @return a token buffer containing the supplied bytes
     * @see #getTokenBytes()
     */
    public static ExtendingTokenBuffer fromTokenBytes(ByteString tokenBytes) {
        return new ExtendingTokenBuffer(tokenBytes);
    }

    /**
     * Creates an empty token buffer with an initial capacity of 20 bytes.
     */
    public ExtendingTokenBuffer() {
        super(20);
    }

    /**
     * @param tokenBytes bytes representing a token sequence
     */
    private ExtendingTokenBuffer(ByteString tokenBytes) {
        super(validateTokens(tokenBytes).toByteArray(), tokenBytes.size());
    }

    /**
     * Validates a ByteString as containing a legitimate set of token bytes, as if generated by the tokenizer for some input. Note that the general structure of the token sequence
     * means that we can only really check that the Marker/token lengths create a valid chain to the end of the bytes.
     *
     * @param tokenBytes the bytes to validate
     * @return the input ByteString, unaltered
     * @throws IllegalArgumentException if invalid
     */
    private static ByteString validateTokens(ByteString tokenBytes) {
        int tokIndex = 0;
        while (tokIndex < tokenBytes.size()) {
            if (TokenBuffer.isMarker(tokenBytes.get(tokIndex))) {
                tokIndex++;
            } else {
                int len = tokenBytes.get(tokIndex + 1) & 0xff;
                tokIndex += len + 2;
            }
        }
        if (tokIndex != tokenBytes.size()) {
            throw new IllegalArgumentException("Not a valid token string: " + tokenBytes);
        }
        return tokenBytes;
    }

    /**
     * Retrieves a ByteString containing the completed token data written to this buffer.
     *
     * @return tokenized data
     */
    public ByteString getTokenBytes() {
        return byteString(super.getInternalData(), 0, getWriteStart());
    }

    /**
     * There's always room, as we auto-expand.
     *
     * @param writeCursor ignored
     * @param size        ignored
     * @return always true
     */
    @Override
    protected boolean checkAvailableCapacityFrom(int writeCursor, int size) {
        return true;
    }

    /**
     * Expands the underlying array, and returns the offset we were trying for now that it will fit.
     * <p>
     * {@inheritDoc}
     *
     * @param overflowedOffset the offset we were trying to access
     * @return the overflowedOffset that was passed in, as there's now enough space
     */
    @Override
    protected int offsetOnOverflow(int overflowedOffset) {
        // expand to double current size
        extendData(getDataSize());
        return overflowedOffset;
    }
}
