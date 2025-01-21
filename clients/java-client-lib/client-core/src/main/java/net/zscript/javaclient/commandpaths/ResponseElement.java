package net.zscript.javaclient.commandpaths;

import javax.annotation.Nullable;

import static java.lang.System.identityHashCode;

import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

/**
 * Component of a {@link net.zscript.javaclient.sequence.ResponseSequence}
 */
public final class ResponseElement implements ByteAppendable {
    private final ZscriptFieldSet fieldSet;

    private final ResponseElement next;
    private final boolean         wasSuccess;
    private final boolean         isOpenBracket;
    private final boolean         isCloseBracket;
    private final int             bracketCount;

    /**
     * Creates Response object, to be linked to represent an element of a response sequence, as per ResponseExecutionPath parser.
     *
     * @param next           simply the response from the command that was executed after this one; if null, represents an empty (always-true) response (eg 'A & & B')
     * @param wasSuccess     true if this response is followed by something that isn't an OR-ELSE ('|') marker, so must have succeeded
     * @param isOpenBracket  this response was followed by an open parenthesis
     * @param isCloseBracket this response was followed by a close parenthesis
     * @param bracketCount
     * @param fieldSet       the fields relating to this response
     */
    ResponseElement(@Nullable ResponseElement next, boolean wasSuccess, boolean isOpenBracket, boolean isCloseBracket, int bracketCount, ZscriptFieldSet fieldSet) {
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
    public ResponseElement getNext() {
        return next;
    }

    @Override
    public void appendTo(ByteStringBuilder out) {
        fieldSet.appendTo(out);
    }

    void separatorBytes(ByteStringBuilder builder) {
        if (wasSuccess) {
            if (isOpenBracket) {
                builder.appendByte(Zchars.Z_OPENPAREN);
            } else if (isCloseBracket) {
                builder.appendByte(Zchars.Z_CLOSEPAREN);
            } else {
                builder.appendByte(Zchars.Z_ANDTHEN);
            }
        } else {
            if (isCloseBracket) {
                builder.appendByte(Zchars.Z_CLOSEPAREN);
            } else {
                for (int i = 0; i < bracketCount; i++) {
                    builder.appendByte(Zchars.Z_CLOSEPAREN);
                }
                builder.appendByte(Zchars.Z_ORELSE);
            }
        }
    }

    @Override
    public String toString() {
        return toStringImpl();
    }

    public ZscriptFieldSet getFields() {
        return fieldSet;
    }

    boolean hasOpenParen() {
        return isOpenBracket;
    }

    boolean hasCloseParen() {
        return isCloseBracket;
    }

    int getParenCount() {
        return bracketCount;
    }

    /**
     * Equality checks are identity-based.
     *
     * @return the identityHashCode
     */
    @Override
    public final int hashCode() {
        return identityHashCode(this);
    }

    /**
     * Equality checks are identity-based.
     *
     * @param obj the object to compare
     * @return true if this == obj, false otherwise
     */
    @Override
    public final boolean equals(Object obj) {
        return this == obj;
    }
}
