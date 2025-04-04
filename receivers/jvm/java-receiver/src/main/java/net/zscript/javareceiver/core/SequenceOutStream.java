package net.zscript.javareceiver.core;

/**
 * Defines the sequence-level life-cycle operations that allow responses to be sent back up to a channel.
 * <p/>
 * Any implementation of this interface must either also implement {@link CommandOutStream} directly, or must override {@link #asCommandOutStream()} to provide a CommandOutStream
 * implementation.
 */
public interface SequenceOutStream {

    /**
     * Opens the stream. Until it is opened, other operations are undefined. Calling open() may perform tasks such as allocating buffers, establishing upstream connections etc.
     * Calling this when already open is safe.
     */
    void open();

    /**
     * Closes the stream. This may perform operations such as transmitting the final stream content, or shutting down the comms protocol with upstream. Calling this when already
     * closed is safe.
     */
    void close();

    /**
     * Indicates whether the stream is open.
     *
     * @return true if open, false otherwise
     */
    boolean isOpen();

    void writeOrElse();

    void writeAndThen();

    void writeOpenParen();

    void writeCloseParen();

    void endSequence();

    void writeBytes(byte[] c);

    /**
     * Accesses the command interface to allow command-response activity.
     *
     * @return a command-oriented output stream
     */
    default CommandOutStream asCommandOutStream() {
        return (CommandOutStream) this;
    }
}
