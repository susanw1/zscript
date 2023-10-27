package net.zscript.demo.morse;

import com.fazecast.jSerialComm.SerialPort;

import java.io.IOException;
import java.net.Socket;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;

import net.zscript.javaclient.connectors.serial.SerialConnection;
import net.zscript.javaclient.connectors.tcp.TcpConnection;
import net.zscript.javaclient.devices.Device;
import net.zscript.javaclient.nodes.ZscriptNode;
import net.zscript.model.ZscriptModel;
import net.zscript.model.modules.base.CoreModule;
import net.zscript.model.modules.base.PinsModule;

public class Main {
    public static void main(String[] args) throws IOException, InterruptedException {
        System.out.println(Arrays.toString(SerialPort.getCommPorts()));
        SerialPort serialPort = SerialPort.getCommPorts()[1];

        Device device = new Device(ZscriptModel.standardModel(), new ZscriptNode(new SerialConnection(serialPort), 128));
        Thread.sleep(2000);
        device.send(CoreModule.activateBuilder().build(), r -> {
        });
        Thread.sleep(100);
        MorseTransmitter transmitter = new MorseTransmitter(device, 100, 13);
        MorseTranslator  translator  = new MorseTranslator();
        transmitter.transmit(translator.translate("Hello"));
        Thread.sleep(100);
        serialPort.closePort();
    }
}
