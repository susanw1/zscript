package net.zscript.demo.morse;

import com.fazecast.jSerialComm.SerialPort;

import java.io.IOException;
import java.util.Arrays;

import net.zscript.javaclient.connectors.serial.SerialConnection;
import net.zscript.javaclient.devices.Device;
import net.zscript.javaclient.nodes.ZscriptNode;
import net.zscript.model.ZscriptModel;
import net.zscript.model.modules.base.CoreModule;

public class Main {
    public static void main(String[] args) throws IOException, InterruptedException {
        System.out.println(Arrays.toString(SerialPort.getCommPorts()));
        SerialPort serialPort1 = SerialPort.getCommPorts()[1];
        serialPort1.setBaudRate(115200);
        // SerialPort serialPort2 = SerialPort.getCommPorts()[2];
        // serialPort2.setBaudRate(115200);

        long   ditPeriodUs = 5000;
        Device device1     = new Device(ZscriptModel.standardModel(), new ZscriptNode(new SerialConnection(serialPort1), 128));
        Device device2     = device1; // new Device(ZscriptModel.standardModel(), new ZscriptNode(new SerialConnection(serialPort2), 128));
        Thread.sleep(2000);
        device1.sendAndWaitExpectSuccess(CoreModule.activateBuilder().build());
        MorseTranslator  translator  = new MorseTranslator();
        MorseTransmitter transmitter = new MorseTransmitter(device1, ditPeriodUs, 13);
        MorseReceiver    receiver    = new MorseReceiver(translator, device2, ditPeriodUs, 2);
        receiver.startReceiving();
        transmitter.transmit(translator.translate(""));
        receiver.close();
        serialPort1.closePort();
        // serialPort2.closePort();
    }
}
