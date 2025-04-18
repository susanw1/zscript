package net.zscript.javaclient.commandbuilderapi;

import java.util.OptionalInt;

import net.zscript.model.components.Zchars;
import net.zscript.model.components.ZscriptStatus;
import net.zscript.tokenizer.ZscriptExpression;

public interface ZscriptResponse {
    /**
     * Performs response validation and returns result. If this is true, then all fields are set according to the model.
     *
     * @return true if response is valid, false otherwise
     */
    boolean isValid();

    /**
     * Accesses this response in expression form.
     *
     * @return a ZscriptExpression representation of this response
     */
    ZscriptExpression expression();

    OptionalInt getFieldValue(byte key);

    default OptionalInt getFieldValue(char key) {
        return getFieldValue((byte) key);
    }

    default boolean succeeded() {
        OptionalInt status = getFieldValue(Zchars.Z_STATUS);
        return status.isEmpty() || status.getAsInt() == ZscriptStatus.SUCCESS;
    }

    default boolean failed() {
        OptionalInt status = getFieldValue(Zchars.Z_STATUS);
        return status.isPresent() && ZscriptStatus.isFailure(status.getAsInt());
    }

    default boolean error() {
        OptionalInt status = getFieldValue(Zchars.Z_STATUS);
        return status.isPresent() && ZscriptStatus.isError(status.getAsInt());
    }
}
