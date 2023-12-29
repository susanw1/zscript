package net.zscript.javaclient.devices;

public class CommandFailedException extends RuntimeException {
    private final ResponseSequenceCallback callback;

    public CommandFailedException(ResponseSequenceCallback callback) {
        this.callback = callback;
    }

    public ResponseSequenceCallback getCallback() {
        return callback;
    }
}
