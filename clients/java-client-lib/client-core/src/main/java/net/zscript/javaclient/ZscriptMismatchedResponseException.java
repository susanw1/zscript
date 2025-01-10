package net.zscript.javaclient;

public class ZscriptMismatchedResponseException extends ZscriptParseException {
    public ZscriptMismatchedResponseException(String format, Object... params) {
        super(format, params);
    }

    public ZscriptMismatchedResponseException(String format, Exception cause, Object... params) {
        super(format, cause, params);
    }
}
