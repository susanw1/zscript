package net.zscript.javaclient.connectors.serial;

import static java.util.Objects.requireNonNull;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortEvent;
import com.fazecast.jSerialComm.SerialPortIOException;
import com.fazecast.jSerialComm.SerialPortMessageListener;
import com.fazecast.jSerialComm.SerialPortMessageListenerWithExceptions;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.function.Consumer;

import net.zscript.javaclient.connectors.RawConnection;

/**
 *
 */
// Note: SerialPort is hard to use in tests as it contains Android references which cause Mocking failure.
public class SerialConnection extends RawConnection {
    private final SerialPort   commPort;
    private final OutputStream out;

    private Consumer<byte[]> handler;

    public SerialConnection(SerialPort commPort) throws IOException {
        this.commPort = commPort;

        if (!commPort.isOpen() && !commPort.openPort()) {
            throw new SerialPortIOException("Port cannot be opened: " + commPort);
        }
        this.out = commPort.getOutputStream();
    }

    public void send(final byte[] data) throws IOException {
        out.write(data);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onReceiveBytes(final Consumer<byte[]> responseHandler) {
        if (this.handler != null) {
            throw new IllegalStateException("Handler already assigned");
        }
        this.handler = requireNonNull(responseHandler, "handler");

        commPort.addDataListener(new SerialPortMessageListenerWithExceptions() {
            @Override
            public void catchException(Exception e) {
                e.printStackTrace();
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
