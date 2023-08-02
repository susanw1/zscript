package net.zscript.javaclient.responseparser;

import java.io.ByteArrayOutputStream;
import java.util.ArrayDeque;
import java.util.Queue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.function.Consumer;

import net.zscript.javaclient.responseparser.ZscriptConnection;
import net.zscript.javareceiver.core.OutputStreamOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.core.ZscriptChannel;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.modules.outerCore.ZscriptOuterCoreModule;
import net.zscript.javareceiver.tokenizer.TokenRingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;

public class LocalZscriptConnection implements ZscriptConnection {
    private final ExecutorService exec   = Executors.newSingleThreadExecutor();
    private final Queue<byte[]>   dataIn = new ArrayDeque<>();
    private Consumer<byte[]>      handler;

    private class ProgressForever implements Runnable {
        private final Zscript zscript;

        public ProgressForever(Zscript zscript) {
            this.zscript = zscript;
        }

        @Override
        public void run() {
            try {
                zscript.progress();
            } catch (Exception e) {
                e.printStackTrace();
            }
            exec.submit(this);
        }

    }

    public LocalZscriptConnection() {
        Zscript zscript = new Zscript();

        zscript.addModule(new ZscriptCoreModule());
        zscript.addModule(new ZscriptOuterCoreModule());

        TokenRingBuffer rbuff = TokenRingBuffer.createBufferWithCapacity(100);
        zscript.addChannel(new ZscriptChannel(rbuff, new OutputStreamOutStream<>(new ByteArrayOutputStream()) {

            @Override
            public void close() {
                handler.accept(getOutputStream().toByteArray());
                getOutputStream().reset();
            }
        }) {
            Tokenizer in = new Tokenizer(rbuff.getTokenWriter(), 2);

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
            public void channelInfo(CommandContext view) {
                view.getOutStream().writeQuotedString("Basic text channel");
            }

            @Override
            public void channelSetup(CommandContext view) {
            }

        });
        exec.submit(new ProgressForever(zscript));
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
