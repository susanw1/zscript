package net.zscript.javaclient.responseParser;

import java.io.ByteArrayOutputStream;
import java.util.ArrayDeque;
import java.util.Queue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.function.Consumer;

import net.zscript.javaclient.responseParser.ZcodeConnection;
import net.zscript.javareceiver.core.OutputStreamOutStream;
import net.zscript.javareceiver.core.Zcode;
import net.zscript.javareceiver.core.ZcodeChannel;
import net.zscript.javareceiver.execution.ZcodeCommandContext;
import net.zscript.javareceiver.modules.core.ZcodeCoreModule;
import net.zscript.javareceiver.modules.outerCore.ZcodeOuterCoreModule;
import net.zscript.javareceiver.tokenizer.ZcodeTokenRingBuffer;
import net.zscript.javareceiver.tokenizer.ZcodeTokenizer;

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
        Zcode zcode = new Zcode();

        zcode.addModule(new ZcodeCoreModule());
        zcode.addModule(new ZcodeOuterCoreModule());

        ZcodeTokenRingBuffer rbuff = ZcodeTokenRingBuffer.createBufferWithCapacity(100);
        zcode.addChannel(new ZcodeChannel(rbuff, new OutputStreamOutStream<>(new ByteArrayOutputStream()) {

            @Override
            public void close() {
                handler.accept(getOutputStream().toByteArray());
                getOutputStream().reset();
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
                    }
                    current = dataIn.poll();
                    pos = 0;
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
                view.getOutStream().writeQuotedString("Basic text channel");
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
