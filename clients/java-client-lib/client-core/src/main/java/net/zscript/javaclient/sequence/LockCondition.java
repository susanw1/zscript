package net.zscript.javaclient.sequence;

import net.zscript.javaclient.commandpaths.CommandExecutionPath;

public interface LockCondition {
    void apply(CommandExecutionPath path, LockSet locks);

    static LockCondition createFromBits(int... lockBits) {
        return (path, locks) -> {
            for (int b : lockBits) {
                locks.setLock(b);
            }
        };
    }
}
