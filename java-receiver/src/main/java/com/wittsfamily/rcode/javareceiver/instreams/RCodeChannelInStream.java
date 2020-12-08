package com.wittsfamily.rcode.javareceiver.instreams;

public interface RCodeChannelInStream {
    int read();

    RCodeSequenceInStream getSequenceInStream();

    RCodeLookaheadStream getLookahead();
}
