package net.zscript.javareceiver.testing;

import javax.annotation.Nonnull;
import java.nio.charset.StandardCharsets;

import net.zscript.javareceiver.core.SequenceOutStream;
import net.zscript.javareceiver.core.ZscriptChannel;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.TokenRingBuffer;
import net.zscript.tokenizer.Tokenizer;

public class StringChannel extends ZscriptChannel {
    private final Tokenizer in;
    private final byte[]    input;
    private       int       index = 0;

    @Nonnull
    public static StringChannel from(String input, SequenceOutStream out) {
        TokenBuffer buffer = TokenRingBuffer.createBufferWithCapacity(64);
        return new StringChannel(buffer, input, out);
    }

    private StringChannel(TokenBuffer buffer, String input, SequenceOutStream out) {
        super(buffer, out);
        this.in = new Tokenizer(buffer.getTokenWriter(), false);
        this.input = input.getBytes(StandardCharsets.UTF_8);
    }

    @SuppressWarnings("StatementWithEmptyBody")
    @Override
    public void moveAlong() {
        while (index < input.length && in.offer(input[index++])) {
        }
    }

    @Override
    public void channelInfo(@Nonnull CommandContext ctx) {
    }

    @Override
    public void channelSetup(@Nonnull CommandContext ctx) {
    }
}
