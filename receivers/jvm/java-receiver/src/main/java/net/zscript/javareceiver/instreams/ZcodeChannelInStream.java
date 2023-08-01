package net.zscript.javareceiver.instreams;

public interface ZcodeChannelInStream {
    int read();

    ZcodeSequenceInStream getSequenceInStream();

    ZcodeLookaheadStream getLookahead();
}
