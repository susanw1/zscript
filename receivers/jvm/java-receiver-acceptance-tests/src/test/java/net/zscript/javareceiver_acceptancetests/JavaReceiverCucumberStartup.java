package net.zscript.javareceiver_acceptancetests;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import io.cucumber.java.en.Given;
import net.zscript.javareceiver.Zcode;
import net.zscript.javareceiver.ZcodeBusInterruptSource;
import net.zscript.javareceiver.ZcodeParameters;
import net.zscript.javareceiver.commands.ZcodeActivateCommand;
import net.zscript.javareceiver.commands.ZcodeCapabilitiesCommand;
import net.zscript.javareceiver.commands.ZcodeEchoCommand;
import net.zscript.javareceiver.parsing.ZcodeCommandChannel;
import net.zscript.zscript_acceptance_tests.AcceptanceTestConnectionManager;

public class JavaReceiverCucumberStartup {
    private static boolean         hasStarted = false;
    private static ExecutorService execs;

    @Given("the target is running and connected")
    public void the_target_is_running_and_connected() {
        if (!hasStarted) {
            hasStarted = true;
            final ZcodeParameters     params = new ZcodeParameters(false);
            final Zcode               z      = new Zcode(params, new ZcodeBusInterruptSource[0]);
            final LocalTestConnection con    = new LocalTestConnection(z, params, false);

            z.setChannels(new ZcodeCommandChannel[] { con });

            z.getCommandFinder().registerCommand(new ZcodeIdentifyCommand())
                    .registerCommand(new ZcodeEchoCommand())
                    .registerCommand(new ZcodeCapabilitiesCommand(params, z))
                    .registerCommand(new ZcodeActivateCommand());

            AcceptanceTestConnectionManager.registerConnection(con);
            execs = Executors.newSingleThreadExecutor();
            execs.submit(() -> {
                try {
                    while (true) {
                        z.progressZcode();
                    }
                } catch (final Throwable t) {
                    t.printStackTrace();
                }
            });
        }
    }

}
