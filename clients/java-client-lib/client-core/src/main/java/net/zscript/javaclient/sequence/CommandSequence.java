package net.zscript.javaclient.sequence;

import javax.annotation.Nullable;
import java.util.Collection;

import static net.zscript.javaclient.commandpaths.NumberField.fieldOf;
import static net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;

import net.zscript.javaclient.ZscriptParseException;
import net.zscript.javaclient.commandpaths.CommandExecutionPath;
import net.zscript.model.ZscriptModel;
import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.TokenBufferIterator;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

/**
 * A sendable, unaddressed command sequence, optionally with locks (eg "%1a") and echo (eg "_c2") fields. It's a {@link CommandExecutionPath} with the extra sequence-level
 * information.
 * <p>
 * As far as a specific device is concerned, executing a CommandSequence results in a corresponding {@link ResponseSequence}.
 */
public class CommandSequence implements ByteAppendable {

    public static CommandSequence from(CommandExecutionPath path, int echoField) {
        return from(path, echoField, false);
    }

    public static CommandSequence from(CommandExecutionPath path, int echoField, boolean supports32Locks) {
        return new CommandSequence(path, echoField, ZscriptLockSet.allLocked(supports32Locks));
    }

    public static CommandSequence from(CommandExecutionPath path, int echoField, boolean supports32Locks, Collection<LockCondition> lockConditions) {
        ZscriptLockSet locks = ZscriptLockSet.noneLocked(supports32Locks);
        for (LockCondition c : lockConditions) {
            c.apply(path, locks);
        }
        return new CommandSequence(path, echoField, locks);
    }

    public static CommandSequence from(CommandExecutionPath path, int echoField, @Nullable ZscriptLockSet locks) {
        return new CommandSequence(path, echoField, locks);
    }

    public static CommandSequence parse(ZscriptModel model, ReadToken start, boolean supports32Locks) {
        ZscriptLockSet      locks     = null;
        int                 echoField = -1;
        TokenBufferIterator iter      = start.tokenIterator();
        ReadToken           current   = iter.next().orElse(null);
        while (current != null && (current.getKey() == Zchars.Z_LOCKS || current.getKey() == Zchars.Z_ECHO)) {
            if (current.getKey() == Zchars.Z_LOCKS) {
                if (locks != null) {
                    throw new ZscriptParseException("Tokens contained two lock fields");
                }
                locks = ZscriptLockSet.parse(current, supports32Locks);
            } else {
                if (echoField != -1) {
                    throw new ZscriptParseException("Tokens contained two echo fields");
                }
                echoField = current.getData16();
            }
            current = iter.next().orElse(null);
        }
        return new CommandSequence(CommandExecutionPath.parse(model, current), echoField, locks);
    }

    private final CommandExecutionPath executionPath;
    private final int                  echoField;
    @Nullable
    private final ZscriptLockSet       locks;

    private CommandSequence(CommandExecutionPath executionPath, int echoField, @Nullable ZscriptLockSet locks) {
        this.executionPath = executionPath;
        this.echoField = echoField;
        this.locks = locks;
    }

    @Override
    public void appendTo(ByteStringBuilder builder) {
        if (locks != null) {
            locks.appendTo(builder);
        }
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

    public boolean hasLockField() {
        return locks != null;
    }

    public int getEchoValue() { // -1 if there isn't one
        return echoField;
    }

    @Nullable
    public ZscriptLockSet getLocks() {
        return locks;
    }

    public int getBufferLength() {
        return executionPath.getBufferLength() + (echoField > 0xff ? 4 : 3) + (locks != null ? locks.getBufferLength() : 0);
    }

    @Override
    public String toString() {
        return toStringImpl();
    }

}
