package net.zscript.javaclient.zcodeApi;

public abstract class CommandSeqElement {
    protected CommandSeqElement parent = null;

    void setParent(CommandSeqElement parent) {
        this.parent = parent;
    }

    CommandSeqElement getParent() {
        return parent;
    }

    public interface SuccessConditional {
        SuccessConditionalElse then(CommandSeqElement cmdsOnSuccess);
    }

    public interface SuccessConditionalElse {
        CommandSeqElement otherwise(CommandSeqElement cmdsOnFail);
    }

    public interface FailureConditional {
        SuccessConditionalElse then(CommandSeqElement cmdsOnFail);
    }

    public interface FailureConditionalElse {
        CommandSeqElement otherwise(CommandSeqElement cmdsOnSuccess);
    }

    public static SuccessConditional ifSucceeds(CommandSeqElement condition) {
        return condition.ifSucceeds();
    }

    public static FailureConditional ifFails(CommandSeqElement condition) {
        return condition.ifFails();
    }

    public CommandSeqElement andThen(CommandSeqElement next) {
        return new ZcodeAndSeqElement(this, next);
    }

    public CommandSeqElement dropFailureCondition() {
        if (canFail()) {
            return onFail(new ZcodeBlankCommand());
        }
        return this;
    }

    public CommandSeqElement thenFail() {
        return andThen(new ZcodeFailureCommand());
    }

    public CommandSeqElement thenAbort() {
        return andThen(new ZcodeAbortCommand());
    }

    public CommandSeqElement onFail(CommandSeqElement next) {
        if (canFail()) {
            return new ZcodeOrSeqElement(this, next);
        }
        return this;
    }

    public CommandSeqElement abortOnFail() {
        return onFail(new ZcodeAbortCommand());
    }

    public SuccessConditional ifSucceeds() {
        return success -> failure -> andThen(success.dropFailureCondition()).onFail(failure);
    }

    public FailureConditional ifFails() {
        return failure -> success -> andThen(success.dropFailureCondition()).onFail(failure);
    }

    public abstract boolean isCommand();

    public abstract boolean canFail();

    abstract CommandSeqElement reEvaluate();

    public abstract byte[] compile(boolean includeParens);
}
