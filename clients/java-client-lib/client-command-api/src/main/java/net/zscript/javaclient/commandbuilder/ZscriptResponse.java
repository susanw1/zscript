package net.zscript.javaclient.commandbuilder;

import java.util.OptionalInt;

import net.zscript.javareceiver.tokenizer.BlockIterator;

public interface ZscriptResponse {
    /**
     * Performs response validation and returns result. If this is true, then all fields are set according to the model.
     *
     * @return true if response is valid, false otherwise
     */
    boolean isValid();

    OptionalInt getField(char key);

    BlockIterator getBigField();
}
