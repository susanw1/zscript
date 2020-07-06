package com.wittsfamily.rcode.parsing;

import com.wittsfamily.rcode.RCodeParameters;
import com.wittsfamily.rcode.commands.RCodeCommand;

public class RCodeCommandFinder {
    private RCodeCommand[] commands;
    private int commandNum;

    public RCodeCommandFinder(RCodeParameters params) {
        commands = new RCodeCommand[params.commandNum];
    }

    public void registerCommand(RCodeCommand cmd) {
        commands[commandNum++] = cmd;
    }

    public RCodeCommand findCommand(RCodeCommandSlot slot) {
        byte rVal = slot.getFields().get('R', (byte) 0xFF);
        if (rVal == 0xFF && !slot.getFields().has('R')) {
            System.out.println("oops, no R value");
            return null;
        }
        for (int i = 0; i < commandNum; i++) {
            if (commands[i].getCode() == rVal) {
                return commands[i];
            }
        }
        return null;
    }
}
