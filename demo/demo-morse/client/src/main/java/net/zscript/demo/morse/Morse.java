package net.zscript.demo.morse;

import com.fazecast.jSerialComm.SerialPort;

import com.fazecast.jSerialComm.SerialPortInvalidPortException;
import picocli.CommandLine;
import picocli.CommandLine.Command;
import picocli.CommandLine.Option;
import picocli.CommandLine.Parameters;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.Scanner;
import java.util.concurrent.Callable;

import net.zscript.javaclient.commandbuilder.commandnodes.ResponseCaptor;
import net.zscript.javaclient.connectors.serial.SerialConnection;
import net.zscript.javaclient.devices.Device;
import net.zscript.javaclient.nodes.ZscriptNode;
import net.zscript.model.ZscriptModel;
import net.zscript.model.modules.base.CoreModule;
import net.zscript.model.modules.base.UartModule;

@Command(mixinStandardHelpOptions = true, version = "Morse code Zscript demo 0.1")
public class Morse implements Callable<Integer> {

    @Option(names = { "-r", "--receive-port" }, arity = "0..1", fallbackValue = "__ManualSelect__", defaultValue = "__NoReceive__",
            description = "The port to receive data along. No reception done if not present. If specified without parameter, gives list of options.")
    private String receivePort;
    @Option(names = { "-rb", "--receive-baud-rate" }, arity = "0..1", defaultValue = "-1",
            description = "Overrides \"--baud-rate\" The initial baud rate of the receiving port. Default: 9600")
    private int    receiveBaud;
    @Option(names = { "-rbn", "--receive-baud-rate-negotiated" }, arity = "0..1", fallbackValue = "-2", defaultValue = "-1",
            description = "Overrides \"--baud-rate-negotiated\" The target baud rate to negotiate to on the receiving port, if possible. Leave blank to read options from device. Default: no renegotiation of baud rate")
    private int    receiveBaudNegotiate;

    @Option(names = { "-t", "--transmit-port" }, arity = "0..1", fallbackValue = "__ManualSelect__", defaultValue = "__NoTransmit__",
            description = "The port to transmit data along. No transmission done if not present. If specified without parameter, gives list of options.")
    private String transmitPort;
    @Option(names = { "-tb", "--transmit-baud-rate" }, arity = "0..1", defaultValue = "-1",
            description = "Overrides \"--baud-rate\" The initial baud rate of the transmitting port. Default: 9600")
    private int    transmitBaud;
    @Option(names = { "-tbn", "--transmit-baud-rate-negotiated" }, arity = "0..1", fallbackValue = "-2", defaultValue = "-1",
            description = "Overrides \"--baud-rate-negotiated\" The target baud rate to negotiate to on the transmitting port, if possible. Leave blank to read options from device. Default: no renegotiation of baud rate")
    private int    transmitBaudNegotiate;

    @Option(names = { "-b", "--baud-rate" }, arity = "0..1", defaultValue = "9600",
            description = "The initial baud rate of the ports. Default: 9600")
    private int generalBaud;
    @Option(names = { "-bn", "--baud-rate-negotiated" }, arity = "0..1", fallbackValue = "-2", defaultValue = "-1",
            description = "The target baud rate to negotiate to on the ports, if possible. Leave blank to read options from device. Default: no renegotiation of baud rate")
    private int generalBaudNegotiate;

    @Option(names = { "-d", "--dit-period" }, required = true,
            description = "The length of a dit in ms")
    private int ditPeriod;

    @Option(names = { "-m", "--message" }, arity = "0..1",
            description = "Message to be transmitted")
    private String message;
    @Parameters(index = "0..*")
    private File[] data;

    private final Scanner stdInScanner = new Scanner(System.in, StandardCharsets.UTF_8);

    private int getInputNumber(int max) {
        int choice = -1;
        try {
            String in = stdInScanner.nextLine();
            choice = Integer.parseInt(in);
            if (choice < 0 || choice >= max) {
                choice = -1;
            }
        } catch (NumberFormatException ignored) {
        }
        return choice;
    }

    private SerialPort selectPortOptions(String name) {
        System.out.println("Available ports:");
        SerialPort[] options = SerialPort.getCommPorts();
        for (int i = 0; i < options.length; i++) {
            System.out.printf("%2d: %s (%s)\n", i, options[i].getDescriptivePortName(), options[i].getSystemPortName());
        }
        System.out.print("Please select index of " + name + " port: ");
        int choice = getInputNumber(options.length);
        System.out.println();
        if (choice == -1) {
            System.err.println("Invalid port choice, aborting");
            return null;
        }
        return options[choice];
    }

