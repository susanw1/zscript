package com.wittsfamily.rcode.javareceiver.parsing;

import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.commands.RCodeCommand;

public class RCodeCommandFinder {
    private final RCodeParameters params;
    private RCodeCommand[] commands;
    private int commandNum;

    public RCodeCommandFinder(RCodeParameters params) {
        this.params = params;
        commands = new RCodeCommand[params.commandNum];
    }

    public void registerCommand(RCodeCommand cmd) {
        commands[commandNum++] = cmd;
    }

    public RCodeCommand findCommand(RCodeCommandSlot slot) {
        int fieldSectionNum = slot.getFields().countFieldSections('R');
        if (fieldSectionNum == 0) {
            System.out.println("oops, no R value");
            return null;
        } else if (fieldSectionNum == 1) {
            byte rVal = slot.getFields().get('R', (byte) 0xFF);
            for (int i = 0; i < commandNum; i++) {
                if (commands[i].getCode() == rVal && commands[i].getCodeLength() == 1) {
                    return commands[i];
                }
            }
            return null;
        } else if (params.hasMultiByteCommands) {
            RCodeFieldMap map = slot.getFields();
            byte[] code = new byte[fieldSectionNum];
            for (int i = 0; i < fieldSectionNum; i++) {
                code[i] = map.get('R', i, (byte) 0xFF);
            }
            for (int i = 0; i < commandNum; i++) {
                if (commands[i].getCodeLength() == fieldSectionNum && commands[i].matchesCode(code)) {
                    return commands[i];
                }
            }
            return null;
        } else {
            return null;
        }
    }

    public byte[] getSupportedCommands() {
        byte[] cmds = new byte[commandNum];
        for (int i = 0; i < commandNum; i++) {
            cmds[i] = commands[i].getCode();
        }
        return cmds;
    }
}
