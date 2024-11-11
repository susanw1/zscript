package net.zscript.javareceiver.testing;

import java.io.IOException;
import java.io.OutputStream;
import java.io.UncheckedIOException;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javareceiver.core.AbstractOutStream;
import net.zscript.util.ByteString;

/**
 * Utility class for helping tests: AbstractOutStream implementation that collects bytes and writes them to a specified {@link java.io.OutputStream}.
 */
public class OutputStreamOutStream<Z extends OutputStream> extends AbstractOutStream {
    private static final Logger LOG = LoggerFactory.getLogger(OutputStreamOutStream.class);

    private final Z       output;
    private       boolean open = false;

    public OutputStreamOutStream(final Z output) {
        this.output = output;
    }

    public Z getOutputStream() {
        return output;
    }

    @Override
    public void open() {
        if (open) {
            throw new IllegalStateException("OutStream already open");
        }
        LOG.trace("open()");
        open = true;
    }

    @Override
    public void close() {
        LOG.trace("close()");
        open = false;
        try {
            output.flush();
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

    @Override
    public boolean isOpen() {
        return open;
    }

    @Override
    protected void writeBytes(byte[] bytes, int count, boolean hexMode) {
        LOG.atTrace().setMessage("writeBytes /{}/ (hex?={})").addArgument(() -> ByteString.from(bytes).substring(0, count).asString()).addArgument(hexMode).log();
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
