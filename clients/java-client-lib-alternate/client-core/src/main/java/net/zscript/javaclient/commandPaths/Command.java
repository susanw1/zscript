package net.zscript.javaclient.commandPaths;

import net.zscript.javaclient.commandbuilder.ZscriptByteString;
import net.zscript.util.ByteString;

public class Command {

    public static class CommandEndLink {
        private final Command onSuccess;
        private final byte[]  successSeparators;
        private final Command onFail;
        private final byte[]  failSeparators;

        CommandEndLink(Command onSuccess, byte[] successSeparators, Command onFail, byte[] failSeparators) {
            this.onSuccess = onSuccess;
            this.successSeparators = successSeparators;
            this.onFail = onFail;
            this.failSeparators = failSeparators;
        }

        public Command getOnSuccess() {
            return onSuccess;
        }

        public byte[] getSuccessSeparators() {
            return successSeparators;
        }

        public Command getOnFail() {
            return onFail;
        }

        public byte[] getFailSeparators() {
            return failSeparators;
        }
    }

    private final CommandEndLink  endLink;
    private final ZscriptFieldSet fieldSet;

    Command(CommandEndLink endLink, ZscriptFieldSet fieldSet) {
        this.endLink = endLink;
        this.fieldSet = fieldSet;
    }

    public CommandEndLink getEndLink() {
        return endLink;
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
