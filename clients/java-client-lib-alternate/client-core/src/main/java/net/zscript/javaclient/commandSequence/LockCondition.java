package net.zscript.javaclient.commandSequence;

import net.zscript.javaclient.commandPaths.CommandExecutionPath;

public interface LockCondition {
    void apply(CommandExecutionPath path, ZscriptLockSet locks);
}
