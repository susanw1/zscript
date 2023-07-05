package org.zcode.javareceiver.modules.core;

import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeAddressingView;
import org.zcode.javareceiver.execution.ZcodeCommandView;
import org.zcode.javareceiver.modules.ZcodeModule;
import org.zcode.javareceiver.tokenizer.BlockIterator;

public class ZcodeCoreModule implements ZcodeModule {

    public static int getCommands() {
        return 0x3007;
    }

    @Override
    public int getModuleID() {
        return 0;
    }

    @Override
    public void execute(ZcodeCommandView view, int command) {
        switch (command) {
        case 0x0:
            ZcodeCapabilitiesCommand.execute(view);
            break;
        case 0x1:
            ZcodeEchoCommand.execute(view);
            break;
        case 0x2:
            ZcodeActivateCommand.execute(view);
            break;
        case 0x4:
            ZcodeGuidCommand.fetch(view);
            break;
        case 0x8:
            ZcodeChannelInfoCommand.execute(view);
            break;
        case 0xc:
            ZcodeRandomCodeCommand.make(view);
            break;
        case 0xd:
            ZcodeRandomCodeCommand.match(view);
            break;
        default:
            view.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
        }
    }

    @Override
    public void moveAlong(ZcodeCommandView view, int command) {
    }

    @Override
    public void address(ZcodeAddressingView view) {
        for (BlockIterator iterator = view.getAddressedData(); iterator.hasNext();) {
            byte b = iterator.next();
            System.out.print((char) b);
        }

    }

    @Override
    public void addressMoveAlong(ZcodeAddressingView view) {
    }
}
