package net.zscript.javaclient.sequence;

import static net.zscript.javaclient.commandPaths.NumberField.fieldOf;

import net.zscript.javaclient.commandPaths.ResponseExecutionPath;
import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.TokenBufferIterator;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

public final class ResponseSequence implements ByteAppendable {
    private final ResponseExecutionPath executionPath;

    private final int     echoField;
    private final int     responseField;
    private final boolean timedOut;

    public static ResponseSequence parse(TokenBuffer.TokenReader.ReadToken start) {
        int echoField     = -1;
        int responseField = -1;

        TokenBufferIterator iter = start.tokenIterator();

        TokenBuffer.TokenReader.ReadToken current = iter.next().orElse(null);
        if (current == null || current.getKey() != Zchars.Z_RESPONSE_MARK) {
            throw new IllegalArgumentException("Invalid response sequence without response mark");
        }
        responseField = current.getData16();
        current = iter.next().orElse(null);
        if (current != null && current.getKey() == Zchars.Z_ECHO) {
            echoField = current.getData16();
            current = iter.next().orElse(null);
        }
        return new ResponseSequence(ResponseExecutionPath.parse(current), echoField, responseField, false);
    }

    public static ResponseSequence empty() {
        return new ResponseSequence(ResponseExecutionPath.blank(), -1, -1, false);
    }

    public static ResponseSequence timedOut() {
        return new ResponseSequence(ResponseExecutionPath.blank(), -1, -1, true);
    }

    private ResponseSequence(ResponseExecutionPath executionPath, int echoField, int responseField, boolean timedOut) {
        this.executionPath = executionPath;
        this.echoField = echoField;
        this.responseField = responseField;
        this.timedOut = timedOut;
    }

    public ResponseExecutionPath getExecutionPath() {
        return executionPath;
    }

    public int getEchoValue() {
        return echoField;
    }

    public boolean hasEchoValue() {
        return echoField != -1;
    }

    public int getResponseValue() {
        return responseField;
    }

    @Override
    public void appendTo(ByteStringBuilder builder) {
        // FIXME: should there only be a '!' when it's been set? Can this even happen?
        if (responseField != -1) {
            fieldOf(Zchars.Z_RESPONSE_MARK, responseField).appendTo(builder);
        }
        if (echoField != -1) {
            fieldOf(Zchars.Z_ECHO, echoField).appendTo(builder);
        }
        executionPath.appendTo(builder);
    }
}
