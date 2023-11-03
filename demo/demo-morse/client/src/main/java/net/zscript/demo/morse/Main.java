package net.zscript.demo.morse;

import com.fazecast.jSerialComm.SerialPort;

import java.io.IOException;
import java.util.Arrays;

import net.zscript.javaclient.connectors.serial.SerialConnection;
import net.zscript.javaclient.devices.CommandFailedException;
import net.zscript.javaclient.devices.Device;
import net.zscript.javaclient.devices.ResponseSequenceCallback;
import net.zscript.javaclient.nodes.ZscriptNode;
import net.zscript.model.ZscriptModel;
import net.zscript.model.modules.base.CoreModule;

public class Main {
    public static void main(String[] args) throws IOException, InterruptedException {
        System.out.println(Arrays.toString(SerialPort.getCommPorts()));
        SerialPort serialPort1 = SerialPort.getCommPorts()[1];
        SerialPort serialPort2 = SerialPort.getCommPorts()[2];

        int ditPeriod = 20;
        Device device1 = new Device(ZscriptModel.standardModel(), new ZscriptNode(new SerialConnection(serialPort1), 128));
        Device device2 = new Device(ZscriptModel.standardModel(), new ZscriptNode(new SerialConnection(serialPort2), 128));
        Thread.sleep(2000);
        device1.sendAndWaitExpectSuccess(CoreModule.activateBuilder().build());
        MorseTransmitter transmitter = new MorseTransmitter(device1, ditPeriod, 13);
        MorseReceiver    receiver    = new MorseReceiver(device2, ditPeriod, 2);
        receiver.startReceiving();
        MorseTranslator translator = new MorseTranslator();
        transmitter.transmit(translator.translate("Hello how are you doing"));
        serialPort1.closePort();
        serialPort2.closePort();
    }
}
