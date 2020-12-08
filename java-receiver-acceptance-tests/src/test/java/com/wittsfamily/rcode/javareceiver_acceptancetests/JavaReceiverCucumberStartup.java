package com.wittsfamily.rcode.javareceiver_acceptancetests;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeBusInterruptSource;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.commands.RCodeActivateCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeCapabilitiesCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeEchoCommand;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode_acceptance_tests.RCodeAcceptanceTestConnectionManager;

import io.cucumber.java.en.Given;

public class JavaReceiverCucumberStartup {
    private static boolean hasStarted = false;
    private static ExecutorService execs;

    @Given("the target is running and connected")
    public void the_target_is_running_and_connected() {
        if (!hasStarted) {
            hasStarted = true;
            RCode r = new RCode(new RCodeParameters(false), new RCodeBusInterruptSource[0]);
            LocalTestConnection con = new LocalTestConnection(r, new RCodeParameters(false), false);
            r.setChannels(new RCodeCommandChannel[] { con });
            r.getCommandFinder().registerCommand(new RCodeIdentifyCommand());
            r.getCommandFinder().registerCommand(new RCodeEchoCommand());
            r.getCommandFinder().registerCommand(new RCodeCapabilitiesCommand(new RCodeParameters(false), r));
            r.getCommandFinder().registerCommand(new RCodeActivateCommand());
            RCodeAcceptanceTestConnectionManager.registerConnection(con);
            execs = Executors.newSingleThreadExecutor();
            execs.submit(() -> {
                try {
                    while (true) {
                        r.progressRCode();
                    }
                } catch (Throwable t) {
                    t.printStackTrace();
                }
            });
        }
    }

}
