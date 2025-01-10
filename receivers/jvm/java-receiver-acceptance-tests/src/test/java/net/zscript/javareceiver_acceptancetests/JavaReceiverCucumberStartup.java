package net.zscript.javareceiver_acceptancetests;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import io.cucumber.java.en.Given;

import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.modules.outerCore.ZscriptOuterCoreModule;
import net.zscript.tokenizer.TokenRingBuffer;

public class JavaReceiverCucumberStartup {
    private static boolean         hasStarted = false;
    private static ExecutorService execs;

    @Given("the target is running and connected")
    public void the_target_is_running_and_connected() {
        if (!hasStarted) {
            hasStarted = true;

            final Zscript z = new Zscript();
            z.addModule(new ZscriptCoreModule());
            z.addModule(new ZscriptOuterCoreModule());

            TokenRingBuffer buffer         = TokenRingBuffer.createBufferWithCapacity(128);
            InputStream     commandStream  = new PipedInputStream();
            OutputStream    responseStream = new PipedOutputStream();

            //            z.addChannel(new LocalChannel(buffer, commandStream, responseStream));

            execs = Executors.newSingleThreadExecutor();
            execs.submit(() -> {
                try {
                    while (true) {
                        z.progress();
                    }
                } catch (final Throwable t) {
                    t.printStackTrace();
                }
            });
        }
    }

}
