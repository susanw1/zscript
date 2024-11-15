package net.zscript.javaclient.commandbuilder;

import net.zscript.javaclient.ZscriptClientException;

public class ZscriptMissingFieldException extends ZscriptClientException {

    public ZscriptMissingFieldException(String format, Object... params) {
        super(format, params);
    }

    public ZscriptMissingFieldException(String format, Exception cause, Object... params) {
        super(format, cause, params);
    }
}
