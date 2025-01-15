package net.zscript.demo.morse;

import java.util.Arrays;
import java.util.stream.Collectors;

import com.fazecast.jSerialComm.SerialPort;

import net.zscript.javaclient.connectors.serial.SerialConnection;
import net.zscript.javaclient.devices.ZscriptDevice;
import net.zscript.javaclient.nodes.ZscriptNode;
import net.zscript.model.ZscriptModel;
import net.zscript.model.modules.base.CoreModule;

public class MorseSimple {
    public static void main(String[] args) throws Exception {
        System.out.println(Arrays.stream(SerialPort.getCommPorts()).map(SerialPort::getSystemPortName).collect(Collectors.toList()));
        SerialPort rxPort = SerialPort.getCommPorts()[1];
        // SerialPort txPort = SerialPort.getCommPorts()[2];

        rxPort.setBaudRate(9600);
        //txPort.setBaudRate(9600);

        ZscriptDevice rxDevice = new ZscriptDevice(ZscriptModel.standardModel(), ZscriptNode.newNode(new SerialConnection(rxPort)));
        ZscriptDevice txDevice = rxDevice; // new Device(ZscriptModel.standardModel(), new ZscriptNode(new SerialConnection(txPort))); // uncomment along with txPort to use 2 targets

        Thread.sleep(2000); // give the ports time to start up

        rxDevice.sendAndWaitExpectSuccess(CoreModule.activateBuilder().build());
        txDevice.sendAndWaitExpectSuccess(CoreModule.activateBuilder().build()); // it's fine to run this twice...

        int ditPeriodUs = 150 * 1000;

        MorseTranslator  translator  = new MorseTranslator();
        MorseReceiver    receiver    = new MorseReceiver(translator, rxDevice, ditPeriodUs, 2);
        MorseTransmitter transmitter = new MorseTransmitter(txDevice, ditPeriodUs, 13);
        transmitter.start();
        receiver.startReceiving();
        transmitter.transmit(translator.translate("Hello world."));

        receiver.close();
        transmitter.close();
        rxPort.closePort();
        // txPort.closePort();
    }
}
