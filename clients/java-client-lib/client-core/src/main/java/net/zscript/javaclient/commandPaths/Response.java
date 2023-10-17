package net.zscript.javaclient.commandPaths;

import net.zscript.javaclient.ZscriptByteString;
import net.zscript.model.components.Zchars;

public class Response {
    private final ZscriptFieldSet fieldSet;

    private final Response next;
    private final boolean  wasSuccess;
    private final boolean  isOpenBracket;
    private final boolean  isCloseBracket;
    private final int      bracketCount;

    Response(Response next, boolean wasSuccess, boolean isOpenBracket, boolean isCloseBracket, int bracketCount, ZscriptFieldSet fieldSet) {
        this.next = next;
        this.wasSuccess = wasSuccess;
        this.isOpenBracket = isOpenBracket;
        this.isCloseBracket = isCloseBracket;
        this.bracketCount = bracketCount;
        this.fieldSet = fieldSet;
    }

    public boolean wasSuccess() {
        return wasSuccess;
    }

    public Response getNext() {
        return next;
    }

    public void toBytes(ZscriptByteString.ZscriptByteStringBuilder out) {
        fieldSet.toBytes(out);
    }

    public void seperatorBytes(ZscriptByteString.ZscriptByteStringBuilder out) {
        if (wasSuccess) {
            if (isOpenBracket) {
                out.appendByte(Zchars.Z_OPENPAREN);
            } else if (isCloseBracket) {
                out.appendByte(Zchars.Z_CLOSEPAREN);
            } else {
                out.appendByte(Zchars.Z_ANDTHEN);
            }
        } else {
            for (int i = 0; i < bracketCount; i++) {
                out.appendByte(Zchars.Z_CLOSEPAREN);
            }
            out.appendByte(Zchars.Z_ORELSE);
        }
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

    public boolean hasOpenParen() {
        return isOpenBracket;
    }

    public boolean hasCloseParen() {
        return isCloseBracket;
    }

    public int getParenCount() {
        return bracketCount;
    }
}
