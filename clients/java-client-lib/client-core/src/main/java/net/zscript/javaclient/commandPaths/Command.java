package net.zscript.javaclient.commandPaths;

import net.zscript.javaclient.util.ZscriptByteString;

public class Command {
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

    public void toBytes(ZscriptByteString.ZscriptByteStringBuilder out) {
        fieldSet.toBytes(out);
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
            if (sVal > 0x00 && sVal < 0x10) {
                return true;
            } else {
                return false;
            }
        }
        return true;
    }

    public boolean canSucceed() {
        if (fieldSet.getFieldValue('Z') == 1) {
            int sVal = fieldSet.getFieldValue('S');
            if (sVal == 0 || sVal == -1) {
                return true;
            } else {
                return false;
            }
        }
        return true;
    }

    public int getBufferLength() {
        return fieldSet.getBufferLength();
    }

    @Override
    public String toString() {
        ZscriptByteString.ZscriptByteStringBuilder out = ZscriptByteString.builder();
        toBytes(out);
        return out.asString();
    }

    public ZscriptFieldSet getFields() {
        return fieldSet;
    }

}
