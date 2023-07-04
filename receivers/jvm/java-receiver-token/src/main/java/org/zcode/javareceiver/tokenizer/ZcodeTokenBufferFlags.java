package org.zcode.javareceiver.tokenizer;

public class ZcodeTokenBufferFlags {
    private boolean markerWritten    = false;
    private boolean seqMarkerWritten = false;
    private boolean readerBlocked    = true;

    public void setMarkerWritten() {
        this.markerWritten = true;
    }

    public void setSeqMarkerWritten() {
        this.seqMarkerWritten = true;
    }

    public boolean isReaderBlocked() {
        return readerBlocked;
    }

    public void clearReaderBlocked() {
        this.readerBlocked = false;
    }

    public void clearMarkerWritten() {
        this.markerWritten = false;
    }

    public void clearSeqMarkerWritten() {
        this.seqMarkerWritten = false;
    }

    public void setReaderBlocked() {
        this.readerBlocked = true;
    }

    public boolean isMarkerWritten() {
        return markerWritten;
    }

    public boolean isSeqMarkerWritten() {
        return seqMarkerWritten;
    }

}
