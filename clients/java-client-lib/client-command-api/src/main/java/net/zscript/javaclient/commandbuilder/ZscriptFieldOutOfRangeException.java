package net.zscript.javaclient.commandbuilder;

import net.zscript.javaclient.ZscriptClientException;

public class ZscriptFieldOutOfRangeException extends ZscriptClientException {

    public ZscriptFieldOutOfRangeException(String format, Object... params) {
        super(format, params);
    }

    public ZscriptFieldOutOfRangeException(String format, Exception e, Object... params) {
        super(format, e, params);
    }
}
