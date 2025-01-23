package net.zscript.javaclient.sequence;

import net.zscript.javaclient.commandpaths.CommandElement;
import net.zscript.javaclient.commandpaths.CommandExecutionPath;

public abstract class AbstractLockCondition implements LockCondition {
    @Override
    public void apply(CommandExecutionPath path, LockSet locks) {
        for (CommandElement c : path) {
            checkCommand(c, locks);
        }
    }

    protected abstract void checkCommand(CommandElement c, LockSet locks);
}
