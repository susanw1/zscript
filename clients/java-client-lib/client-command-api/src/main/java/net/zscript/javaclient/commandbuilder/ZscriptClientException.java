package net.zscript.javaclient.commandbuilder;

import static java.lang.String.format;

public class ZscriptClientException extends RuntimeException {
    public ZscriptClientException(String format, Object... params) {
        super(format(format, params));
    }

    public ZscriptClientException(String msg, Exception e) {
        super(msg, e);
    }

    public ZscriptClientException(Exception e, String format, Object... params) {
        super(format(format, params), e);
    }
}
