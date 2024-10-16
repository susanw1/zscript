package net.zscript.javaclient.commandbuilder;

import java.util.OptionalInt;

import net.zscript.model.components.Zchars;
import net.zscript.model.components.ZscriptStatus;

public interface ZscriptResponse {
    /**
     * Performs response validation and returns result. If this is true, then all fields are set according to the model.
     *
     * @return true if response is valid, false otherwise
     */
    boolean isValid();

    OptionalInt getField(byte key);

    default boolean succeeded() {
        OptionalInt status = getField(Zchars.Z_STATUS);
        return status.isEmpty() || status.getAsInt() == ZscriptStatus.SUCCESS;
    }

    default boolean failed() {
        OptionalInt status = getField(Zchars.Z_STATUS);
        return status.isPresent() && ZscriptStatus.isFailure(status.getAsInt());
    }

    default boolean error() {
        OptionalInt status = getField(Zchars.Z_STATUS);
        return status.isPresent() && ZscriptStatus.isError(status.getAsInt());
    }
}
