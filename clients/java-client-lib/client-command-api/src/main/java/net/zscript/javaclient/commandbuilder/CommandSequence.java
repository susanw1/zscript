package net.zscript.javaclient.commandbuilder;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.UncheckedIOException;

import net.zscript.model.components.Zchars;

/**
 * An element of a Command Sequence under construction, representing a node in the Syntax Tree of a sequence during building.
 */
public abstract class CommandSequence {
    protected CommandSequence parent = null;

    void setParent(CommandSequence parent) {
        this.parent = parent;
    }

    CommandSequence getParent() {
        return parent;
    }

    public interface SuccessConditional {
        SuccessConditionalElse then(CommandSequence cmdsOnSuccess);
    }

    public interface SuccessConditionalElse {
        CommandSequence otherwise(CommandSequence cmdsOnFail);
    }

    public interface FailureConditional {
        SuccessConditionalElse then(CommandSequence cmdsOnFail);
    }

    public interface FailureConditionalElse {
        CommandSequence otherwise(CommandSequence cmdsOnSuccess);
    }

    public static SuccessConditional ifSucceeds(CommandSequence condition) {
        return condition.ifSucceeds();
    }

    public static FailureConditional ifFails(CommandSequence condition) {
        return condition.ifFails();
    }

    public CommandSequence andThen(CommandSequence next) {
        return new AndSequence(this, next);
    }

    public CommandSequence dropFailureCondition() {
        if (canFail()) {
            return onFail(new BlankCommand());
        }
        return this;
    }

    public CommandSequence thenFail() {
        return andThen(new FailureCommand());
    }

    public CommandSequence thenAbort() {
        return andThen(new AbortCommand());
    }

    public CommandSequence onFail(CommandSequence next) {
        if (canFail()) {
            return new OrSequence(this, next);
        }
        return this;
    }

    public CommandSequence abortOnFail() {
        return onFail(new AbortCommand());
    }

    public SuccessConditional ifSucceeds() {
        return success -> failure -> andThen(success.dropFailureCondition()).onFail(failure);
    }

    public FailureConditional ifFails() {
        return failure -> success -> andThen(success.dropFailureCondition()).onFail(failure);
    }

    public abstract boolean isCommand();

    public abstract boolean canFail();

    abstract CommandSequence reEvaluate();

    protected abstract byte[] compile(boolean includeParens);

    public final byte[] compile() {
        byte[]                compiled              = compile(false);
        ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream(compiled.length + 1);
        try {
            byteArrayOutputStream.write(compiled);
        } catch (IOException e) {
            // this should be impossible
            throw new UncheckedIOException(e);
        }
        byteArrayOutputStream.write(Zchars.Z_NEWLINE);
        return byteArrayOutputStream.toByteArray();
    }
}
