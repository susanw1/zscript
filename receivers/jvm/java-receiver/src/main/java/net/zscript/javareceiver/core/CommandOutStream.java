package net.zscript.javareceiver.core;

import net.zscript.tokenizer.ZscriptField;
import net.zscript.util.BlockIterator;

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
    @Deprecated
    void writeBigFieldQuoted(String text);

    /**
     * Writes the supplied bytes as a quoted Big Field. It is assumed to already be encoded as required, and will *not* be UTF-8 encoded. However, forbidden chars (eg '\n', '"',
     * '=', '\0') will be automatically Quoted-Printable escaped, eg =0a.
     *
     * @param utf8text the text to write
     */
    @Deprecated
    void writeBigFieldQuoted(byte[] utf8text);

    /**
     * Writes the bytes provided as a hex big field, eg +68656c6c6f
     *
     * @param data the data to write
     */
    @Deprecated
    void writeBigFieldHex(byte[] data);

    /**
     * Writes the supplied string as a quoted string field. It is assumed to represent actual text and will be UTF-8 encoded. Forbidden chars (eg '\n', '"', '=', '\0') will be
     * automatically Quoted-Printable escaped, eg =0a.
     *
     * @param key  the key for the field
     * @param text the text to write
     */
    void writeFieldQuoted(byte key, String text);

    /**
     * Convenience call to {@link #writeFieldQuoted(byte, String)} with key as char.
     *
     * @param key  the key for the field
     * @param text the text to write
     * @see #writeFieldQuoted(byte, String)
     */
    default void writeFieldQuoted(char key, String text) {
        writeFieldQuoted((byte) key, text);
    }

    /**
     * Writes the supplied bytes as a quoted string field. It is assumed to already be encoded as required, and will *not* be UTF-8 encoded. However, forbidden chars (eg '\n', '"',
     * '=', '\0') will be automatically Quoted-Printable escaped, eg =0a.
     *
     * @param key      the key for the field
     * @param utf8text the text to write
     */
    void writeFieldQuoted(byte key, byte[] utf8text);

    /**
     * Convenience call to {@link #writeFieldQuoted(byte, byte[])} with key as char.
     *
     * @param key      the key for the field
     * @param utf8text the text to write
     */
    default void writeFieldQuoted(char key, byte[] utf8text) {
        writeFieldQuoted((byte) key, utf8text);
    }

    /**
     * Writes the bytes produced from the supplied BlockIterator as a quoted string field. It is assumed to already be encoded as required, and will *not* be UTF-8 encoded.
     * However, forbidden chars (eg '\n', '"', '=', '\0') will be automatically Quoted-Printable escaped, eg =0a.
     *
     * <p>When this method returns, the supplied iterator will have been drained, and its {@link BlockIterator#hasNext()} method will be {@code false}.
     *
     * @param key              the key for the field
     * @param utf8textIterator the text to write
     */
    void writeFieldQuoted(byte key, BlockIterator utf8textIterator);

    /**
     * Convenience call to {@link #writeFieldQuoted(byte, BlockIterator)} with key as char.
     *
     * @param key              the key for the field
     * @param utf8textIterator the text to write
     */
    default void writeFieldQuoted(char key, BlockIterator utf8textIterator) {
        writeFieldQuoted((byte) key, utf8textIterator);
    }

    /**
     * Writes the bytes provided as a hex string field, eg A68656c6c6f
     *
     * @param key  the key for the field
     * @param data the data to write
     */
    void writeFieldHex(byte key, byte[] data);

    /**
     * Convenience call to {@link #writeFieldHex(byte, byte[])} with key as char.
     *
     * @param key  the key for the field
     * @param data the data to write
     */
    default void writeFieldHex(char key, byte[] data) {
        writeFieldHex((byte) key, data);
    }

    /**
     * Writes the bytes provided as a hex string field, eg A68656c6c6f
     *
     * <p>When this method returns, the supplied iterator will have been drained, and its {@link BlockIterator#hasNext()} method will be {@code false}.
     *
     * @param key          the key for the field
     * @param dataIterator the data to write
     */
    void writeFieldHex(byte key, BlockIterator dataIterator);

    /**
     * Convenience call to {@link #writeFieldHex(byte, BlockIterator)} with key as char.
     *
     * @param key          the key for the field
     * @param dataIterator the data to write
     */
    default void writeFieldHex(char key, BlockIterator dataIterator) {
        writeFieldHex((byte) key, dataIterator);
    }

}
