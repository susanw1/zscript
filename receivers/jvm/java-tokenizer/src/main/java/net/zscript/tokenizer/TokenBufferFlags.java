package net.zscript.tokenizer;

/**
 * Flags to indicate current status, acting as a communication channel between the TokenWriter and TokenReader.
 */
public class TokenBufferFlags {
    private boolean markerWritten    = false;
    private boolean seqMarkerWritten = false;
    private boolean readerBlocked    = true;

    /**
     * Signals that some Marker (possibly a sequence marker) has just been written to the TokenBuffer.
     * <p/>
     * This allows the Writer to tell the Reader that it can perform some processing.
     */
    public void setMarkerWritten() {
        this.markerWritten = true;
    }

    /**
     * Indicates if any Marker (sequence or otherwise) has been written (flag is auto-cleared).
     * <p/>
     * This allows the Reader to tell that it can proceed with token processing.
     *
     * @return true if set, false otherwise
     */
    public boolean getAndClearMarkerWritten() {
        if (markerWritten) {
            markerWritten = false;
            return true;
        }
        return false;
    }

    /**
     * Signals that some sequence marker has just been written to the TokenBuffer.
     * <p/>
     * This allows the Writer to tell the Reader that there's a completed sequence ready to process (or an error or overrun). It's possible that this flag is already set, but the
     * Reader should process as much as it can anyway, so lost notifications don't matter.
     */
    public void setSeqMarkerWritten() {
        this.seqMarkerWritten = true;
    }

    /**
     * Indicates whether a *sequence* Marker has been written (flag is auto-cleared).
     * <p/>
     * This allows the Reader to know that there's a completed sequence awaiting processing (if it didn't know already). If the marker is indicating an error, then this might mean
     * the current sequence should be aborted.
     *
     * @return true if set, false otherwise
     */
    public boolean getAndClearSeqMarkerWritten() {
        if (seqMarkerWritten) {
            seqMarkerWritten = false;
            return true;
        }
        return false;
    }

    /**
     * Indicates whether the Reader is blocked (ie waiting for a Marker to say it can proceed with processing).
     * <p/>
     * This allows the Tokenizer to decide whether there's any point rejecting an offered byte if there's no capacity, given that the Reader will never clear any space. This might
     * happen if a single command exceeds the size of the buffer. Instead, it will add an OVERRUN marker which will cause the Reader to proceed into an error state.
     *
     * @return true if the reader is blocked, false otherwise
     */
    public boolean isReaderBlocked() {
        return readerBlocked;
    }

    /**
     * Indicates that the Reader is not (or no longer) blocked, and can proceed with processing tokens if given the chance. This is effectively an acknowledgement that a Marker has
     * been identified.
     * <p/>
     * If the Tokenizer has been struggling with buffer space, this allows it to start recovering.
     */
    public void clearReaderBlocked() {
        this.readerBlocked = false;
    }

    /**
     * Indicates that the Reader is now blocked - it has processed everything it can and is awaiting more tokens.
     * <p/>
     * This signals to the Tokenizer that the Reader needs feeding, and if the buffer is running out of space then it's not the Reader's fault.
     */
    public void setReaderBlocked() {
        this.readerBlocked = true;
    }

}
