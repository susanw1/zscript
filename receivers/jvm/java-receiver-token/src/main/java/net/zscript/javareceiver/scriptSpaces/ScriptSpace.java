package net.zscript.javareceiver.scriptSpaces;

import java.nio.charset.StandardCharsets;

import net.zscript.javareceiver.core.OutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.execution.ActionSource;
import net.zscript.javareceiver.execution.ZscriptAction;
import net.zscript.javareceiver.semanticParser.ExecutionActionFactory;
import net.zscript.javareceiver.semanticParser.SemanticParser;
import net.zscript.javareceiver.tokenizer.ScriptSpaceBuffer;
import net.zscript.javareceiver.tokenizer.ScriptSpaceBuffer.ScriptSpaceWriterBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;

public class ScriptSpace implements ActionSource {
    private final ScriptSpaceOutStream outStream;
    private final ScriptSpaceBuffer    buffer;
    private final SemanticParser       parser;
    private final boolean              canBeWrittenTo;

    public static ScriptSpace from(Zscript z, String str) {
        ScriptSpaceBuffer       buffer = new ScriptSpaceBuffer();
        ScriptSpaceWriterBuffer writer = buffer.fromStart();
        Tokenizer               tok    = new Tokenizer(writer.getTokenWriter(), 2);
        for (byte b : str.getBytes(StandardCharsets.UTF_8)) {
            tok.accept(b);
        }
        writer.commitChanges();
        return new ScriptSpace(z, buffer, false);
    }

    public static ScriptSpace blank(Zscript z) {
        ScriptSpaceBuffer buffer = new ScriptSpaceBuffer();
        return new ScriptSpace(z, buffer, true);
    }

    public ScriptSpace(Zscript z, ScriptSpaceBuffer buffer, boolean canBeWrittenTo) {
        this.buffer = buffer;
        this.parser = new SemanticParser(buffer.getTokenReader(), new ExecutionActionFactory());
        this.outStream = new ScriptSpaceOutStream(z, parser, new byte[32]);
        this.canBeWrittenTo = canBeWrittenTo;
    }

    @Override
    public ZscriptAction getAction() {
        ZscriptAction a = parser.getAction();
        return a;
    }

    @Override
    public OutStream getOutStream(Zscript zscript) {
        return outStream;
    }

    public boolean canBeWrittenTo() {
        return canBeWrittenTo;
    }

    public ScriptSpaceWriterBuffer append() {
        if (canBeWrittenTo) {
            return buffer.append();
        } else {
            throw new UnsupportedOperationException();
        }
    }

    public ScriptSpaceWriterBuffer overwrite() {
        if (canBeWrittenTo) {
            return buffer.fromStart();
        } else {
            throw new UnsupportedOperationException();
        }
    }

    public int getCurrentLength() {
        return buffer.getCurrentLength();
    }

    public boolean isRunning() {
        return parser.isRunning();
    }

    public void run() {
        parser.resume();
    }

    public void stop() {
        parser.stop();
    }

}
