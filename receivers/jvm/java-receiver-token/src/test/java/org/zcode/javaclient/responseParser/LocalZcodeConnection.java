package org.zcode.javaclient.responseParser;

import java.io.ByteArrayOutputStream;
import java.util.ArrayDeque;
import java.util.Queue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.function.Consumer;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeAbstractOutStream;
import org.zcode.javareceiver.core.ZcodeChannel;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javareceiver.modules.ZcodeCommandFinder;
import org.zcode.javareceiver.modules.core.ZcodeCoreModule;
import org.zcode.javareceiver.modules.outerCore.ZcodeOuterCoreModule;
import org.zcode.javareceiver.tokenizer.ZcodeTokenRingBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

public class LocalZcodeConnection implements ZcodeConnection {
    private final ExecutorService exec   = Executors.newSingleThreadExecutor();
    private final Queue<byte[]>   dataIn = new ArrayDeque<>();
    private Consumer<byte[]>      handler;

    private class ProgressForever implements Runnable {
        private final Zcode zcode;

        public ProgressForever(Zcode zcode) {
            this.zcode = zcode;
        }

        @Override
        public void run() {
            try {
                zcode.progress();
            } catch (Exception e) {
                e.printStackTrace();
            }
            exec.submit(this);
        }

    }

    public LocalZcodeConnection() {

        ZcodeCommandFinder.addModule(new ZcodeCoreModule());
        ZcodeCommandFinder.addModule(new ZcodeOuterCoreModule());

        Zcode                zcode = Zcode.getZcode();
        ZcodeTokenRingBuffer rbuff = ZcodeTokenRingBuffer.createBufferWithCapacity(100);
        zcode.addChannel(new ZcodeChannel(rbuff, new ZcodeAbstractOutStream() {
            private ByteArrayOutputStream stream = null;

            @Override
            public void writeByte(byte c) {
                stream.write(c);
            }

            @Override
            public void open() {
                stream = new ByteArrayOutputStream();
            }

            @Override
            public boolean isOpen() {
                return stream != null;
            }

            @Override
            public void close() {
                handler.accept(stream.toByteArray());
                stream = null;
            }
        }) {
            ZcodeTokenizer in = new ZcodeTokenizer(rbuff.getTokenWriter(), 2);

            byte[] current = null;
            int    pos     = 0;

            @Override
            public void moveAlong() {
                if (current == null) {
                    if (dataIn.isEmpty()) {
                        return;
                    } else {
                        current = dataIn.poll();
                        pos = 0;
                    }
                }
                while (pos < current.length && in.offer(current[pos])) {
                    pos++;
                }
                if (pos >= current.length) {
                    current = null;
                }
            }

            @Override
            public void channelInfo(ZcodeCommandContext view) {
                view.getOutStream().writeString("Basic text channel");
            }

            @Override
            public void channelSetup(ZcodeCommandContext view) {
            }

        });
        exec.submit(new ProgressForever(zcode));
    }

    @Override
    public void send(byte[] data) {
        exec.submit(() -> dataIn.add(data));
    }

    @Override
    public void onReceive(Consumer<byte[]> handler) {
        this.handler = handler;
    }

}