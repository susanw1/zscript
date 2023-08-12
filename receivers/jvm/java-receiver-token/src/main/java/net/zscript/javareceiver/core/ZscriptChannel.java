package net.zscript.javareceiver.core;

import net.zscript.javareceiver.execution.ActionSource;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.semanticParser.ExecutionActionFactory;
import net.zscript.javareceiver.semanticParser.SemanticAction;
import net.zscript.javareceiver.semanticParser.SemanticParser;
import net.zscript.javareceiver.tokenizer.TokenBuffer;

public abstract class ZscriptChannel implements ActionSource {
    protected final SemanticParser p;
    protected final OutStream out;

    public ZscriptChannel(final SemanticParser p, final OutStream out) {
        this.p = p;
        this.out = out;
    }

    public ZscriptChannel(final TokenBuffer buffer, final OutStream out) {
        this(new SemanticParser(buffer.getTokenReader(), new ExecutionActionFactory()), out);
    }

    public SemanticParser getParser() {
        return p;
    }

    @Override
    public OutStream getOutStream(Zscript zscript) {
        return out;
    }

    @Override
    public SemanticAction getAction() {
        return p.getAction();
    }

    public void moveAlong() {
    }

    public void setChannelIndex(byte i) {
        p.setChannelIndex(i);
    }

    public abstract void channelInfo(final CommandContext ctx);

    public abstract void channelSetup(final CommandContext ctx);
}
