package net.zscript.javaclient;

import static java.lang.String.format;

public class ZscriptClientException extends RuntimeException {
    /**
     * Creates a generic zscript client exception, allowing {@link String#format(String, Object...)}-style format strings with params.
     *
     * @param format a format string with optional standard '%s'-style format elements
     * @param params the params, matching the format specifiers in the format string
     */
    public ZscriptClientException(String format, Object... params) {
        super(format(format, params));
    }

    /**
     * Creates a generic zscript client exception, allowing a chained "cause" exception plus {@link String#format(String, Object...)}-style format strings with params. Annoyingly,
     * as the varargs param list has to be the last argument, the 'cause' is a bit stuck in the middle.
     *
     * @param format a format string with optional standard '%s'-style format elements
     * @param cause  the upstream exception which caused this exception
     * @param params the params, matching the format specifiers in the format string
     */
    public ZscriptClientException(String format, Exception cause, Object... params) {
        super(format(format, params), cause);
    }
}
