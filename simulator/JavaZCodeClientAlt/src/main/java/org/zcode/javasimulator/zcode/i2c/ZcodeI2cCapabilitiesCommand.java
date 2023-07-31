package org.zcode.javasimulator.zcode.i2c;

import org.zcode.javareceiver.core.ZcodeCommandOutStream;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javasimulator.Entity;
import org.zcode.javasimulator.connections.i2c.I2cProtocolCategory;

public class ZcodeI2cCapabilitiesCommand {

    public static void execute(ZcodeCommandContext ctx, Entity entity) {
        ZcodeCommandOutStream out = ctx.getOutStream();
        out.writeField('C', 0x0f);
        out.writeField('N', 0);
        out.writeField('A', 0);
        out.writeField('P', entity.countConnection(I2cProtocolCategory.class));
        out.writeField('F', 4);
    }
}
