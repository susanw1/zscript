package net.zscript.javareceiver.modules.core;

import javax.annotation.Nonnull;

import net.zscript.javareceiver.execution.AddressingContext;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.modules.ZscriptModule;
import net.zscript.model.components.ZscriptStatus;
import net.zscript.util.BlockIterator;

public class ZscriptCoreModule implements ZscriptModule {
    private final ZscriptCapabilitiesCommand capabilitiesCmd = new ZscriptCapabilitiesCommand();
    private final ZscriptGuidCommand         guidCmd         = new ZscriptGuidCommand();
    private final ZscriptRandomCodeCommand   codeCmd         = new ZscriptRandomCodeCommand();

    public static int getCommands() {
        return 0x3117;
    }

    @Override
    public int getModuleId() {
        return 0;
    }

    @Override
    public void execute(CommandContext ctx, int command) {
        switch (command) {
        case 0x0:
            capabilitiesCmd.execute(ctx);
            break;
        case 0x1:
            ZscriptEchoCommand.execute(ctx);
            break;
        case 0x2:
            ZscriptActivateCommand.execute(ctx);
            break;
        case 0x4:
            guidCmd.fetch(ctx);
            break;
        case 0x8:
            ZscriptChannelInfoCommand.execute(ctx);
            break;
        case 0xc:
            codeCmd.make(ctx);
            break;
        case 0xd:
            codeCmd.match(ctx);
            break;
        default:
            ctx.status(ZscriptStatus.COMMAND_NOT_FOUND);
        }
    }

    @Override
    public void address(AddressingContext ctx) {
        for (BlockIterator iterator = ctx.getAddressedData(); iterator.hasNext(); ) {
            byte b = iterator.next();
            System.out.print((char) b);
        }
    }

    @Nonnull
    public ZscriptCapabilitiesCommand getCapabilitiesCmd() {
        return capabilitiesCmd;
    }

    @Nonnull
    public ZscriptRandomCodeCommand getCodeCmd() {
        return codeCmd;
    }

    @Nonnull
    public ZscriptGuidCommand getGuidCmd() {
        return guidCmd;
    }

}
