package net.zscript.javaclient.sequence;

import java.util.HashSet;
import java.util.Set;

import net.zscript.javaclient.commandPaths.Command;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;

public abstract class AbstractLockCondition implements LockCondition {
    @Override
    public void apply(CommandExecutionPath path, ZscriptLockSet locks) {
        Set<Command> visited = new HashSet<>();
        Set<Command> toVisit = new HashSet<>();
        toVisit.add(path.getFirstCommand());
        while (!toVisit.isEmpty()) {
            Set<Command> next = new HashSet<>();
            for (Command c : toVisit) {
                visited.add(c);
                checkCommand(c, locks);
                Command s = c.getEndLink().getOnSuccess();
                if (!visited.contains(s)) {
                    next.add(s);
                }
                Command f = c.getEndLink().getOnFail();
                if (!visited.contains(f)) {
                    next.add(f);
                }
            }
            toVisit = next;
        }
    }

    protected abstract void checkCommand(Command c, ZscriptLockSet locks);
}
