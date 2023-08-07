package net.zscript.javareceiver.scriptSpaces;

import java.util.Arrays;

import net.zscript.javareceiver.core.AbstractOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.semanticParser.SemanticParser;

public class ScriptSpaceOutStream extends AbstractOutStream {
    private final Zscript        z;
    private final SemanticParser parser;
    private final byte[]         buffer;
    private int                  bufferPos = 0;
    private boolean              open      = false;
    private boolean              isFailed  = false;
    private boolean              isError   = false;

    public ScriptSpaceOutStream(Zscript z, SemanticParser parser, byte[] buffer) {
        this.z = z;
        this.parser = parser;
        this.buffer = buffer;
    }

    @Override
    public void open() {
        open = true;
        bufferPos = 0;
        isFailed = false;
    }

    @Override
    public void close() {
        if (isFailed) {
            z.getNotificationOutStream().open();
            z.getNotificationOutStream().writeBytes(Arrays.copyOf(buffer, bufferPos));
            z.getNotificationOutStream().close();
        }
        if (isError) {
            parser.stop();
        }
        isError = false;
        isFailed = false;
        open = false;
    }

    @Override
    public boolean isOpen() {
        return open;
    }

    @Override
    protected void writeBytes(byte[] bytes, int count, boolean hexMode) {
        if (hexMode) {
            for (int i = 0; i < count; i++) {
                buffer[bufferPos++] = toHexChar(bytes[i] >>> 4);
                buffer[bufferPos++] = toHexChar(bytes[i] & 0xf);
            }
        } else {
            for (int i = 0; i < count; i++) {
                buffer[bufferPos++] = bytes[i];
            }
        }
        if (parser.isInErrorState()) {
            isFailed = true;
            isError = true;
        } else if (parser.isFailed()) {
            isFailed = true;
        } else {
            isFailed = false;
        }
    }
}
