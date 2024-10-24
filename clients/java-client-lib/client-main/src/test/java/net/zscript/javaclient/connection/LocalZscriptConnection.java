package net.zscript.javaclient.connection;

import javax.annotation.Nonnull;
import java.io.ByteArrayOutputStream;
import java.util.ArrayDeque;
import java.util.Queue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.function.Consumer;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.connectors.RawConnection;
import net.zscript.javareceiver.core.OutputStreamOutStream;
import net.zscript.javareceiver.core.SequenceOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.core.ZscriptChannel;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.modules.outerCore.ZscriptOuterCoreModule;
import net.zscript.tokenizer.TokenRingBuffer;
import net.zscript.tokenizer.Tokenizer;

public class LocalZscriptConnection extends RawConnection {
    private static final Logger           LOG    = LoggerFactory.getLogger(LocalZscriptConnection.class);
    private final        ExecutorService  exec   = Executors.newSingleThreadExecutor();
    private final        Queue<byte[]>    dataIn = new ArrayDeque<>();
    private              Consumer<byte[]> responseHandler;

    @Override
    protected Logger getLogger() {
        return LOG;
    }

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
            // resubmit itself
            exec.submit(this);
        }
    }

    public LocalZscriptConnection() {
        Zscript zscript = new Zscript();

        zscript.addModule(new ZscriptCoreModule());
        zscript.addModule(new ZscriptOuterCoreModule());

        TokenRingBuffer ringBuffer = TokenRingBuffer.createBufferWithCapacity(100);
        final SequenceOutStream outStream = new OutputStreamOutStream<>(new ByteArrayOutputStream()) {
            @Override
            public void close() {
                responseHandler.accept(getOutputStream().toByteArray());
                getOutputStream().reset();
            }
        };

        zscript.addChannel(new ZscriptChannel(ringBuffer, outStream) {
            final Tokenizer in = new Tokenizer(ringBuffer.getTokenWriter(), 2);

            byte[] current = null;
            int pos = 0;

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
            public void channelInfo(@Nonnull CommandContext view) {
                view.getOutStream().writeBigFieldQuoted("Basic text channel");
            }

            @Override
            public void channelSetup(@Nonnull CommandContext view) {
            }
        });
        exec.submit(new ProgressForever(zscript));
    }

    @Override
    public void send(@Nonnull byte[] data) {
        exec.submit(() -> dataIn.add(data));
    }

    @Override
    public void onReceiveBytes(@Nonnull Consumer<byte[]> responseHandler) {
        this.responseHandler = responseHandler;
    }

    public void close() {
    }
}
