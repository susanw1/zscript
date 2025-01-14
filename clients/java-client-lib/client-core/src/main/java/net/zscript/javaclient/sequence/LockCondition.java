package net.zscript.javaclient.sequence;

import net.zscript.javaclient.commandpaths.CommandExecutionPath;

public interface LockCondition {
    void apply(CommandExecutionPath path, ZscriptLockSet locks);
}
