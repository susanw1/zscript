package net.zscript.javareceiver.core;

import java.io.IOException;
import java.io.OutputStream;
import java.io.UncheckedIOException;

/**
 * Utility class for helping tests: AbstractOutStream implementation that collects bytes
 */
public class OutputStreamOutStream<Z extends OutputStream> extends AbstractOutStream {
    private final Z output;
    boolean         open = false;

    public OutputStreamOutStream(final Z output) {
        this.output = output;
    }

    public Z getOutputStream() {
        return output;
    }

    @Override
    public void open() {
        open = true;
    }

    @Override
    public void close() {
        open = false;
    }

    @Override
    public boolean isOpen() {
        return open;
    }

    @Override
    protected void writeBytes(byte[] bytes, int count, boolean hexMode) {
        try {
            if (hexMode) {
                for (int i = 0; i < count; i++) {
                    output.write(toHexChar(bytes[i] >>> 4));
                    output.write(toHexChar(bytes[i] & 0xf));
                }
            } else {
                output.write(bytes, 0, count);
            }
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }
}
