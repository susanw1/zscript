package com.wittsfamily.rcode.javareceiver.instreams;

import com.wittsfamily.rcode.javareceiver.parsing.RCodeLookaheadStream;

public interface RCodeChannelInStream {
    int read();

    RCodeSequenceInStream getSequenceInStream();

    RCodeLookaheadStream getLookahead();
}
