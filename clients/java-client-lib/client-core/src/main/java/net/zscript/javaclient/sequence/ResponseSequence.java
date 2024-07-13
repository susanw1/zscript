package net.zscript.javaclient.sequence;

import net.zscript.javaclient.commandPaths.ResponseExecutionPath;
import net.zscript.javaclient.util.ZscriptByteString;
import net.zscript.javareceiver.tokenizer.TokenBuffer;
import net.zscript.javareceiver.tokenizer.TokenBufferIterator;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;

public class ResponseSequence {
    private final ResponseExecutionPath executionPath;

    private final int     echoField;
    private final int     responseField;
    private final boolean timedOut;

    public static ResponseSequence parse(TokenBuffer.TokenReader.ReadToken start) {
        if (start == null) {
            return new ResponseSequence(ResponseExecutionPath.blank(), -1, -1, false);
        }
        int                 echoField     = -1;
        int                 responseField = -1;
        TokenBufferIterator iter          = start.getNextTokens();

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

    public static ResponseSequence blank() {
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

    public ByteString toSequence() {
        ZscriptByteString.ZscriptByteStringBuilder out = ZscriptByteString.builder();
        toSequence(out);
        return out.build();
    }

    public void toSequence(ZscriptByteString.ZscriptByteStringBuilder out) {
        if (responseField != -1) {
            out.appendField(Zchars.Z_RESPONSE_MARK, responseField);
        }
        if (echoField != -1) {
            out.appendField(Zchars.Z_ECHO, echoField);
        }
        executionPath.toSequence(out);
    }

}
