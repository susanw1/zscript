package org.zcode.javareceiver.execution;

import org.zcode.javareceiver.semanticParser.ContextView;
import org.zcode.javareceiver.semanticParser.ContextView.AsyncActionNotifier;

abstract class AbstractContext {
    protected final ContextView contextView;

    protected AbstractContext(final ContextView contextView) {
        this.contextView = contextView;
    }

    public void status(byte status) {
        contextView.setStatus(status);
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
