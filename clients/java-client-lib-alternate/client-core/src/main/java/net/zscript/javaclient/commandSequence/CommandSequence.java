package net.zscript.javaclient.commandSequence;

import java.util.Collection;

import static net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;

import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.commandbuilder.ZscriptByteString;
import net.zscript.javareceiver.tokenizer.TokenBuffer;
import net.zscript.javareceiver.tokenizer.TokenBufferIterator;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;

public class CommandSequence {
    private final CommandExecutionPath executionPath;
    private final int                  echoField;
    private final ZscriptLockSet       locks;

    public CommandSequence from(CommandExecutionPath path, int echoField) {
        return from(path, echoField, false);
    }

    public CommandSequence from(CommandExecutionPath path, int echoField, boolean supports32Locks) {
        return new CommandSequence(path, echoField, ZscriptLockSet.allLocked(supports32Locks));
    }

    public CommandSequence from(CommandExecutionPath path, int echoField, boolean supports32Locks, Collection<LockCondition> lockConditions) {
        ZscriptLockSet locks = ZscriptLockSet.noneLocked(supports32Locks);
        for (LockCondition c : lockConditions) {
            c.apply(path, locks);
        }
        return new CommandSequence(path, echoField, locks);
    }

    public CommandSequence from(CommandExecutionPath path, int echoField, ZscriptLockSet locks) {
        return new CommandSequence(path, echoField, locks);
    }

    public CommandSequence parse(ReadToken start, boolean supports32Locks) {
        ZscriptLockSet      locks     = null;
        int                 echoField = -1;
        TokenBufferIterator iter      = start.getNextTokens();
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
        return new CommandSequence(CommandExecutionPath.parse(current), echoField, locks);
    }

    private CommandSequence(CommandExecutionPath executionPath, int echoField, ZscriptLockSet locks) {
        this.executionPath = executionPath;
        this.echoField = echoField;
        this.locks = locks;
    }

    public ByteString toBytes() {
        ZscriptByteString.ZscriptByteStringBuilder builder = ZscriptByteString.builder();
        locks.toBytes(builder);
        if (echoField != -1) {
            builder.appendField(Zchars.Z_ECHO, echoField);
        }
        builder.append(executionPath.toSequence());
        return builder.build();
    }

    public CommandExecutionPath getExecutionPath() {
        return executionPath;
    }
}
