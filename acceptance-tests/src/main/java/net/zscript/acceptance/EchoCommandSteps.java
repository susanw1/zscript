package net.zscript.acceptance;

import java.io.IOException;

import static java.time.Duration.ofSeconds;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.awaitility.Awaitility.await;

import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.nodes.DirectConnection;
import net.zscript.javaclient.testing.LocalConnection;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.testing.CollectingConsumer;
import net.zscript.javareceiver.testing.LocalChannel;
import net.zscript.model.modules.base.CoreModule;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.TokenRingBuffer;

public class EchoCommandSteps {
    private static final Logger LOG = LoggerFactory.getLogger(EchoCommandSteps.class);

    private Zscript zscript;

    //    private Device  testDevice;
    private       DirectConnection           conn;
    //    private CollectingConsumer<AddressedResponse> respCollector;
    //    private CollectingConsumer<ByteString> bytesCollector  = new CollectingConsumer<>();
    private final CollectingConsumer<byte[]> bytesCollector2 = new CollectingConsumer<>();

    @Given("a running zscript device")
    public void runningZscriptDevice() {
        LOG.info("GIVEN a running zscript device");

        // ===== Set up zscript device emulator
        final TokenBuffer  buffer       = TokenRingBuffer.createBufferWithCapacity(128);
        final LocalChannel localChannel = new LocalChannel(buffer);

        zscript = new Zscript();
        zscript.addModule(new ZscriptCoreModule());
        zscript.addChannel(localChannel);
    }

    @Given("a connection to the device")
    public void connectionToDevice() {
        LOG.info("GIVEN a connection to the device");

        LocalChannel channel = (LocalChannel) zscript.getChannels().get(0);
        conn = new LocalConnection(channel.getCommandOutputStream(), channel.getResponseInputStream());
        conn.onReceiveBytes(bytesCollector2);

        //        final ZscriptNode node = ZscriptNode.newNode(conn);

        //        final ZscriptModel model = ZscriptModel.standardModel();

        //        testDevice = new Device(model, node);
    }

    @Given("a connection to a zscript device")
    public void connectionToZscriptDevice() {
        LOG.info("GIVEN a connection to a zscript device!");
        runningZscriptDevice();
        connectionToDevice();
    }

    @When("I send an echo command")
    public void sendAnEchoCommand() throws IOException {
        LOG.debug("WHEN I send an echo command");
        CoreModule c;
        LOG.debug("Sending... ");

        conn.sendBytes(byteStringUtf8("Z1 A8\n").toByteArray());

        //        testDevice.send(from("Z1\n", UTF8_APPENDER), bytesCollector);
        LOG.debug("Sent");
    }

    @Then("I should receive an echo response")
    public void shouldReceiveAnEchoResponse() {
        LOG.debug("THEN I should receive an echo response");

        await().atMost(ofSeconds(10)).until(() -> !zscript.progress() && !bytesCollector2.isEmpty());

        LOG.trace("bytesCollector2 empty? {}", bytesCollector2.isEmpty());
        assertThat(bytesCollector2.next().get()).contains(byteStringUtf8("!A8S\n").toByteArray());
    }
}
