package net.zscript.javareceiver.core;

import java.io.IOException;
import java.io.StringWriter;
import java.nio.charset.StandardCharsets;

import org.apache.commons.io.output.WriterOutputStream;

/**
 * Utility class for helping tests: AbstractOutStream implementation that collects bytes
 */
public class StringWriterOutStream extends OutputStreamOutStream<WriterOutputStream> {
    private final StringWriter stringWriter;
    private       boolean      open = false;

    public StringWriterOutStream() throws IOException {
        this(new StringWriter());
    }

    private StringWriterOutStream(StringWriter stringWriter) throws IOException {
        super(new WriterOutputStream.Builder()
                .setCharset(StandardCharsets.ISO_8859_1)
                .setWriter(stringWriter)
                .setWriteImmediately(true)
                .get());
        this.stringWriter = stringWriter;
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

    public String getString() {
        return stringWriter.toString();
    }

    public String getStringAndReset() {
        final String string = stringWriter.toString();
        stringWriter.getBuffer().setLength(0);
        return string;
    }

    @Override
    public String toString() {
        return stringWriter.toString();
    }
}
