package net.zscript.javareceiver.core;

import net.zscript.tokenizer.ZscriptField;

/**
 * Defines the operations on the response OutStream available to a command via the CommandContext. Deliberately narrowed interface - we don't want commands confused with machinery
 * that can break the sequence flow.
 */
public interface CommandOutStream {
    /**
     * Writes the supplied field char and its 16-bit value to this stream in Zscript conventional 0-4 digit notation (ie no leading zeros, even for zero).
     *
     * @param field the field char
     * @param value the 0-0xffff value to write
     */
    void writeField(byte field, int value);

    /**
     * Convenience method to minimize char->byte casting.
     *
     * @param field the field char
     * @param value the 0-0xffff value to write
     */
    default void writeField(char field, int value) {
        writeField((byte) field, value);
    }

    void writeField(ZscriptField field);

    /**
     * Writes the supplied string as a quoted Big Field. It is assumed to represent actual text and will be UTF-8 encoded. Forbidden chars (eg '\n', '"', '=', '\0') will be
     * automatically Quoted-Printable escaped, eg =0a.
     *
     * @param text the text to write
     */
    void writeBigFieldQuoted(String text);

    /**
     * Writes the supplied bytes as a quoted Big Field. It is assumed to already be encoded as required, and will *not* be UTF-8 encoded. However, forbidden chars (eg '\n', '"',
     * '=', '\0') will be automatically Quoted-Printable escaped, eg =0a.
     *
     * @param utf8text the text to write
     */
    void writeBigFieldQuoted(byte[] utf8text);

    /**
     * Writes the bytes provided as a hex big field, eg +68656c6c6f
     *
     * @param data the data to write
     */
    void writeBigFieldHex(byte[] data);
}
