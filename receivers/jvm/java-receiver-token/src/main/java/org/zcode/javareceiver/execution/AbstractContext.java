package org.zcode.javareceiver.execution;

import org.zcode.javareceiver.semanticParser.ContextView;
import org.zcode.javareceiver.semanticParser.ContextView.AsyncActionNotifier;

abstract class AbstractContext {
    protected final ContextView contextView;

    protected AbstractContext(final ContextView contextView) {
        this.contextView = contextView;
        commandComplete();
    }

    /**
     * Updates the status of the current command (and sends the 'S' field), if it hasn't already been updated.
     *
     * @param status the status to set
     * @return true if changed/sent, false otherwise
     */
    public boolean status(byte status) {
        return contextView.requestStatusChange(status);
    }

    public void commandComplete() {
        contextView.setCommandComplete(true);
    }

    public void commandNotComplete() {
        contextView.setCommandComplete(false);
    }

    public boolean isCommandComplete() {
        return contextView.isCommandComplete();
    }

    public boolean isActivated() {
        return contextView.isActivated();
    }

    public AsyncActionNotifier getAsyncActionNotifier() {
        return contextView.getAsyncActionNotifier();
    }

}
