package net.zscript.acceptance;

import io.cucumber.java.en.Given;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.nodes.DirectConnection;
import net.zscript.javaclient.testing.LocalConnection;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.testing.LocalChannel;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.TokenRingBuffer;

/**
 * Steps that relate to a locally running zscript device Java emulation, see {@link Zscript}.
 */
public class LocalDeviceSteps {
    private static final Logger LOG = LoggerFactory.getLogger(LocalDeviceSteps.class);

    private Zscript          zscript;
    private DirectConnection conn;

    public boolean progressZscriptDevice() {
        return zscript.progress();
    }

    public DirectConnection getLocalConnection() {
        if (zscript == null) {
            throw new IllegalStateException("You need 'a locally running zscript device'");
        }
        if (conn == null) {
            throw new IllegalStateException("You need 'a connection to that local device'");
        }
        return conn;
    }

    @Given("a locally running zscript device")
    public void runningZscriptDevice() {
        runningZscriptDevice(128);
    }

    @Given("a locally running zscript device with buffer size {int}")
    public void runningZscriptDevice(int sz) {
        if (zscript != null) {
            throw new IllegalStateException("zscript device already initialized");
        }

        final TokenBuffer  buffer       = TokenRingBuffer.createBufferWithCapacity(sz);
        final LocalChannel localChannel = new LocalChannel(buffer);

        zscript = new Zscript();
        zscript.addModule(new ZscriptCoreModule());
        zscript.addChannel(localChannel);
    }

    @Given("a connection to that local device")
    public void connectionToDevice() {
        LocalChannel channel = (LocalChannel) zscript.getChannels().get(0);
        if (conn != null) {
            throw new IllegalStateException("Connection already initialized");
        }
        conn = new LocalConnection(channel.getCommandOutputStream(), channel.getResponseInputStream());
    }

    @Given("a connection to a local zscript device")
    public void connectionToZscriptDevice() {
        runningZscriptDevice();
        connectionToDevice();
    }

}
