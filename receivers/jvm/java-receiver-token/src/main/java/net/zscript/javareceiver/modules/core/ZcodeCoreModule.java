package net.zscript.javareceiver.modules.core;

import net.zscript.javareceiver.core.ZcodeStatus;
import net.zscript.javareceiver.execution.ZcodeAddressingContext;
import net.zscript.javareceiver.execution.ZcodeCommandContext;
import net.zscript.javareceiver.modules.ZcodeModule;
import net.zscript.javareceiver.tokenizer.BlockIterator;

public class ZcodeCoreModule implements ZcodeModule {
    private final ZcodeCapabilitiesCommand capabilitiesCmd = new ZcodeCapabilitiesCommand();
    private final ZcodeGuidCommand         guidCmd         = new ZcodeGuidCommand();
    private final ZcodeRandomCodeCommand   codeCmd         = new ZcodeRandomCodeCommand();

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
            capabilitiesCmd.execute(ctx);
            break;
        case 0x1:
            ZcodeEchoCommand.execute(ctx);
            break;
        case 0x2:
            ZcodeActivateCommand.execute(ctx);
            break;
        case 0x4:
            guidCmd.fetch(ctx);
            break;
        case 0x8:
            ZcodeChannelInfoCommand.execute(ctx);
            break;
        case 0xc:
            codeCmd.make(ctx);
            break;
        case 0xd:
            codeCmd.match(ctx);
            break;
        default:
            ctx.status(ZcodeStatus.COMMAND_NOT_FOUND);
        }
    }

    @Override
    public void address(ZcodeAddressingContext ctx) {
        for (BlockIterator iterator = ctx.getAddressedData(); iterator.hasNext();) {
            byte b = iterator.next();
            System.out.print((char) b);
        }
    }

    public ZcodeCapabilitiesCommand getCapabilitiesCmd() {
        return capabilitiesCmd;
    }

    public ZcodeRandomCodeCommand getCodeCmd() {
        return codeCmd;
    }

    public ZcodeGuidCommand getGuidCmd() {
        return guidCmd;
    }

}
