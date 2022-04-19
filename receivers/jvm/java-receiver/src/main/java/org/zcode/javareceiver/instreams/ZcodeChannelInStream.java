package org.zcode.javareceiver.instreams;

public interface ZcodeChannelInStream {
    int read();

    ZcodeSequenceInStream getSequenceInStream();

    ZcodeLookaheadStream getLookahead();
}
