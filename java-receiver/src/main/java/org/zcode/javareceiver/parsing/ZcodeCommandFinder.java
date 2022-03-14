package org.zcode.javareceiver.parsing;

import org.zcode.javareceiver.Zcode;
import org.zcode.javareceiver.ZcodeParameters;
import org.zcode.javareceiver.commands.ZcodeCommand;

public class ZcodeCommandFinder {
    private final ZcodeParameters params;
    private final Zcode           zcode;
    private final ZcodeCommand[]  commands;
    private int                   commandNum;

    public ZcodeCommandFinder(final ZcodeParameters params, final Zcode zcode) {
        this.params = params;
        this.zcode = zcode;
        commands = new ZcodeCommand[params.commandNum];
    }

    public ZcodeCommandFinder registerCommand(final ZcodeCommand cmd) {
        if (commandNum >= commands.length) {
            zcode.configFail("Too many commands registered.");
        } else {
            commands[commandNum++] = cmd;
        }
        return this;
    }

    public ZcodeCommand findCommand(final ZcodeCommandSlot slot) {
        final int fieldSectionNum = slot.getFields().countFieldSections('R');
        if (fieldSectionNum == 0) {
            return null;
        } else if (fieldSectionNum == 1) {
            final byte rVal = slot.getFields().get('R', (byte) 0xFF);
            for (int i = 0; i < commandNum; i++) {
                if (commands[i].getCode() == rVal && commands[i].getCodeLength() == 1) {
                    return commands[i];
                }
            }
            return null;
        } else if (params.hasMultiByteCommands) {
            final ZcodeFieldMap map  = slot.getFields();
            final byte[]        code = new byte[fieldSectionNum];
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
        int highestCodeLength = 0;
        int totalLength       = 0;
        for (int i = 0; i < commandNum; i++) {
            if (commands[i].getCodeLength() > highestCodeLength) {
                highestCodeLength = commands[i].getCodeLength();
            }
            totalLength += commands[i].getCodeLength();
        }
        for (int l = 1; l <= highestCodeLength; l++) {
            for (int i = 0; i < commandNum; i++) {
                if (commands[i].getCodeLength() == l) {
                    totalLength += l;
                    break;
                }
            }
        }
        final byte[] cmds = new byte[totalLength];
        int          pos  = 0;
        for (int l = 1; l <= highestCodeLength; l++) {
            boolean hasInitialisedLength = l == 1;
            for (int i = 0; i < commandNum; i++) {
                if (commands[i].getCodeLength() == l) {
                    if (!hasInitialisedLength) {
                        for (int j = 0; j < l; j++) {
                            cmds[pos++] = 0;
                        }
                        hasInitialisedLength = true;
                    }
                    final byte[] fullCode = commands[i].getFullCode();
                    for (int j = 0; j < l; j++) {
                        cmds[pos++] = fullCode[j];
                    }
                }
            }
        }
        return cmds;
    }
}
