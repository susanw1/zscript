package net.zscript.javareceiver.testing;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PushbackInputStream;
import java.io.UncheckedIOException;

import net.zscript.javareceiver.core.ZscriptChannel;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.Tokenizer;

/**
 * Channel designed for testing. It reads commands from an InputStream and sends responses/notifications to an OutputStream.
 */
public class IostreamChannel extends ZscriptChannel {
    private final PushbackInputStream commandStream;
    private final Tokenizer           tokenizer;

    /**
     * Channel reads commands and sends responses/notifications.
     *
     * @param commandStream  the source of commands
     * @param responseStream the return stream for responses/notifications
     */
    public IostreamChannel(final TokenBuffer buffer, InputStream commandStream, OutputStream responseStream) {
        super(buffer, new OutputStreamOutStream<>(responseStream));
        this.commandStream = new PushbackInputStream(commandStream);
        this.tokenizer = new Tokenizer(buffer.getTokenWriter());
    }

    @Override
    public void moveAlong() {
        try {
            while (commandStream.available() > 0) {
                int c = commandStream.read();
                if (c == -1) {
                    // EOF isn't really meaningful. close and leave?
                    commandStream.close();
                    break;
                } else if (!tokenizer.offer((byte) c)) {
                    commandStream.unread(c);
                    break;
                }
            }
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

    @Override
    public void channelInfo(CommandContext ctx) {
        ctx.getOutStream().writeBigFieldQuoted("IostreamChannel");
    }

    @Override
    public void channelSetup(CommandContext ctx) {
        // nothing to do here
    }
}
