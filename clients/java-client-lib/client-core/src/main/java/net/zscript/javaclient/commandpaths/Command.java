package net.zscript.javaclient.commandpaths;

import javax.annotation.Nullable;

import net.zscript.model.components.Zchars;
import net.zscript.model.components.ZscriptStatus;
import net.zscript.util.ByteString;
import net.zscript.util.ByteString.ByteAppendable;

/**
 * Represents a single command within a command-sequence, with its fields; it keeps track of the commands that follow it on success (ie AND_THEN) or on failure (ie OR_ELSE).
 */
public class Command implements ByteAppendable {
    private final ZscriptFieldSet fieldSet;

    private final Command onSuccess;
    private final Command onFail;

    public Command(@Nullable Command onSuccess, @Nullable Command onFail, ZscriptFieldSet fieldSet) {
        this.onSuccess = onSuccess;
        this.onFail = onFail;
        this.fieldSet = fieldSet;
    }

    /**
     * Gets the Command which will run in this sequence if this Command is successful, based on the interpretation of the sequence logic.
     *
     * @return the command to run on success, or null if there is no following command to run on success
     */
    @Nullable
    public Command getOnSuccess() {
        return onSuccess;
    }

    /**
     * Gets the Command which will run in this sequence if this Command fails, based on the interpretation of the sequence logic. (Note that FAILURE is distinct from ERROR,
     * whereupon the sequence terminates)
     *
     * @return the command to run on failure, or null if there is no following command to run on failure
     */
    @Nullable
    public Command getOnFail() {
        return onFail;
    }

    @Override
    public void appendTo(ByteString.ByteStringBuilder builder) {
        fieldSet.appendTo(builder);
    }

    /**
     * Determines whether any fields (numeric or big-fields) are set.
     *
     * @return true if no fields are set, false otherwise
     */
    public boolean isEmpty() {
        return fieldSet.isEmpty();
    }

    /**
     * In general, all commands may fail. Special cases: empty command always succeeds, and echo with a success status (used somewhat in command-builder machinery to force success
     * conditions).
     *
     * @return true, unless it's empty or a success echo command
     */
    public boolean canFail() {
        // can't fail if it's the trivial empty command
        if (isEmpty()) {
            return false;
        }
        // special case: "Z1 S{status}" command, where {status} is a failure code (esp COMMAND_FAIL_CONTROL)
        if (fieldSet.getFieldVal(Zchars.Z_CMD) == 1) {
            int sVal = fieldSet.getFieldVal(Zchars.Z_STATUS);
            return ZscriptStatus.isFailure(sVal);
        }
        return true;
    }

    /**
     * In general, all commands may succeed. Special case: echo with a non-success status (used somewhat in command-builder machinery to force "orFail" conditions).
     *
     * @return true, unless it's a non-success echo command
     */
    public boolean canSucceed() {
        // special case: "Z1" command, where 'S' is missing, or has a zero (success) code
        if (fieldSet.getFieldVal(Zchars.Z_CMD) == 1) {
            int sVal = fieldSet.getFieldVal(Zchars.Z_STATUS);
            return sVal == -1 || ZscriptStatus.isSuccess(sVal);
        }
        return true;
    }

    public int getBufferLength() {
        return fieldSet.getBufferLength();
    }

    @Override
    public String toString() {
        return toStringImpl();
    }

    public ZscriptFieldSet getFields() {
        return fieldSet;
    }
}
