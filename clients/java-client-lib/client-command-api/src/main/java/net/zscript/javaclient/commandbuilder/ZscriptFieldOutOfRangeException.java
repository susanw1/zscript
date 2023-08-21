package net.zscript.javaclient.commandbuilder;

import static java.lang.String.format;

public class ZscriptFieldOutOfRangeException extends ZscriptClientException {

    public ZscriptFieldOutOfRangeException(String format, Object... params) {
        super(format(format, params));
    }

    public ZscriptFieldOutOfRangeException(String msg, Exception e) {
        super(msg, e);
    }
}
