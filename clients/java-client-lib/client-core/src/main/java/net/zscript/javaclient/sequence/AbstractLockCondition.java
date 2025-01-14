package net.zscript.javaclient.sequence;

import net.zscript.javaclient.commandpaths.Command;
import net.zscript.javaclient.commandpaths.CommandExecutionPath;

public abstract class AbstractLockCondition implements LockCondition {
    @Override
    public void apply(CommandExecutionPath path, ZscriptLockSet locks) {
        for (Command c : path) {
            checkCommand(c, locks);
        }
    }

    protected abstract void checkCommand(Command c, ZscriptLockSet locks);
}
