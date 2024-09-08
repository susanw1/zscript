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
