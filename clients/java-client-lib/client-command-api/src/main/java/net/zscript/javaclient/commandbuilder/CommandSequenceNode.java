package net.zscript.javaclient.commandbuilder;

/**
 * An element of a Command Sequence under construction, representing a node in the Syntax Tree of a sequence during building.
 */
public abstract class CommandSequenceNode {
    protected CommandSequenceNode parent = null;

    void setParent(CommandSequenceNode parent) {
        this.parent = parent;
    }

    CommandSequenceNode getParent() {
        return parent;
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

    public abstract boolean isCommand();

    public abstract boolean canFail();

    abstract CommandSequenceNode reEvaluate();

    protected abstract byte[] compile(boolean includeParens);

    public final byte[] compile() {
        return compile(false);
    }
}
