package com.wittsfamily.rcode.commands;

import com.wittsfamily.rcode.RCodeLockSet;
import com.wittsfamily.rcode.RCodeOutStream;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.parsing.RCodeCommandSlot;

public interface RCodeCommand {
    boolean continueLocking(RCodeCommandChannel c);

    void finish(RCodeCommandSlot rCodeCommandSlot, RCodeOutStream out);

    void execute(RCodeCommandSlot slot, RCodeCommandSequence sequence, RCodeOutStream out);

    void setLocks(RCodeLockSet locks);

    byte getCode();
}
