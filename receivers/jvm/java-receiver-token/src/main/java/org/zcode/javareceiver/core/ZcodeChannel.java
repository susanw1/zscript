package org.zcode.javareceiver.core;

import org.zcode.javareceiver.execution.ActionSource;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javareceiver.semanticParser.ExecutionActionFactory;
import org.zcode.javareceiver.semanticParser.SemanticParser;
import org.zcode.javareceiver.semanticParser.ZcodeSemanticAction;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;

public abstract class ZcodeChannel implements ActionSource {
    protected final SemanticParser p;
    protected final ZcodeOutStream out;

    public ZcodeChannel(final SemanticParser p, final ZcodeOutStream out) {
        this.p = p;
        this.out = out;
    }

    public ZcodeChannel(final ZcodeTokenBuffer buffer, final ZcodeOutStream out) {
        this(new SemanticParser(buffer.getTokenReader(), new ExecutionActionFactory()), out);
    }

    public SemanticParser getParser() {
        return p;
    }

    @Override
    public ZcodeOutStream getOutStream() {
        return out;
    }

    @Override
    public ZcodeSemanticAction getAction() {
        return p.getAction();
    }

    public void moveAlong() {
    }

    public void setChannelIndex(byte i) {
        p.setChannelIndex(i);
    }

    public abstract void channelInfo(final ZcodeCommandContext ctx);

    public abstract void channelSetup(final ZcodeCommandContext ctx);
}
