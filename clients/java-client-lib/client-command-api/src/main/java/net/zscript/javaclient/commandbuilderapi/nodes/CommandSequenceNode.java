package net.zscript.javaclient.commandbuilderapi.nodes;

import javax.annotation.Nonnull;
import java.util.Iterator;
import java.util.List;
import java.util.NoSuchElementException;

import net.zscript.javaclient.commandbuilderapi.defaultcommands.AbortCommandNode;
import net.zscript.javaclient.commandbuilderapi.defaultcommands.BlankCommandNode;
import net.zscript.javaclient.commandbuilderapi.defaultcommands.FailureCommandNode;
import net.zscript.util.ByteString.ByteAppendable;

/**
 * An element of a Command Sequence under construction, representing a node in the Syntax Tree of a sequence during building.
 */
public abstract class CommandSequenceNode implements Iterable<ZscriptCommandNode<?>>, ByteAppendable {
    private CommandSequenceNode parent = null;

    void setParent(CommandSequenceNode parent) {
        this.parent = parent;
    }

    CommandSequenceNode getParent() {
        return parent;
    }

    public static SuccessConditional ifSucceeds(CommandSequenceNode condition) {
        return condition.ifSucceeds();
    }

    public static FailureConditional ifFails(CommandSequenceNode condition) {
        return condition.ifFails();
    }

    public CommandSequenceNode andThen(CommandSequenceNode next) {
        return new AndSequenceNode(this, next);
    }

    public CommandSequenceNode dropFailureCondition() {
        if (canFail()) {
            return onFail(new BlankCommandNode());
        }
        return this;
    }

    public CommandSequenceNode thenFail() {
        return andThen(new FailureCommandNode());
    }

    public CommandSequenceNode thenAbort() {
        return andThen(new AbortCommandNode());
    }

    public CommandSequenceNode onFail(CommandSequenceNode next) {
        if (canFail()) {
            return new OrSequenceNode(this, next);
        }
        return this;
    }

    public CommandSequenceNode abortOnFail() {
        return onFail(new AbortCommandNode());
    }

    public SuccessConditional ifSucceeds() {
        return success -> failure -> andThen(success.dropFailureCondition()).onFail(failure);
    }

    public FailureConditional ifFails() {
        return failure -> success -> andThen(success.dropFailureCondition()).onFail(failure);
    }

    /**
     * Distinguishes an executable syntax-tree node from a logical AND/OR group.
     *
     * @return true if this is a Command, false if it's a logic group.
     */
    boolean isCommand() {
        return true;
    }

    /**
     * Override if command never produces a FAIL (S1-Sf) condition, which allows builder to ignore ORELSE choices (this is an optimization, if in doubt, leave it!).
     *
     * @return true if command may fail, false otherwise
     */
    protected boolean canFail() {
        return true;
    }

    /**
     * Allows a node to be optimized to remove unused structured, esp for commands with no "failure" conditions.
     *
     * @return a rewritten node.
     */
    CommandSequenceNode optimize() {
        return this;
    }

    public abstract List<CommandSequenceNode> getChildren();

    @Nonnull
    @Override
    public Iterator<ZscriptCommandNode<?>> iterator() {
        if (this instanceof ZscriptCommandNode<?>) {
            return List.<ZscriptCommandNode<?>>of((ZscriptCommandNode<?>) this).iterator();
        }
        return new Iterator<>() {
            final Iterator<CommandSequenceNode> children = getChildren().iterator();
            Iterator<ZscriptCommandNode<?>> childIter = null;

            @Override
            public boolean hasNext() {
                return (childIter != null && childIter.hasNext()) || children.hasNext();
            }

            @Override
            public ZscriptCommandNode<?> next() {
                while (childIter == null || !childIter.hasNext()) {
                    if (!children.hasNext()) {
                        throw new NoSuchElementException();
                    }
                    final CommandSequenceNode node = children.next();
                    if (node instanceof ZscriptCommandNode) {
                        return (ZscriptCommandNode<?>) node;
                    }
                    childIter = node.iterator();
                }
                return childIter.next();
            }
        };
    }

    public interface SuccessConditional {
        SuccessConditionalElse then(CommandSequenceNode cmdsOnSuccess);
    }

    public interface SuccessConditionalElse {
        CommandSequenceNode otherwise(CommandSequenceNode cmdsOnFail);
    }

    public interface FailureConditional {
        SuccessConditionalElse then(CommandSequenceNode cmdsOnFail);
    }

    public interface FailureConditionalElse {
        CommandSequenceNode otherwise(CommandSequenceNode cmdsOnSuccess);
    }

}
