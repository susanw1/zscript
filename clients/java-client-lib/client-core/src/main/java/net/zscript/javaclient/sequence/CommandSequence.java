package net.zscript.javaclient.sequence;

import javax.annotation.Nullable;
import java.util.Collection;

import static net.zscript.javaclient.commandPaths.NumberField.fieldOf;
import static net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;

import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.model.ZscriptModel;
import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.TokenBufferIterator;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

public class CommandSequence implements ByteAppendable {

    public static CommandSequence from(CommandExecutionPath path, int echoField) {
        return from(path, echoField, false);
    }

    public static CommandSequence from(CommandExecutionPath path, int echoField, boolean supports32Locks) {
        return new CommandSequence(path, echoField, ZscriptLockSet.allLocked(supports32Locks));
    }

    public static CommandSequence from(CommandExecutionPath path, int echoField, boolean supports32Locks, Collection<LockCondition> lockConditions) {
        ZscriptLockSet locks;
        if (lockConditions.isEmpty()) {
            locks = ZscriptLockSet.allLocked(supports32Locks);
        } else {
            locks = ZscriptLockSet.noneLocked(supports32Locks);
            for (LockCondition c : lockConditions) {
                c.apply(path, locks);
            }
        }
        return new CommandSequence(path, echoField, locks);
    }

    public static CommandSequence from(CommandExecutionPath path, int echoField, ZscriptLockSet locks) {
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
                    throw new IllegalArgumentException("Tokens contained two lock fields");
                }
                locks = ZscriptLockSet.parse(current, supports32Locks);
            } else {
                if (echoField != -1) {
                    throw new IllegalArgumentException("Tokens contained two echo fields");
                }
                echoField = current.getData16();
            }
            current = iter.next().orElse(null);
        }
        return new CommandSequence(CommandExecutionPath.parse(model, current), echoField, locks);
    }

    private final CommandExecutionPath executionPath;
    private final int                  echoField;
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

    public int getEchoValue() { //-1 if there isn't one
        return echoField;
    }

    @Nullable
    public ZscriptLockSet getLocks() {
        return locks;
    }

    public int getBufferLength() {
        return executionPath.getBufferLength() + (echoField > 0xff ? 4 : 3) + (locks != null ? locks.getBufferLength() : 0);
    }
}
