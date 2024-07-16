package net.zscript.javaclient.commandPaths;

import net.zscript.util.ByteString;
import net.zscript.util.ByteString.ByteAppendable;

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
        if (isEmpty()) {
            return false;
        }
        if (fieldSet.getFieldValue('Z') == 1) {
            int sVal = fieldSet.getFieldValue('S');
            return sVal > 0x00 && sVal < 0x10;
        }
        return true;
    }

    public boolean canSucceed() {
        if (fieldSet.getFieldValue('Z') == 1) {
            int sVal = fieldSet.getFieldValue('S');
            return sVal == 0 || sVal == -1;
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
