package net.zscript.javasimulator.zcode.i2c;

import net.zscript.javareceiver.core.ZscriptCommandOutStream;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javasimulator.Entity;
import net.zscript.javasimulator.connections.i2c.I2cProtocolCategory;

public class ZcodeI2cCapabilitiesCommand {

    public static void execute(CommandContext ctx, Entity entity) {
        ZscriptCommandOutStream out = ctx.getOutStream();
        out.writeField('C', 0x0f);
        out.writeField('N', 0);
        out.writeField('A', 0);
        out.writeField('P', entity.countConnection(I2cProtocolCategory.class));
        out.writeField('F', 4);
    }
}
