package org.zcode.javaclient.components;

import org.zcode.javaclient.parsing.Address;
import org.zcode.javaclient.parsing.Comment;
import org.zcode.javaclient.parsing.LockSet;

public class CommandSequence extends ZcodeSequence<Command> {
    protected CommandSequence(Address address, Comment comment, LockSet locks, Command... commands) {
        super(address, comment, commands);
    }

}
