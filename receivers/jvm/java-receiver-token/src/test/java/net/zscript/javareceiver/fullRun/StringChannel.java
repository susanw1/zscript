package net.zscript.javareceiver.fullRun;

import java.nio.charset.StandardCharsets;

import net.zscript.javareceiver.core.OutStream;
import net.zscript.javareceiver.core.ZscriptChannel;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.tokenizer.TokenBuffer;
import net.zscript.javareceiver.tokenizer.TokenRingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;

public class StringChannel extends ZscriptChannel {
    private final Tokenizer in;
    private final byte[]    input;
    private int             index = 0;

    public static StringChannel from(String input, OutStream out) {
        TokenBuffer buffer = TokenRingBuffer.createBufferWithCapacity(64);
        return new StringChannel(buffer, input, out);
    }

    private StringChannel(TokenBuffer buffer, String input, OutStream out) {
        super(buffer, out);
        this.in = new Tokenizer(buffer.getTokenWriter(), 2);
        this.input = input.getBytes(StandardCharsets.UTF_8);
    }

    @Override
    public void moveAlong() {
        while (index < input.length && in.offer(input[index++])) {
        }
    }

    @Override
    public void channelInfo(CommandContext ctx) {
    }

    @Override
    public void channelSetup(CommandContext ctx) {
    }

}
