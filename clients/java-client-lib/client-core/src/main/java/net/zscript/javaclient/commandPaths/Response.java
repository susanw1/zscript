package net.zscript.javaclient.commandPaths;

import javax.annotation.Nullable;

import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

public final class Response implements ByteAppendable {
    private final ZscriptFieldSet fieldSet;

    private final Response next;
    private final boolean  wasSuccess;
    private final boolean  isOpenBracket;
    private final boolean  isCloseBracket;
    private final int      bracketCount;

    Response(@Nullable Response next, boolean wasSuccess, boolean isOpenBracket, boolean isCloseBracket, int bracketCount, ZscriptFieldSet fieldSet) {
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

    @Nullable
    public Response getNext() {
        return next;
    }

    @Override
    public void appendTo(ByteStringBuilder out) {
        fieldSet.appendTo(out);
    }

    public void separatorBytes(ByteStringBuilder builder) {
        if (wasSuccess) {
            if (isOpenBracket) {
                builder.appendByte(Zchars.Z_OPENPAREN);
            } else if (isCloseBracket) {
                builder.appendByte(Zchars.Z_CLOSEPAREN);
            } else {
                builder.appendByte(Zchars.Z_ANDTHEN);
            }
        } else {
            for (int i = 0; i < bracketCount; i++) {
                builder.appendByte(Zchars.Z_CLOSEPAREN);
            }
            builder.appendByte(Zchars.Z_ORELSE);
        }
    }

    @Override
    public String toString() {
        return toByteString().asString();
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
