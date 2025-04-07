package net.zscript.javareceiver.demorun;

import javax.annotation.Nonnull;

import net.zscript.javareceiver.core.SequenceOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.core.ZscriptChannel;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.modules.outercore.ZscriptOuterCoreModule;
import net.zscript.javareceiver.modules.scriptspaces.ScriptSpaceModule;
import net.zscript.javareceiver.scriptspaces.ScriptSpace;
import net.zscript.javareceiver.testing.OutputStreamOutStream;
import net.zscript.tokenizer.TokenRingBuffer;
import net.zscript.tokenizer.Tokenizer;

public class Main {
    public static void main(String[] args) {
        Zscript                zscript  = new Zscript();
        ScriptSpaceModule      scrSpace = new ScriptSpaceModule();
        ZscriptOuterCoreModule oCore    = new ZscriptOuterCoreModule();
        zscript.addNotificationSource(oCore.getNotificationSource());
        zscript.addModule(scrSpace);
        zscript.addModule(oCore);
        zscript.addModule(new ZscriptCoreModule());
        byte[]      code  = "Z2&Z20&Z21P0\n".getBytes();
        ScriptSpace space = ScriptSpace.from(zscript, "Z0&Z2\nZ1S1\n%Z28M20\n");
        scrSpace.addScriptSpace(space);
        zscript.addActionSource(space);

        TokenRingBuffer   rbuff = TokenRingBuffer.createBufferWithCapacity(100);
        SequenceOutStream out   = new OutputStreamOutStream<>(System.out);
        zscript.addChannel(new ZscriptChannel(rbuff, out) {
            private final Tokenizer in = new Tokenizer(rbuff.getTokenWriter(), false);
            private       int       i  = 0;

            @Override
            public void moveAlong() {
                if (i < code.length) {
                    in.accept(code[i++]);
                }
            }

            @Override
            public void channelInfo(@Nonnull CommandContext ctx) {
                ctx.getOutStream().writeFieldQuoted('D', "Basic text channel");
            }

            @Override
            public void channelSetup(@Nonnull CommandContext ctx) {
            }
        });
        zscript.setNotificationOutStream(out);
        for (int i = 0; i < 5000000; i++) {
            zscript.progress();
        }
    }
}
