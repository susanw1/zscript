package net.zscript.javareceiver.core;

import javax.annotation.Nullable;

import net.zscript.javareceiver.execution.ActionSource;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.semanticparser.ExecutionActionFactory;
import net.zscript.javareceiver.semanticparser.SemanticAction;
import net.zscript.javareceiver.semanticparser.SemanticParser;
import net.zscript.tokenizer.TokenBuffer;

/**
 * Abstract implementation of a Channel that integrates a parser and buffer, and a source of actions that execute the received commands. As an ActionSource, it provides executable
 * Actions. Implementors must supply:
 * <ul>
 *     <li>{@link #channelInfo(CommandContext)}</li>
 *     <li>{@link #channelSetup(CommandContext)}</li>
 *     <li>either: an independent background mechanism to write to the Tokenizer, or a {@link #moveAlong()} implementation to keep feeding it synchronously.</li>
 * </ul>
 */
public abstract class ZscriptChannel implements ActionSource {
    protected final SemanticParser    parser;
    protected final SequenceOutStream out;

    protected ZscriptChannel(final SemanticParser parser, final SequenceOutStream out) {
        this.parser = parser;
        this.out = out;
    }

    protected ZscriptChannel(final TokenBuffer buffer, final SequenceOutStream out) {
        this(new SemanticParser(buffer.getTokenReader(), new ExecutionActionFactory()), out);
    }

    public SemanticParser getParser() {
        return parser;
    }

    @Override
    public SequenceOutStream getOutStream(@Nullable Zscript zscript) {
        return out;
    }

    @Override
    public SemanticAction getAction() {
        return parser.getAction();
    }

    /**
     * If this Channel is synchronous and needs prodding, then this method should be overridden to feed bytes into the Tokenizer.
     */
    public void moveAlong() {
    }

    public void setChannelIndex(byte i) {
        parser.setChannelIndex(i);
    }

    public abstract void channelInfo(final CommandContext ctx);

    public abstract void channelSetup(final CommandContext ctx);
}
