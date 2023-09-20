package net.zscript.model.datamodel;

import static java.lang.String.format;

public class ZscriptModelException extends RuntimeException {
    public ZscriptModelException(String format, Object... params) {
        super(format(format, params));
    }
}
