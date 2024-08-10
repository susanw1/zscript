package net.zscript.javaclient.connection;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;

import com.fazecast.jSerialComm.SerialPort;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.connectors.RawConnection;
import net.zscript.javaclient.connectors.ZscriptConnectors;
import net.zscript.javaclient.connectors.serial.SerialConnector;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.model.modules.base.CoreModule;
import net.zscript.tokenizer.TokenExtendingBuffer;
import net.zscript.tokenizer.Tokenizer;

class SerialMain {
    public static void main(String[] args) throws IOException, InterruptedException {
        SerialConnector connector = ZscriptConnectors.SERIAL;
        SerialPort      commPort  = SerialPort.getCommPort("/dev/ttyACM0");

        SerialPort[] p = SerialPort.getCommPorts();

        connector.scan(System.out::println);

        // Note: 115200 makes Arduino UNO take ~6-10s to start accepting.
        commPort.setBaudRate(9600);

        if (!commPort.openPort()) {
            throw new IOException("port failed to open");
        }

        OutputStream outputStream = commPort.getOutputStream();

        System.out.println("Opened. Before sleep...");
        //        Thread.sleep(10000);
        System.out.println("After sleep");

        //        for (int i = 0; i < 20; i++) {
        //            System.out.println("Loop: " + i);
        //            outputStream.write('A');
        //            outputStream.flush();
        //            Thread.sleep(500);
        //            outputStream.write('B');
        //            outputStream.flush();
        //            Thread.sleep(500);
        //        }

        //        InputStream in = commPort.getInputStream();
        //        int         ch;
        //        while ((ch = in.read()) != -1) {
        //            System.out.print((char) ch);
        //        }

        //        commPort.addDataListener(new SerialPortMessageListener() {
        //            @Override
        //            public byte[] getMessageDelimiter() {
        //                return new byte[] { '\n' };
        //            }
        //
        //            @Override
        //            public boolean delimiterIndicatesEndOfMessage() {
        //                return true;
        //            }
        //
        //            @Override
        //            public int getListeningEvents() {
        //                return SerialPort.LISTENING_EVENT_DATA_RECEIVED;
        //            }
        //
        //            @Override
        //            public void serialEvent(SerialPortEvent serialPortEvent) {
        //                System.out.println("Event: " + serialPortEvent);
        //                if (serialPortEvent.getEventType() == SerialPort.LISTENING_EVENT_DATA_RECEIVED) {
        //                    System.out.println("Response: " + Arrays.toString(serialPortEvent.getReceivedData()));
        //                }
        //            }
        //        });
        //
        //        for (int i = 0; i < 10; i++) {
        //            outputStream.write(new byte[] { 'Z', '1', 'A', '3', '\n' });
        //            Thread.sleep(1000);
        //        }

        try (RawConnection conn = connector.connect(commPort)) {
            conn.onReceive((response) -> {
                System.out.println("Response: " + response.toString());
            });
            for (int i = 0; i < 10; i++) {
                byte[]               ba     = CoreModule.echoBuilder().setAny('A', 35).build().asString().getBytes(StandardCharsets.UTF_8);
                TokenExtendingBuffer buffer = new TokenExtendingBuffer();
                Tokenizer            t      = new Tokenizer(buffer.getTokenWriter(), 2);
                for (byte b : ba) {
                    t.accept(b);
                }
                conn.send(new AddressedCommand(CommandSequence.from(CommandExecutionPath.parse(buffer.getTokenReader().getFirstReadToken()), -1)));
                System.out.println("Sending: " + Arrays.toString(ba));
                Thread.sleep(1000);
            }
        } catch (Exception e) {
        }

        Thread.sleep(1000);

        System.out.println("Finished!");
    }
}
