package net.zscript.javaclient.commandPaths;

import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.ZscriptField;
import net.zscript.util.BlockIterator;
import net.zscript.util.ByteString;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

/**
 * Represents a single big-field element (there may be multiple elements within n expression).
 */
public class BigField implements ZscriptField, ByteAppendable {
    private final ByteString data;
    private final boolean    isString;

    public BigField(ByteString data, boolean isString) {
        this.data = data;
        this.isString = isString;
    }

    public BigField(byte[] data, boolean isString) {
        this(ByteString.from(data), isString);
    }

    /**
     * Returns a copy of the enclosed data.
     *
     * @return copy of the data
     */
    public byte[] getData() {
        return data.toByteArray();
    }

    /**
     * Returns the big-field data.
     *
     * @return the big-field's data
     */
    public ByteString getDataAsByteString() {
        return data;
    }

    /**
     * Determines whether this big-field identifies as a text string, or binary data. Doesn't matter much, except we might keep it in its preferred form.
     *
     * @return true if textual, false if binary
     */
    public boolean isString() {
        return isString;
    }

    /**
     * Determines the length of the big-field data, in bytes.
     *
     * @return number of data bytes
     */
    public int getDataLength() {
        return data.size();
    }

    /**
     * Writes the big-field in its preferred form to the ByteStringBuilder.
     *
     * @param builder the builder to append to
     */
    @Override
    public void appendTo(ByteStringBuilder builder) {
        if (isString) {
            builder.appendByte(Zchars.Z_BIGFIELD_QUOTED);
            for (byte b : data) {
                if (Zchars.mustStringEscape(b)) {
                    builder.appendByte(Zchars.Z_STRING_ESCAPE).appendHexPair(b);
                } else {
                    builder.appendByte(b);
                }
            }
            builder.appendByte(Zchars.Z_BIGFIELD_QUOTED);
        } else {
            builder.appendByte(Zchars.Z_BIGFIELD_HEX);
            for (byte b : data) {
                builder.appendHexPair(b);
            }
        }
    }

    /**
     * Determines how much space this BigField requires when Tokenized in a standard TokenBuffer.
     *
     * @return the length of this field when written as a token
     */
    public int getBufferLength() {
        // FIXME: Note: doesn't take into account extension blocks length (they add 2 bytes each) - should it? (yes...)
        return 2 + getDataLength();
    }

    @Override
    public byte getKey() {
        return isString ? Zchars.Z_BIGFIELD_QUOTED : Zchars.Z_BIGFIELD_HEX;
    }

    @Override
    public int getValue() {
        return 0;
    }

    @Override
    public boolean isBigField() {
        return true;
    }

    @Override
    public BlockIterator iterator() {
        return data.iterator();
    }
}
