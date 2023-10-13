package net.zscript.javareceiver.tokenizer;

/**
 *
 */
public class TokenBufferFlags {
    private boolean markerWritten    = false;
    private boolean seqMarkerWritten = false;
    private boolean readerBlocked    = true;

    public void setMarkerWritten() {
        this.markerWritten = true;
    }

    public boolean getAndClearMarkerWritten() {
        if (markerWritten) {
            markerWritten = false;
            return true;
        }
        return false;
    }

    public void setSeqMarkerWritten() {
        this.seqMarkerWritten = true;
    }

    public boolean getAndClearSeqMarkerWritten() {
        if (seqMarkerWritten) {
            seqMarkerWritten = false;
            return true;
        }
        return false;
    }

    public boolean isReaderBlocked() {
        return readerBlocked;
    }

    public void clearReaderBlocked() {
        this.readerBlocked = false;
    }

    public void setReaderBlocked() {
        this.readerBlocked = true;
    }

}
