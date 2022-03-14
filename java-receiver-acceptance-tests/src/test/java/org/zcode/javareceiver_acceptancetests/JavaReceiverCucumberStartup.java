package org.zcode.javareceiver_acceptancetests;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import org.zcode.javareceiver.Zcode;
import org.zcode.javareceiver.ZcodeBusInterruptSource;
import org.zcode.javareceiver.ZcodeParameters;
import org.zcode.javareceiver.commands.ZcodeActivateCommand;
import org.zcode.javareceiver.commands.ZcodeCapabilitiesCommand;
import org.zcode.javareceiver.commands.ZcodeEchoCommand;
import org.zcode.javareceiver.parsing.ZcodeCommandChannel;
import org.zcode.zcode_acceptance_tests.ZcodeAcceptanceTestConnectionManager;

import io.cucumber.java.en.Given;

public class JavaReceiverCucumberStartup {
    private static boolean hasStarted = false;
    private static ExecutorService execs;

    @Given("the target is running and connected")
    public void the_target_is_running_and_connected() {
        if (!hasStarted) {
            hasStarted = true;
            Zcode z = new Zcode(new ZcodeParameters(false), new ZcodeBusInterruptSource[0]);
            LocalTestConnection con = new LocalTestConnection(z, new ZcodeParameters(false), false);
            z.setChannels(new ZcodeCommandChannel[] { con });
            z.getCommandFinder().registerCommand(new ZcodeIdentifyCommand());
            z.getCommandFinder().registerCommand(new ZcodeEchoCommand());
            z.getCommandFinder().registerCommand(new ZcodeCapabilitiesCommand(new ZcodeParameters(false), z));
            z.getCommandFinder().registerCommand(new ZcodeActivateCommand());
            ZcodeAcceptanceTestConnectionManager.registerConnection(con);
            execs = Executors.newSingleThreadExecutor();
            execs.submit(() -> {
                try {
                    while (true) {
                        z.progressZcode();
                    }
                } catch (Throwable t) {
                    t.printStackTrace();
                }
            });
        }
    }

}
