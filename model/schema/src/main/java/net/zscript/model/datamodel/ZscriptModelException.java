package net.zscript.model.datamodel;

import static java.lang.String.format;

/**
 * Indicates a problem with the structure of the Zscript model definitions.
 */
public class ZscriptModelException extends RuntimeException {
    /**
     * Formattable exception.
     *
     * @param format a {@link String#format(String, Object...)} format string
     * @param params the params to the format string
     */
    public ZscriptModelException(String format, Object... params) {
        super(format(format, params));
    }

    /**
     * Formattable exception. Sorry we put the cause as the 1st arg, but hey vararg lists make it necessary.
     *
     * @param cause  chained exception
     * @param format a {@link String#format(String, Object...)} format string
     * @param params the params to the format string
     */
    public ZscriptModelException(Exception cause, String format, Object... params) {
        super(format(format, params), cause);
    }
}
