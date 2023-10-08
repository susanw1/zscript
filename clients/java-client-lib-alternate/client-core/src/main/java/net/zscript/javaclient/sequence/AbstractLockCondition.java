package net.zscript.javaclient.sequence;

import java.util.HashSet;
import java.util.Set;

import net.zscript.javaclient.commandPaths.Command;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;

public abstract class AbstractLockCondition implements LockCondition {
    @Override
    public void apply(CommandExecutionPath path, ZscriptLockSet locks) {
        for (Command c : path) {
            checkCommand(c, locks);
        }
    }

    protected abstract void checkCommand(Command c, ZscriptLockSet locks);
}
