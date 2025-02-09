package net.zscript.acceptance;

import java.util.Optional;

import io.cucumber.java.en.Given;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.devicenodes.DirectConnection;
import net.zscript.javaclient.local.LocalConnection;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.testing.LocalChannel;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.TokenRingBuffer;

/**
 * Steps that relate to a locally running zscript-receiving Java emulation, see {@link Zscript}.
 */
public class LocalJavaReceiverSteps {
    private static final Logger LOG = LoggerFactory.getLogger(LocalJavaReceiverSteps.class);

    private Zscript          zscript;
    private DirectConnection localConn;

    public boolean progressZscriptDevice() {
        return zscript.progress();
    }

    public Optional<DirectConnection> getLocalConnectionIfSet() {
        return Optional.ofNullable(localConn);
    }

    public DirectConnection getLocalConnection() {
        if (zscript == null) {
            throw new IllegalStateException("You need 'a locally running zscript receiver'");
        }
        if (localConn == null) {
            throw new IllegalStateException("You need 'a connection to that local receiver'");
        }
        return localConn;
    }

    @Given("a locally running zscript receiver")
    public void runningZscriptReceiver() {
        runningZscriptReceiver(128);
    }

    @Given("a locally running zscript receiver with buffer size {int}")
    public void runningZscriptReceiver(int sz) {
        if (zscript != null) {
            throw new IllegalStateException("zscript receiver already initialized");
        }

        final TokenBuffer  buffer       = TokenRingBuffer.createBufferWithCapacity(sz);
        final LocalChannel localChannel = new LocalChannel(buffer);

        zscript = new Zscript();
        zscript.addModule(new ZscriptCoreModule());
        zscript.addChannel(localChannel);
    }

    @Given("a connection to that local receiver")
    public void connectionToLocalJavaReceiver() {
        if (zscript == null) {
            throw new IllegalStateException("You need 'a locally running zscript receiver'");
        }
        if (localConn != null) {
            throw new IllegalStateException("Connection already initialized");
        }
        LocalChannel channel = (LocalChannel) zscript.getChannels().get(0);
        localConn = new LocalConnection(channel.getCommandOutputStream(), channel.getResponseInputStream());
    }

    @Given("a connection to a local zscript receiver")
    public void runAndConnectToZscriptReceiver() {
        runningZscriptReceiver();
        connectionToLocalJavaReceiver();
    }

    public boolean isConnected() {
        return localConn != null;
    }
}