    private SerialPort selectPort(String port, String operationName) {
        SerialPort selected;
        if (port.equals("__ManualSelect__")) {
            selected = selectPortOptions(operationName);
            if (selected == null) {
                return null;
            }
        } else {
            try {
                selected = SerialPort.getCommPort(port);
            } catch (SerialPortInvalidPortException e) {
                System.err.println("No port found with name: " + port);
                return null;
            }
        }
        return selected;
    }

    private int negotiateBaud(Device device, int target, String name) throws InterruptedException {
        if (target > 16000000) {
            System.err.println("Cannot negotiate to frequencies over 16MHz");
            return 1;
        } else if (target < -2) {
            System.err.println("Invalid frequency selection");
            return 1;
        }
        if (target != -1) {
            ResponseCaptor<CoreModule.ChannelInfoCommand.ChannelInfoResponse> infoResp = ResponseCaptor.create();
            int currentChannelIndex = device.sendAndWaitExpectSuccess(
                    CoreModule.channelInfoBuilder().capture(infoResp).build()).getResponseFor(infoResp).orElseThrow().getCurrentChannel();
            System.out.println(currentChannelIndex);
            ResponseCaptor<UartModule.ChannelInfoCommand.ChannelInfoResponse> uartInfo = ResponseCaptor.create();
            UartModule.ChannelInfoCommand.ChannelInfoResponse resp = device.sendAndWaitExpectSuccess(
                    UartModule.channelInfoBuilder().setChannel(currentChannelIndex).capture(uartInfo).build()).getResponseFor(uartInfo).orElseThrow();
            System.out.println("aaa");
            if (resp.getBitsetCapabilities().contains(UartModule.ChannelInfoCommand.ChannelInfoResponse.BitsetCapabilities.ArbitraryFrequency)) {
                int baudInt = parseBaud(resp.getBaudRate());
                if (baudInt < target) {
                    System.err.println(name + " does not support negotiating to frequency given");
                    return 1;
                }
                byte[] baud = new byte[] { (byte) (target >> 16), (byte) (target >> 8), (byte) target };
                device.sendAndWaitExpectSuccess(UartModule.channelSetupBuilder().setChannel(currentChannelIndex).setBaudRate(baud).build());
            } else if (resp.hasFrequenciesSupported()) {
                if (target == -2) {
                    System.out.println(name + " available frequencies: ");
                    for (int i = 0; i < resp.getFrequenciesSupported().orElseThrow(); i++) {
                        ResponseCaptor<UartModule.ChannelInfoCommand.ChannelInfoResponse> uartInfoEach = ResponseCaptor.create();
                        UartModule.ChannelInfoCommand.ChannelInfoResponse respEach = device.sendAndWaitExpectSuccess(
                                        UartModule.channelInfoBuilder().setChannel(currentChannelIndex).setFrequencySelection(i).capture(uartInfoEach).build()).getResponseFor(uartInfoEach)
                                .orElseThrow();
                        int    baudInt      = parseBaud(respEach.getBaudRate());
                        int    baudIntHuman = baudInt;
                        String unit         = "Hz";
                        if (baudIntHuman > 999999) {
                            baudIntHuman /= 1000000;
                            unit = "MHz";
                        } else if (baudIntHuman > 999) {
                            baudIntHuman /= 1000;
                            unit = "kHz";
                        }
                        System.out.printf("%d3: %d8 (%d%s)\n", i, baudInt, baudIntHuman, unit);
                    }
                    System.out.print("Please select frequency index: ");
                    int choice = getInputNumber(resp.getFrequenciesSupported().orElseThrow());
                    System.out.println();
                    if (choice == -1) {
                        System.err.println("Invalid frequency choice, aborting");
                        return 1;
                    }
                    device.sendAndWaitExpectSuccess(UartModule.channelSetupBuilder().setChannel(currentChannelIndex).setFrequency(choice).build());
                }
            } else {
                System.err.println(name + "Receive device does not support frequency selection");
                return 1;
            }
        }
        return 0;
    }

    private int parseBaud(byte[] baud) {
        if (baud.length > 3) {
            return 16000000;
        }
        int baudInt = 0;
        for (byte b : baud) {
            baudInt <<= 8;
            baudInt += b;
        }
        return baudInt;
    }

