package net.zscript.javareceiver.demoRun;

import java.io.IOException;

import net.zscript.javareceiver.core.Zcode;
import net.zscript.javareceiver.core.ZcodeChannel;
import net.zscript.javareceiver.core.ZcodeOutStream;
import net.zscript.javareceiver.execution.ZcodeCommandContext;
import net.zscript.javareceiver.modules.core.ZcodeCoreModule;
import net.zscript.javareceiver.modules.outerCore.ZcodeOuterCoreModule;
import net.zscript.javareceiver.tokenizer.ZcodeTokenRingBuffer;
import net.zscript.javareceiver.tokenizer.ZcodeTokenizer;

public class Main {
    public static void main(String[] args) throws IOException {
        Zcode                zcode = new Zcode();
        ZcodeOuterCoreModule oCore = new ZcodeOuterCoreModule();
        zcode.addNotificationSource(oCore.getNotificationSource());
        zcode.addModule(oCore);
        zcode.addModule(new ZcodeCoreModule());
        byte[] code = "Z2(Z1S1)Z1&Z1|Z1\n".getBytes();

        ZcodeTokenRingBuffer rbuff = ZcodeTokenRingBuffer.createBufferWithCapacity(100);
        ZcodeOutStream       out   = new ZcodePrintingOutStream();
        zcode.addChannel(new ZcodeChannel(rbuff, out) {
            ZcodeTokenizer in = new ZcodeTokenizer(rbuff.getTokenWriter(), 2);
            private int    i  = 0;

            @Override
            public void moveAlong() {
                if (i < code.length) {
                    in.accept(code[i++]);
                }
            }

            @Override
            public void channelInfo(ZcodeCommandContext ctx) {
                ctx.getOutStream().writeQuotedString("Basic text channel");
            }

            @Override
            public void channelSetup(ZcodeCommandContext ctx) {
            }
        });
        zcode.setNotificationOutStream(out);
        for (int i = 0; i < 10000; i++) {
            zcode.progress();
        }
    }
}
