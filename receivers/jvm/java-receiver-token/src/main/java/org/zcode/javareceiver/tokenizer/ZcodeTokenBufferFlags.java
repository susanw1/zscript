package org.zcode.javareceiver.tokenizer;

public class ZcodeTokenBufferFlags {
    private boolean markerWritten;
    private boolean seqMarkerWritten;
    private boolean canRead;

    public void setMarkerWritten() {
        this.markerWritten = true;
    }

    public void setSeqMarkerWritten() {
        this.seqMarkerWritten = true;
    }

    public void setCanRead() {
        this.canRead = true;
    }

    public void clearMarkerWritten() {
        this.markerWritten = false;
    }

    public void clearSeqMarkerWritten() {
        this.seqMarkerWritten = false;
    }

    public void clearCanRead() {
        this.canRead = false;
    }

    public boolean isMarkerWritten() {
        return markerWritten;
    }

    public boolean isSeqMarkerWritten() {
        return seqMarkerWritten;
    }

    public boolean isCanRead() {
        return canRead;
    }

}
