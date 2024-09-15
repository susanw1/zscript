package net.zscript.javaclient.connectors.serial;

import javax.annotation.Nonnull;
import java.io.IOException;
import java.util.function.Consumer;

import static java.util.Arrays.stream;

import com.fazecast.jSerialComm.SerialPort;

import net.zscript.javaclient.connectors.ZscriptConnectors;

public class SerialConnector implements ZscriptConnectors.ZscriptConnector<SerialPort> {
    public SerialConnector() {

    }

    @Nonnull
    @Override
    public SerialConnection connect(SerialPort commPort) throws IOException {
        return new SerialConnection(commPort);
    }

    @Override
    public void scan(Consumer<SerialPort> scanResult) {
        stream(SerialPort.getCommPorts())
                .forEach(scanResult);
    }
}
