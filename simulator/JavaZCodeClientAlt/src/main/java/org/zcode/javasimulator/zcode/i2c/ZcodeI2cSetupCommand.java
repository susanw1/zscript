package org.zcode.javasimulator.zcode.i2c;

import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeCommandOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javasimulator.connections.i2c.I2cProtocolCategory;

public class ZcodeI2cSetupCommand {

    public static void execute(ZcodeCommandContext ctx, I2cModule module) {
        ZcodeCommandOutStream out = ctx.getOutStream();
        Optional<Integer> portOpt = ctx.getField((byte) 'P');
        if (portOpt.isPresent() && portOpt.get() >= module.getEntity().countConnection(I2cProtocolCategory.class)) {
            ctx.status(ZcodeStatus.VALUE_OUT_OF_RANGE);
            return;
        }
        Optional<Integer> freqOpt = ctx.getField((byte) 'F');
        if (freqOpt.isPresent()) {
            if (freqOpt.get() > 3) {
                ctx.status(ZcodeStatus.VALUE_OUT_OF_RANGE);
                return;
            }
            int freq = freqOpt.get();
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
                ctx.status(ZcodeStatus.VALUE_OUT_OF_RANGE);
                return;
            }
            if (portOpt.isPresent()) {
                module.setBaud(portOpt.get(), baud);
            } else {
                module.setBauds(baud);
            }
        }

    }
}
