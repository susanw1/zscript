package org.zcode.network_acceptancetests;

import java.io.UncheckedIOException;
import java.net.DatagramPacket;
import java.net.SocketAddress;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.function.Consumer;

import javax.swing.JOptionPane;

import org.assertj.core.api.Assertions;
import org.junit.AssumptionViolatedException;
import org.zcode.zcode_acceptance_tests.ZcodeAcceptanceTestConnectionManager;

import io.cucumber.java.en.Given;

public class NetworkReceiverCucumberStartup {

    private static boolean hasStarted = false;

    @Given("the target is running and connected")
    public void the_target_is_running_and_connected() {
        if (!hasStarted) {
            hasStarted = true;
            try {
                NetworkAcceptanceTestConnection connection = new NetworkAcceptanceTestConnection();
                BroadcastListener               listener   = new BroadcastListener();
                connection.registerBroadcastListener(listener);

                connection.watchResponses();

                SocketAddress target = null;

                do {
                    Integer port = Integer.getInteger("zcode.network.acceptance-tests.port");

                    if (port == null) {
                        port = promptUserForPort();
                    }

                    String deviceIdent = System.getProperty("zcode.network.acceptance-tests.device-ident", null);

                    if (deviceIdent == null) {
                        target = promptUserForTarget(connection, listener, port);
                    } else {
                        target = checkForSpecifiedTarget(connection, listener, port, deviceIdent);
                    }

                } while (target == null);

                connection.clearListeners();
                connection.setTarget(target);
                ZcodeAcceptanceTestConnectionManager.registerConnection(connection);
            } catch (InterruptedException e) {
                System.err.println("interrupted");
            } catch (SocketException e) {
                throw new UncheckedIOException("Search for target device aborted", e);
            }
        }
    }

    /**
     * @param connection the (UDP) connection to use to send probe messages
     * @param listener   the registered listener to pick up broadcast responses
     * @param port       the port to use
     * @return the SocketAddress of identified device, or null if aborted
     * @throws InterruptedException
     * @throws AssumptionViolatedException if search is aborted
     */
    private SocketAddress promptUserForTarget(NetworkAcceptanceTestConnection connection, BroadcastListener listener, int port) throws InterruptedException {
        while (true) {
            doBroadcastScan(connection, listener, port);

            if (listener.getResponseCount() == 0) {
                int result = JOptionPane.showOptionDialog(null, "No devices have been found, do you want to re-send the broadcast, or choose a new port?",
                        "No devices found", JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE, null, new String[] { "Re-send", "Choose new port", "Give up" },
                        "Re-send");
                if (result == JOptionPane.YES_OPTION) {
                    continue;
                } else if (result == JOptionPane.NO_OPTION) {
                    return null;
                } else {
                    throw new AssumptionViolatedException("Search for target device aborted");
                }
            } else {
                Object[] options = listener.getResponses().toArray();
                String   chosen  = (String) JOptionPane.showInputDialog(null, "These responses have been received, please select the correct response",
                        "Select target device", JOptionPane.PLAIN_MESSAGE, null, options, options[0]);

                if (chosen != null) {
                    for (int i = 0; i < options.length; i++) {
                        if (options[i].equals(chosen)) {
                            return listener.getAddresses().get(i);
                        }
                    }
                }
            }
        }
    }

    private SocketAddress checkForSpecifiedTarget(NetworkAcceptanceTestConnection connection, BroadcastListener listener, int port, String deviceIdent)
            throws InterruptedException {
        doBroadcastScan(connection, listener, port);

        int index = listener.getResponses().indexOf(deviceIdent);
        if (index == -1) {
            Assertions.fail("No device found matching '" + deviceIdent + "'; responses found were: " + listener.getResponses());
        }
        return listener.getAddresses().get(index);
    }

    private void doBroadcastScan(NetworkAcceptanceTestConnection connection, BroadcastListener listener, int port) throws InterruptedException {
        int previousResponseCount = listener.getResponseCount();
        connection.scanForTargets("*R0\n".getBytes(StandardCharsets.UTF_8), port);
        long startTime = System.currentTimeMillis();
        while (listener.getResponseCount() == previousResponseCount && System.currentTimeMillis() - startTime < 2500) {
            Thread.sleep(500);
        }
    }

    private int promptUserForPort() {
        boolean foundPort = false;

        int    port                = 0;
        String initialPromptString = "Enter the target device's port";
        String promptString        = initialPromptString;

        while (!foundPort) {
            String portString = JOptionPane.showInputDialog(promptString);
            if (portString == null) {
                throw new AssumptionViolatedException("No port entered");
            }
            try {
                port = Integer.parseInt(portString);
                foundPort = true;
            } catch (NumberFormatException e) {
                promptString = e.toString() + "\n" + initialPromptString;
            }
        }
        return port;
    }

    private static class BroadcastListener implements Consumer<DatagramPacket> {
        private List<String>        responses = new ArrayList<>();
        private List<SocketAddress> addresses = new ArrayList<>();

        @Override
        public synchronized void accept(DatagramPacket p) {
            responses.add(StandardCharsets.UTF_8.decode(ByteBuffer.wrap(p.getData())).toString());
            addresses.add(p.getSocketAddress());
        }

        public synchronized int getResponseCount() {
            return responses.size();
        }

        public synchronized List<String> getResponses() {
            return new ArrayList<>(responses);
        }

        public synchronized List<SocketAddress> getAddresses() {
            return new ArrayList<>(addresses);
        }

    }
}
