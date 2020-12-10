package com.wittsfamily.rcode.network_acceptancetests;

import static java.util.Collections.list;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InterfaceAddress;
import java.net.NetworkInterface;
import java.net.SocketAddress;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.function.Consumer;

import com.wittsfamily.rcode_acceptance_tests.acceptancetest_asserts.RCodeAcceptanceTestConnection;

public class NetworkAcceptanceTestConnection implements RCodeAcceptanceTestConnection {
    private final Lock lock = new ReentrantLock();
    private final List<Consumer<byte[]>> handlers = new ArrayList<>();
    private final List<Consumer<DatagramPacket>> broadcastHandlers = new ArrayList<>();
    private final ExecutorService exec;
    private final DatagramSocket s;
    private SocketAddress target = null;

    public NetworkAcceptanceTestConnection() throws SocketException {
        s = new DatagramSocket(43244);
        exec = Executors.newSingleThreadExecutor();
    }

    public void watchResponses() {
        exec.submit(() -> {
            try {
                while (true) {
                    byte[] received = new byte[10000];
                    DatagramPacket p = new DatagramPacket(received, received.length);
                    s.receive(p);
                    lock.lock();
                    try {
                        if (p.getLength() > 0 && p.getSocketAddress().equals(target)) {
                            byte[] result = new byte[p.getLength()];
                            for (int i = 0; i < result.length; i++) {
                                result[i] = received[i];
                            }
                            for (Consumer<byte[]> handler : handlers) {
                                handler.accept(result);
                            }
                        }
                        if (p.getLength() > 0) {
                            for (Consumer<DatagramPacket> handler : broadcastHandlers) {
                                handler.accept(p);
                            }
                        }
                    } finally {
                        lock.unlock();
                    }
                }
            } catch (Throwable t) {
                t.printStackTrace();
            }
        });
    }

    public void setTarget(SocketAddress target) {
        this.target = target;
    }

    public void scanForTargets(byte[] message, int port) {
        lock.lock();
        try {
            for (NetworkInterface nic : list(NetworkInterface.getNetworkInterfaces())) {
                if (nic.isUp() && !nic.isLoopback()) {
                    for (InterfaceAddress nicAddr : nic.getInterfaceAddresses()) {
                        InetAddress broadcastAddr = nicAddr.getBroadcast();
                        if (broadcastAddr == null) {
                            continue;
                        }
                        s.send(new DatagramPacket(message, message.length, broadcastAddr, port));
                    }
                }
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        } finally {
            lock.unlock();
        }
    }

    @Override
    public void send(byte[] message) {
        lock.lock();
        try {
            DatagramPacket p = new DatagramPacket(message, message.length, target);
            s.send(p);
        } catch (IOException e) {
            throw new RuntimeException(e);
        } finally {
            lock.unlock();
        }
    }

    @Override
    public void registerListener(Consumer<byte[]> handler) {
        lock.lock();
        try {
            handlers.add(handler);
        } finally {
            lock.unlock();
        }
    }

    public void registerBroadcastListener(Consumer<DatagramPacket> handler) {
        lock.lock();
        try {
            broadcastHandlers.add(handler);
        } finally {
            lock.unlock();
        }
    }

    @Override
    public void clearListeners() {
        lock.lock();
        try {
            handlers.clear();
            broadcastHandlers.clear();
        } finally {
            lock.unlock();
        }
    }

}
