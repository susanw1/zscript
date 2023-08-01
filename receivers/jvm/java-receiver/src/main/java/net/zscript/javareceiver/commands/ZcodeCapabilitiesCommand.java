package net.zscript.javareceiver.commands;

import net.zscript.javareceiver.Zcode;
import net.zscript.javareceiver.ZcodeLockSet;
import net.zscript.javareceiver.ZcodeOutStream;
import net.zscript.javareceiver.ZcodeParameters;
import net.zscript.javareceiver.ZcodeResponseStatus;
import net.zscript.javareceiver.parsing.ZcodeCommandSequence;
import net.zscript.javareceiver.parsing.ZcodeCommandSlot;

public class ZcodeCapabilitiesCommand implements ZcodeCommand {
    private final ZcodeParameters params;
    private final Zcode           zcode;

    public ZcodeCapabilitiesCommand(final ZcodeParameters params, final Zcode zcode) {
        this.params = params;
        this.zcode = zcode;
    }

    @Override
    public void execute(final ZcodeCommandSlot slot, final ZcodeCommandSequence sequence, final ZcodeOutStream out) {
        final byte[] cmds = zcode.getCommandFinder().getSupportedCommands();
        out.writeField('M', (byte) 1);
        if (params.hugeFieldLength > 255) {
            int bigl   = params.hugeFieldLength;
            int offset = 0;
            while (bigl > 255) {
                bigl >>>= 8;
                offset++;
            }
            out.writeField('B', (byte) (params.hugeFieldLength >>> (8 * offset)));
            for (int i = offset - 1; i >= 0; i--) {
                out.continueField((byte) (params.hugeFieldLength >>> (8 * i)));
            }
        } else {
            out.writeField('B', (byte) params.hugeFieldLength);
        }
        if (params.fieldNum > 255) {
            int bigl   = params.fieldNum;
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
            out.writeStatus(ZcodeResponseStatus.OK);
        } else {
            out.writeStatus(ZcodeResponseStatus.BAD_PARAM);
        }
        slot.setComplete(true);
    }

    @Override
    public void setLocks(final ZcodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 2;
    }

    @Override
    public boolean matchesCode(final byte[] code) {
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
