package net.zscript.javasimulator.zcode.i2c;

import java.util.OptionalInt;

import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javasimulator.connections.i2c.I2cProtocolCategory;
import net.zscript.model.components.ZscriptStatus;

public class I2cSetupCommand {

    public static void execute(CommandContext ctx, I2cModule module) {
        final OptionalInt portOpt = ctx.getField((byte) 'P');
        if (portOpt.isPresent() && portOpt.getAsInt() >= module.getEntity().countConnection(I2cProtocolCategory.class)) {
            ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
            return;
        }
        if (portOpt.isPresent()) {
            int port = portOpt.getAsInt();
            module.getNotificationHandler().setNotifications(port, ctx.hasField((byte) 'N'));
            module.getNotificationHandler().setAddressing(port, ctx.hasField((byte) 'A'));
        }
        final OptionalInt freqOpt = ctx.getField((byte) 'F');
        if (freqOpt.isPresent()) {
            if (freqOpt.getAsInt() > 3) {
                ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
                return;
            }
            int  freq = freqOpt.getAsInt();
            long baud;
            if (freq == 0) {
                baud = 10_000;
            } else if (freq == 1) {
                baud = 100_000;
            } else if (freq == 2) {
                baud = 400_000;
            } else if (freq == 3) {
                baud = 1000_000;
            } else {
                ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
                return;
            }
            if (portOpt.isPresent()) {
                module.setBaud(portOpt.getAsInt(), baud);
            } else {
                module.setBauds(baud);
            }
        }
    }
}
