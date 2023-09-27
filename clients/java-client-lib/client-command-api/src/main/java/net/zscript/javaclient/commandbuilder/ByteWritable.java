package net.zscript.javaclient.commandbuilder;

import java.io.IOException;
import java.io.OutputStream;

import static net.zscript.javaclient.commandbuilder.Utils.toHex;

public interface ByteWritable {
    <T extends OutputStream> T writeTo(final T out) throws IOException;

    default <T extends OutputStream> T writeNumeric(byte f, int value, final T out) throws IOException {
        out.write(f);
        return writeNumeric(value, out);
    }

    default <T extends OutputStream> T writeNumeric(int value, final T out) throws IOException {
        if (value == 0) {
            return out;
        }
        if (value >= 0x10000 || value < 0) {
            throw new IllegalArgumentException("Numeric fields must be uint16s: " + value);
        }
        if (value >= 0x1000) {
            out.write(toHex(value >>> 12));
        }
        if (value >= 0x100) {
            out.write(toHex((value >>> 8) & 0xF));
        }
        if (value >= 0x10) {
            out.write(toHex((value >>> 4) & 0xF));
        }
        out.write(toHex(value & 0xF));
        return out;
    }

    default <T extends OutputStream> T writeHex(byte value, final T out) throws IOException {
        out.write(toHex((value >> 4) & 0xF));
        out.write(toHex(value & 0xF));
        return out;
    }
}
