package net.zscript.javareceiver.scriptSpaces;

import net.zscript.javareceiver.core.AbstractOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.semanticParser.SemanticParser;
import net.zscript.util.ByteString;

public class ScriptSpaceOutStream extends AbstractOutStream {
    private final Zscript                      z;
    private final SemanticParser               parser;
    private       ByteString.ByteStringBuilder buffer;
    private       boolean                      open     = false;
    private       boolean                      isFailed = false;
    private       boolean                      isError  = false;

    public ScriptSpaceOutStream(Zscript z, SemanticParser parser) {
        this.z = z;
        this.parser = parser;
    }

    @Override
    public void open() {
        open = true;
        buffer = ByteString.builder();
        isFailed = false;
    }

    @Override
    public void close() {
        if (isFailed) {
            z.getNotificationOutStream().open();
            z.getNotificationOutStream().writeBytes(buffer.toByteArray());
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
                buffer.appendByte(bytes[i] >>> 4);
                buffer.appendByte(toHexChar(bytes[i] & 0xf));
            }
        } else {
            buffer.appendRaw(bytes, 0, count);
        }
        if (parser.isInErrorState()) {
            isFailed = true;
            isError = true;
        } else {
            isFailed = parser.isFailed();
        }
    }
}
