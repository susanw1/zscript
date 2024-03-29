package net.zscript.javareceiver.core;

import net.zscript.javareceiver.execution.ZscriptField;

/**
 * Defines the operations on the response OutStream available to a command via the CommandContext.
 */
public interface ZscriptCommandOutStream {

    void writeField(byte field, int value);

    default void writeField(char field, int value) {
        writeField((byte) field, value);
    }

    void writeField(ZscriptField field);

    /**
     * Writes the supplied string as a quoted Big Field. It is assumed to represent actual text and will be UTF-8 encoded. Forbidden chars (eg '\n', '"', '=', '\0') will be
     * automatically escaped.
     *
     * @param text the text to write
     */
    void writeQuotedString(String text);

    /**
     * Writes the supplied bytes as a quoted Big Field. It is assumed to be encoded as required, and will not be UTF-8 encoded. However, forbidden chars (eg '\n', '"', '=', '\0')
     * will be automatically escaped.
     *
     * @param text the text to write
     */
    void writeQuotedString(byte[] data);

    /**
     * Writes the bytes provided as a hex big field, eg +68656c6c6f
     *
     * @param data the data to write
     */
    void writeBig(byte[] data);
}