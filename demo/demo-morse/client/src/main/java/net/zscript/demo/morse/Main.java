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
        SerialPort serialPort = SerialPort.getCommPorts()[1];

        Device device = new Device(ZscriptModel.standardModel(), new ZscriptNode(new SerialConnection(serialPort), 128));
        Thread.sleep(2000);
        device.sendAndWaitExpectSuccess(CoreModule.activateBuilder().build());
        MorseTransmitter transmitter = new MorseTransmitter(device, 150, 13);
        MorseTranslator  translator  = new MorseTranslator();
        transmitter.transmit(translator.translate("Best bent wire best ben"));
        serialPort.closePort();
    }
}
