package net.zscript.javaclient.sequence;

import static net.zscript.javaclient.commandpaths.FieldElement.fieldOf;
import static net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;

import net.zscript.javaclient.ZscriptParseException;
import net.zscript.javaclient.commandpaths.CommandExecutionPath;
import net.zscript.model.ZscriptModel;
import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.TokenBufferIterator;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

/**
 * A sendable, unaddressed command sequence, optionally with echo (eg "_c2") field. It's a {@link CommandExecutionPath} with the extra sequence-level information.
 *
 * <p>As far as a specific device is concerned, executing a CommandSequence results in a corresponding {@link ResponseSequence}.
 */
public class CommandSequence implements ByteAppendable {

    public static CommandSequence from(CommandExecutionPath path, int echoField) {
        return new CommandSequence(path, echoField);
    }

    public static CommandSequence parse(ZscriptModel model, ReadToken start) {
        int                 echoField = -1;
        TokenBufferIterator iter      = start.tokenIterator();
        ReadToken           current   = iter.next().orElse(null);
        while (current != null && current.getKey() == Zchars.Z_ECHO) {
            if (echoField != -1) {
                throw new ZscriptParseException("Tokens contained two echo fields");
            }
            echoField = current.getData16();

            current = iter.next().orElse(null);
        }
        return new CommandSequence(CommandExecutionPath.parse(model, current), echoField);
    }

    private final CommandExecutionPath executionPath;
    private final int                  echoField;

    private CommandSequence(CommandExecutionPath executionPath, int echoField) {
        this.executionPath = executionPath;
        this.echoField = echoField;
    }

    @Override
    public void appendTo(ByteStringBuilder builder) {
        if (echoField != -1) {
            fieldOf(Zchars.Z_ECHO, echoField).appendTo(builder);
        }
        executionPath.appendTo(builder);
    }

    public CommandExecutionPath getExecutionPath() {
        return executionPath;
    }

    public boolean hasEchoField() {
        return echoField != -1;
    }

    public int getEchoValue() { // -1 if there isn't one
        return echoField;
    }

    public int getBufferLength() {
        return executionPath.getBufferLength() + (echoField > 0xff ? 4 : 3);
    }

    @Override
    public String toString() {
        return toStringImpl();
    }

}