    @Override
    public Integer call() throws Exception {
        System.out.println(Arrays.toString(SerialPort.getCommPorts()));
        SerialPort rx = null;
        if (!receivePort.equals("__NoReceive__")) {
            rx = selectPort(receivePort, "receiving");
            if (rx == null) {
                return 1;
            }
        }
        SerialPort tx = null;
        if (!transmitPort.equals("__NoTransmit__")) {
            tx = selectPort(transmitPort, "transmitting");
            if (tx == null) {
                return 1;
            }
        }
        if (ditPeriod > 100000) {
            System.err.println("Dit periods over 100s not supported (and probably not desired)");
            return 1;
        } else if (ditPeriod <= 0) {
            System.err.println("Dit period must be positive");
            return 1;
        } else if (ditPeriod <= 2) {
            System.err.println("Dit period too short to transmit accurately");
            return 1;
        }
        if (rx == null && tx == null) {
            System.err.println("No operation selected.");
            return 1;
        }
        if (rx == null && (receiveBaud != -1 || receiveBaudNegotiate != -1)) {
            System.err.println("Cannot set frequency on receiving port without receive device");
            return 1;
        }
        if (tx == null && (transmitBaud != -1 || transmitBaudNegotiate != -1)) {
            System.err.println("Cannot set frequency on transmitting port without transmit device");
            return 1;
        }
        if (tx == null && (message != null || data != null)) {
            System.err.println("Cannot transmit without transmitting port");
            return 1;
        }
        if (tx != null && rx != null && tx.getSystemPortName().equals(rx.getSystemPortName())) {
            if (receiveBaud != -1 || receiveBaudNegotiate != -1 || transmitBaud != -1 || transmitBaudNegotiate != -1) {
                System.err.println("Cannot set independent baud rates when transmit and receive devices are the same");
                return 1;
            }
        }
        Device rxDevice = null;
        if (rx != null) {
            rx.setBaudRate(receiveBaud == -1 ? generalBaud : receiveBaud);
            rxDevice = new Device(ZscriptModel.standardModel(), new ZscriptNode(new SerialConnection(rx), 128));
            Thread.sleep(2000);
            rxDevice.sendAndWaitExpectSuccess(CoreModule.activateBuilder().build());
            if (negotiateBaud(rxDevice, receiveBaudNegotiate == -1 ? generalBaudNegotiate : receiveBaudNegotiate, "Receive device") != 0) {
                rx.closePort();
                return 1;
            }
        }
        Device txDevice = null;
        if (tx != null) {
            if (rx != null && tx.getSystemPortName().equals(rx.getSystemPortName())) {
                txDevice = rxDevice;
            } else {
                tx.setBaudRate(transmitBaud == -1 ? generalBaud : transmitBaud);
                txDevice = new Device(ZscriptModel.standardModel(), new ZscriptNode(new SerialConnection(tx), 128));
                Thread.sleep(2000);
                txDevice.sendAndWaitExpectSuccess(CoreModule.activateBuilder().build());
                if (negotiateBaud(txDevice, transmitBaudNegotiate == -1 ? generalBaudNegotiate : transmitBaudNegotiate, "Transmit device") != 0) {
                    tx.closePort();
                    if (rx != null) {
                        rx.closePort();
                    }
                    return 1;
                }
            }
        }

        MorseTranslator translator = new MorseTranslator();
        MorseReceiver   receiver   = null;
        if (rxDevice != null) {
            receiver = new MorseReceiver(translator, rxDevice, ditPeriod * 1000, 2);
            receiver.startReceiving();
        }
        if (txDevice != null) {
            MorseTransmitter transmitter = new MorseTransmitter(txDevice, ditPeriod * 1000, 13);
            if (message != null) {
                transmitter.transmit(translator.translate(message));
            }
            if (data != null) {
                for (File file : data) {
                    FileInputStream in = new FileInputStream(file);
                    transmitter.transmit(translator.translate(StandardCharsets.UTF_8.decode(ByteBuffer.wrap(in.readAllBytes())).toString()));
                    in.close();
                }
            }

            if (message == null && data == null) {
                while (true) {
                    transmitter.transmit(translator.translate(stdInScanner.nextLine()));
                }

            }
        }
        if (receiver != null) {
            receiver.close();
        }
        if (rx != null) {
            rx.closePort();
        }
        if (tx != null) {
            tx.closePort();
        }
        return 0;
    }

    public static void main(String[] args) throws IOException, InterruptedException {
        String[] demoArgs = new String[] { "-r", "-t", "-b", "115200", "-d", "100" };
        int      exitCode = new CommandLine(new Morse()).execute(demoArgs);
        System.exit(exitCode);
    }
}
