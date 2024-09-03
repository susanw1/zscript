package net.zscript.javareceiver.scriptSpaces;

import java.nio.charset.StandardCharsets;

import net.zscript.javareceiver.core.OutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.execution.ActionSource;
import net.zscript.javareceiver.execution.ZscriptAction;
import net.zscript.javareceiver.scriptSpaces.ScriptSpaceTokenBuffer.ScriptSpaceWriterTokenBuffer;
import net.zscript.javareceiver.semanticParser.ExecutionActionFactory;
import net.zscript.javareceiver.semanticParser.SemanticParser;
import net.zscript.tokenizer.Tokenizer;

/**
 *
 */
public class ScriptSpace implements ActionSource {
    private final ScriptSpaceOutStream   outStream;
    private final ScriptSpaceTokenBuffer buffer;
    private final SemanticParser         parser;
    private final boolean                isWritable;

    public static ScriptSpace from(Zscript z, String str) {
        ScriptSpaceTokenBuffer       buffer = new ScriptSpaceTokenBuffer();
        ScriptSpaceWriterTokenBuffer writer = buffer.fromStart();
        Tokenizer                    tok    = new Tokenizer(writer.getTokenWriter(), 2);
        for (byte b : str.getBytes(StandardCharsets.UTF_8)) {
            tok.accept(b);
        }
        writer.commitChanges();
        return new ScriptSpace(z, buffer, false);
    }

    public static ScriptSpace blank(Zscript z) {
        ScriptSpaceTokenBuffer buffer = new ScriptSpaceTokenBuffer();
        ScriptSpace            space  = new ScriptSpace(z, buffer, true);
        space.stop();
        return space;
    }

    private ScriptSpace(Zscript z, ScriptSpaceTokenBuffer buffer, boolean isWritable) {
        this.buffer = buffer;
        this.parser = new SemanticParser(buffer.getTokenReader(), new ExecutionActionFactory());
        this.outStream = new ScriptSpaceOutStream(z, parser, new byte[32]);
        this.isWritable = isWritable;
    }

    @Override
    public ZscriptAction getAction() {
        return parser.getAction();
    }

    @Override
    public OutStream getOutStream(Zscript zscript) {
        return outStream;
    }

    public boolean isWritable() {
        return isWritable;
    }

    public ScriptSpaceWriterTokenBuffer append() {
        if (isWritable) {
            return buffer.append();
        } else {
            throw new UnsupportedOperationException();
        }
    }

    public ScriptSpaceWriterTokenBuffer overwrite() {
        if (isWritable) {
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
