package net.zscript.javaclient.commandPaths;

import net.zscript.model.components.Zchars;
import net.zscript.model.components.ZscriptStatus;
import net.zscript.util.ByteString;
import net.zscript.util.ByteString.ByteAppendable;

/**
 * Represents a single command within a command-sequence, with its fields; it keeps track of the commands that follow it onSuccess
 */
public class Command implements ByteAppendable {
    private final ZscriptFieldSet fieldSet;

    private final Command onSuccess;
    private final Command onFail;

    public Command(Command onSuccess, Command onFail, ZscriptFieldSet fieldSet) {
        this.onSuccess = onSuccess;
        this.onFail = onFail;
        this.fieldSet = fieldSet;
    }

    public Command getOnSuccess() {
        return onSuccess;
    }

    public Command getOnFail() {
        return onFail;
    }

    @Override
    public void appendTo(ByteString.ByteStringBuilder builder) {
        fieldSet.appendTo(builder);
    }

    public boolean isEmpty() {
        return fieldSet.isEmpty();
    }

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
        return toByteString().asString();
    }

    public ZscriptFieldSet getFields() {
        return fieldSet;
    }
}
