package org.zcode.javareceiver.core;

import org.zcode.javareceiver.execution.ZcodeCommandView;
import org.zcode.javareceiver.semanticParser.SemanticParser;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;

public abstract class ZcodeChannel {
    protected final SemanticParser p;
    protected final ZcodeOutStream out;

    public ZcodeChannel(SemanticParser p, ZcodeOutStream out) {
        this.p = p;
        this.out = out;
    }

    public ZcodeChannel(ZcodeTokenBuffer buffer, ZcodeOutStream out) {
        this.p = new SemanticParser(buffer.getTokenReader());
        this.out = out;
    }

    public SemanticParser getParser() {
        return p;
    }

    public ZcodeOutStream getOutStream() {
        return out;
    }

    public void moveAlong() {

    }

    public abstract void channelInfo(ZcodeCommandView view);

    public abstract void channelSetup(ZcodeCommandView view);
}
