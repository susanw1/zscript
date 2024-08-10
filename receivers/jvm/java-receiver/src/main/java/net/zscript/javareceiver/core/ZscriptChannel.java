package net.zscript.javareceiver.core;

import net.zscript.javareceiver.execution.ActionSource;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.semanticParser.ExecutionActionFactory;
import net.zscript.javareceiver.semanticParser.SemanticAction;
import net.zscript.javareceiver.semanticParser.SemanticParser;
import net.zscript.tokenizer.TokenBuffer;

/**
 * Abstract implementation of a Channel that integrates a parser and buffer, and a source of actions that execute the received commands. As an ActionSource, it provides executable
 * Actions. Implementors must supply:
 * <ul>
 *     <li>{@link #channelInfo(CommandContext)}</li>
 *     <li>{@link #channelSetup(CommandContext)}</li>
 *     <li>either: an independent background means to write to the Tokenizer, or a {@link #moveAlong()} implementation to keep feeding it synchronously.</li>
 * </ul>
 */
public abstract class ZscriptChannel implements ActionSource {
    protected final SemanticParser p;
    protected final OutStream      out;

    protected ZscriptChannel(final SemanticParser p, final OutStream out) {
        this.p = p;
        this.out = out;
    }

    protected ZscriptChannel(final TokenBuffer buffer, final OutStream out) {
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

    /**
     * If this Channel is synchronous and needs prodding, then this method should be overridden to feed bytes into the Tokenizer.
     */
    public void moveAlong() {
    }

    public void setChannelIndex(byte i) {
        p.setChannelIndex(i);
    }

    public abstract void channelInfo(final CommandContext ctx);

    public abstract void channelSetup(final CommandContext ctx);
}
