package net.zscript.javaclient;

public class ZscriptParseException extends ZscriptClientException {
    public ZscriptParseException(String format, Object... params) {
        super(format, params);
    }

    public ZscriptParseException(String format, Exception cause, Object... params) {
        super(format, cause, params);
    }
}
