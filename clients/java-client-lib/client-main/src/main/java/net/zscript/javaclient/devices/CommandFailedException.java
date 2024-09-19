package net.zscript.javaclient.devices;

import javax.annotation.Nonnull;

import static java.util.Objects.requireNonNull;

public class CommandFailedException extends RuntimeException {
    private final ResponseSequenceCallback callback;

    public CommandFailedException(ResponseSequenceCallback callback) {
        this.callback = requireNonNull(callback, "callback is null");
    }

    @Nonnull
    public ResponseSequenceCallback getCallback() {
        return callback;
    }
}
