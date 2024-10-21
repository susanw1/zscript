package net.zscript.javaclient.connectors.serial;

import java.io.IOException;
import java.io.OutputStream;
import java.util.function.Consumer;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortEvent;
import com.fazecast.jSerialComm.SerialPortIOException;
import com.fazecast.jSerialComm.SerialPortMessageListenerWithExceptions;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.nodes.DirectConnection;

/**
 *
 */
// Note: SerialPort is hard to use in tests as it contains Android references which cause Mocking failure.
public class SerialConnection extends DirectConnection {
    private static final Logger LOG = LoggerFactory.getLogger(SerialConnection.class);

    private final SerialPort   commPort;
    private final OutputStream out;

    private Consumer<byte[]> responseHandler;

    public SerialConnection(SerialPort commPort) throws IOException {
        this.commPort = commPort;

        if (!commPort.isOpen() && !commPort.openPort()) {
            throw new SerialPortIOException("Port cannot be opened: " + commPort);
        }
        this.out = commPort.getOutputStream();
    }

    @Override
    protected Logger getLogger() {
        return LOG;
    }

    public void send(final byte[] data) throws IOException {
        out.write(data);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onReceiveBytes(final Consumer<byte[]> responseHandler) {
        if (this.responseHandler != null) {
            commPort.removeDataListener();
        }

        this.responseHandler = responseHandler;

        commPort.addDataListener(new SerialPortMessageListenerWithExceptions() {
            @Override
            public void catchException(Exception e) {
                LOG.error(e.getMessage());
            }

            @Override
            public int getListeningEvents() {
                return SerialPort.LISTENING_EVENT_DATA_RECEIVED;
            }

            @Override
            public void serialEvent(SerialPortEvent serialPortEvent) {
                if (serialPortEvent.getEventType() == SerialPort.LISTENING_EVENT_DATA_RECEIVED) {
                    responseHandler.accept(serialPortEvent.getReceivedData());
                }
            }

            @Override
            public byte[] getMessageDelimiter() {
                return new byte[] { '\n' };
            }

            @Override
            public boolean delimiterIndicatesEndOfMessage() {
                return true;
            }

        });
    }

    @Override
    public void close() {
        commPort.removeDataListener();
        commPort.closePort();
    }

    @Override
    public String toString() {
        return commPort.toString();
    }
}
