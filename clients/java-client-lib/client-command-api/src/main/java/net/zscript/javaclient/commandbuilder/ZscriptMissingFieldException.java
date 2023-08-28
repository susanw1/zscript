package net.zscript.javaclient.commandbuilder;

import static java.lang.String.format;

public class ZscriptMissingFieldException extends ZscriptClientException {

    public ZscriptMissingFieldException(String format, Object... params) {
        super(format(format, params));
    }

    public ZscriptMissingFieldException(String msg, Exception e) {
        super(msg, e);
    }

}
