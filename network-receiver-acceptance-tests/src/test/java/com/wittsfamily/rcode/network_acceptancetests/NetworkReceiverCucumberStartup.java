package com.wittsfamily.rcode.network_acceptancetests;

import java.net.DatagramPacket;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.function.Consumer;

import javax.swing.JOptionPane;

import com.wittsfamily.rcode_acceptance_tests.RCodeAcceptanceTestConnectionManager;

import io.cucumber.java.en.Given;

public class NetworkReceiverCucumberStartup {
    private static boolean hasStarted = false;

    @Given("the target is running and connected")
    public void the_target_is_running_and_connected() {
        if (!hasStarted) {
            hasStarted = true;
            try {
                NetworkAcceptanceTestConnection connection = new NetworkAcceptanceTestConnection();
                SocketAddress target = null;
                while (target == null) {
                    boolean foundPort = false;
                    int port = 0;
                    while (!foundPort) {
                        String portStr = JOptionPane.showInputDialog("Enter the target device's port");
                        if (portStr == null) {
                            throw new RuntimeException("No port entered");
                        }
                        try {
                            port = Integer.parseInt(portStr);
                            foundPort = true;
                        } catch (NumberFormatException e) {
                            e.printStackTrace();
                        }
                    }
                    connection.watchResponses();
                    BroadcastListener listener = new BroadcastListener();

                    connection.registerBroadcastListener(listener);
                    while (target == null) {
                        int lastSize = listener.getByteResults().size();
                        connection.scanForTargets("*R0\n".getBytes(StandardCharsets.UTF_8), port);
                        long startTime = System.currentTimeMillis();

                        while (listener.getByteResults().size() <= lastSize && System.currentTimeMillis() - startTime < 2500) {
                            Thread.sleep(500);
                        }
                        if (listener.getByteResults().size() == lastSize) {
                            int result = JOptionPane.showOptionDialog(null, "No devices have been found, do you want to re-send the broadcast, or choose a new port?",
                                    "No devices found", JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE, null, new String[] { "Re-send", "Choose new port", "Give up" },
                                    "Re-send");
                            if (result == JOptionPane.YES_OPTION) {
                                continue;
                            } else if (result == JOptionPane.NO_OPTION) {
                                break;
                            } else {
                                throw new RuntimeException("Search for target device aborted");
                            }
                        } else {
                            String[] options = new String[listener.getByteResults().size()];
                            for (int i = 0; i < listener.getByteResults().size(); i++) {
                                options[i] = StandardCharsets.UTF_8.decode(ByteBuffer.wrap(listener.getByteResults().get(i))).toString();
                            }
                            String result = (String) JOptionPane.showInputDialog(null, "These responses have been received, please select the correct response",
                                    "Select target device", JOptionPane.PLAIN_MESSAGE, null, options, options[0]);
                            if (result != null) {
                                int targetResult = -1;
                                for (int i = 0; i < options.length; i++) {
                                    if (options[i].equals(result)) {
                                        targetResult = i;
                                        break;
                                    }
                                }
                                target = listener.getAddresses().get(targetResult);
                            }
                        }
                    }
                }
                connection.setTarget(target);
                connection.clearListeners();
                RCodeAcceptanceTestConnectionManager.registerConnection(connection);
            } catch (Exception e) {
                throw new RuntimeException(e);
            }

        }
    }

    private class BroadcastListener implements Consumer<DatagramPacket> {
        private final Lock lock = new ReentrantLock();
        private List<byte[]> byteResults = new ArrayList<>();
        private List<SocketAddress> addresses = new ArrayList<>();

        @Override
        public void accept(DatagramPacket p) {
            lock.lock();
            byteResults.add(p.getData());
            addresses.add(p.getSocketAddress());
            lock.unlock();
        }

        public List<byte[]> getByteResults() {
            lock.lock();
            lock.unlock();
            return new ArrayList<>(byteResults);
        }

        public List<SocketAddress> getAddresses() {
            lock.lock();
            lock.unlock();
            return new ArrayList<>(addresses);
        }

    }
}
