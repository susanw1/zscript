package com.wittsfamily.rcode.javareceiver.commands;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeLockSet;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.RCodeResponseStatus;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSlot;

public class RCodeCapabilitiesCommand implements RCodeCommand {
    private final RCodeParameters params;
    private final RCode rcode;

    public RCodeCapabilitiesCommand(RCodeParameters params, RCode rcode) {
        this.params = params;
        this.rcode = rcode;
    }

    @Override
    public void execute(RCodeCommandSlot slot, RCodeCommandSequence sequence, RCodeOutStream out) {
        byte[] cmds = rcode.getCommandFinder().getSupportedCommands();
        out.writeField('M', (byte) 1);
        if (params.bigBigFieldLength > 255) {
            int bigl = params.bigBigFieldLength;
            int offset = 0;
            while (bigl > 255) {
                bigl >>>= 8;
                offset++;
            }
            out.writeField('B', (byte) (params.bigBigFieldLength >>> (8 * offset)));
            for (int i = offset - 1; i >= 0; i--) {
                out.continueField((byte) (params.bigBigFieldLength >>> (8 * i)));
            }
        } else {
            out.writeField('B', (byte) params.bigBigFieldLength);
        }
        if (params.fieldNum > 255) {
            int bigl = params.fieldNum;
            int offset = 0;
            while (bigl > 255) {
                bigl >>>= 8;
                offset++;
            }
            out.writeField('N', (byte) (params.fieldNum >>> (8 * offset)));
            for (int i = offset - 1; i >= 0; i--) {
                out.continueField((byte) (params.fieldNum >>> (8 * i)));
            }
        } else {
            out.writeField('N', (byte) params.fieldNum);
        }
        out.writeField('C', (byte) 0);
        out.writeBigHexField(cmds, cmds.length);
        if (slot.getFields().get('P', (byte) 0) == 0) {
            out.writeStatus(RCodeResponseStatus.OK);
        } else {
            out.writeStatus(RCodeResponseStatus.BAD_PARAM);
        }
        slot.setComplete(true);
    }

    @Override
    public void finish(RCodeCommandSlot slot, RCodeOutStream out) {
    }

    @Override
    public void setLocks(RCodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 2;
    }

    @Override
    public boolean matchesCode(byte[] code) {
        return code.length == 1 && code[0] == getCode();
    }

    @Override
    public int getCodeLength() {
        return 1;
    }

    @Override
    public byte[] getFullCode() {
        return new byte[] { getCode() };
    }

}
