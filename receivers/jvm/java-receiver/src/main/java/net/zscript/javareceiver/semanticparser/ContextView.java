package net.zscript.javareceiver.semanticparser;

import javax.annotation.Nonnull;

import net.zscript.tokenizer.TokenBuffer.TokenReader;

/**
 * Defines parser state methods accessed by Command/Address contexts.
 */
public interface ContextView {
    @Nonnull
    TokenReader getReader();

    /**
     * Allows a command to signal that it is complete (set as default on entry to a command's code), or not complete. If not complete, then the parser will note that the command is
     * incomplete, and will expect some other code to call {@link AsyncActionNotifier#notifyNeedsAction()}.
     *
     * @param isComplete if true then command is marked as complete; if false, then the command will not be considered complete on return from its execute method.
     */
    void setCommandComplete(boolean isComplete);

    /**
     * Indicates whether the command is in its {@code complete} state, or whether there is async work yet to do. Note that command code when executed defaults to complete.
     *
     * @return true if complete, false otherwise
     */
    boolean isCommandComplete();

    void activate();

    boolean isActivated();

    /**
     * Updates the status of the current command if it hasn't already been updated.
     *
     * @param status the status to set
     * @return true if changed, false otherwise
     */
    boolean requestStatusChange(byte status);

    /**
     * Allows a background/async command component to signal that the command's code needs scheduling to move it along.
     * <p/>
     * Example, an interrupt-driven serial writer might call {@link AsyncActionNotifier#notifyNeedsAction()} in its interrupt handler to indicate that the data was all sent
     * correctly. The command will be progressed next time Zscript is given execution time.
     */
    interface AsyncActionNotifier {
        void notifyNeedsAction();
    }

    /**
     * Allows the initial "foreground" part of an async command to get hold of the AsyncActionNotifier, which it can store and call back in the "background" part - say in an
     * interrupt handler or other event listener - in order to signal that the command should move along.
     *
     * @return the AsyncActionNotifier to allow a later callback
     */
    @Nonnull
    AsyncActionNotifier getAsyncActionNotifier();

    int getChannelIndex();
}
