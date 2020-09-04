package com.wittsfamily.rcode.javareceiver.commands;

import com.wittsfamily.rcode.javareceiver.RCodeLockSet;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSlot;

public interface RCodeCommand {
    boolean continueLocking(RCodeCommandChannel c);

    void finish(RCodeCommandSlot rCodeCommandSlot, RCodeOutStream out);

    void execute(RCodeCommandSlot slot, RCodeCommandSequence sequence, RCodeOutStream out);

    void setLocks(RCodeLockSet locks);

    byte getCode();

    boolean matchesCode(byte[] code);

    int getCodeLength();
}
