package net.zscript.javareceiver.scriptSpaces;

import java.nio.charset.StandardCharsets;

import net.zscript.javareceiver.core.OutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.execution.ActionSource;
import net.zscript.javareceiver.execution.ZscriptAction;
import net.zscript.javareceiver.semanticParser.ExecutionActionFactory;
import net.zscript.javareceiver.semanticParser.SemanticParser;
import net.zscript.javareceiver.tokenizer.ScriptSpaceBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;

public class ScriptSpace implements ActionSource {
    private final ScriptSpaceOutStream outStream;
    private final SemanticParser       parser;

    public static ScriptSpace from(Zscript z, String str) {
        ScriptSpaceBuffer buffer = new ScriptSpaceBuffer();
        Tokenizer         tok    = new Tokenizer(buffer.getTokenWriter(), 2);
        for (byte b : str.getBytes(StandardCharsets.UTF_8)) {
            tok.accept(b);
        }
        return new ScriptSpace(z, buffer);
    }

    public ScriptSpace(Zscript z, ScriptSpaceBuffer buffer) {
        this.parser = new SemanticParser(buffer.getReadOnlyView().getTokenReader(), new ExecutionActionFactory());
        this.outStream = new ScriptSpaceOutStream(z, parser, new byte[32]);
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

}
