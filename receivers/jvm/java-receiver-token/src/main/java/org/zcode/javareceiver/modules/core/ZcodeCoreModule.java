package org.zcode.javareceiver.modules.core;

import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeAddressingContext;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
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
    public void execute(ZcodeCommandContext ctx, int command) {
        switch (command) {
        case 0x0:
            ZcodeCapabilitiesCommand.execute(ctx);
            break;
        case 0x1:
            ZcodeEchoCommand.execute(ctx);
            break;
        case 0x2:
            ZcodeActivateCommand.execute(ctx);
            break;
        case 0x4:
            ZcodeGuidCommand.fetch(ctx);
            break;
        case 0x8:
            ZcodeChannelInfoCommand.execute(ctx);
            break;
        case 0xc:
            ZcodeRandomCodeCommand.make(ctx);
            break;
        case 0xd:
            ZcodeRandomCodeCommand.match(ctx);
            break;
        default:
            ctx.status(ZcodeStatus.COMMAND_NOT_FOUND);
        }
    }

    @Override
    public void moveAlong(ZcodeCommandContext ctx, int command) {
    }

    @Override
    public void address(ZcodeAddressingContext ctx) {
        for (BlockIterator iterator = ctx.getAddressedData(); iterator.hasNext();) {
            byte b = iterator.next();
            System.out.print((char) b);
        }

    }

    @Override
    public void addressMoveAlong(ZcodeAddressingContext ctx) {
    }
}
