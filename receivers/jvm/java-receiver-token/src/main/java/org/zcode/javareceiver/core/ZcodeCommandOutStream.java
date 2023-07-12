package org.zcode.javareceiver.core;

import org.zcode.javareceiver.execution.ZcodeField;

/**
 * Defines the operations on the response OutStream available to a command via the ZcodeCommandContext.
 */
public interface ZcodeCommandOutStream {

    void writeByte(byte c);

    void writeString(String string);

    void writeString(byte[] data);

    void writeBig(byte[] data);

    void writeField(byte field, int value);

    default void writeField(char field, int value) {
        writeField((byte) field, value);
    }

    void writeField(ZcodeField field);
}