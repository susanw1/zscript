package net.zscript.demo.morse;

import java.io.File;
import java.io.FileInputStream;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Scanner;
import java.util.Set;
import java.util.concurrent.Callable;
import java.util.function.Predicate;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortInvalidPortException;
import picocli.CommandLine;
import picocli.CommandLine.Command;
import picocli.CommandLine.Option;
import picocli.CommandLine.Parameters;

import net.zscript.javaclient.commandbuilder.commandnodes.CommandSequenceNode;
import net.zscript.javaclient.commandbuilder.commandnodes.ResponseCaptor;
import net.zscript.javaclient.commandbuilder.defaultCommands.BlankCommandNode;
import net.zscript.javaclient.connectors.serial.SerialConnection;
import net.zscript.javaclient.devices.Device;
import net.zscript.javaclient.devices.ResponseSequenceCallback;
import net.zscript.javaclient.nodes.ZscriptNode;
import net.zscript.model.ZscriptModel;
import net.zscript.model.modules.base.CoreModule;
import net.zscript.model.modules.base.PinsModule;
import net.zscript.model.modules.base.PinsModule.DigitalSetupCommand.DigitalSetupResponse;
import net.zscript.model.modules.base.UartModule;

@Command(mixinStandardHelpOptions = true, version = "Morse code Zscript demo 0.1")
public class MorseFullCli implements Callable<Integer> {

    @Option(names = { "-r", "--receive-port" }, arity = "0..1", fallbackValue = "__ManualSelect__", defaultValue = "__NoReceive__",
            description = "The port to receive data along. No reception done if not present. If specified without parameter, gives list of options.")
    private String receivePort;
    @Option(names = { "-rb", "--receive-baud-rate" }, arity = "0..1", defaultValue = "-1",
            description = "Overrides \"--baud-rate\" The initial baud rate of the receiving port. Default: 9600")
    private int    receiveBaud;
    @Option(names = { "-rbn", "--receive-baud-rate-negotiated" }, arity = "0..1", fallbackValue = "-2", defaultValue = "-1",
            description = "Overrides \"--baud-rate-negotiated\" The target baud rate to negotiate to on the receiving port, if possible. Leave blank to read options from device. Default: no renegotiation of baud rate")
    private int    receiveBaudNegotiate;
    @Option(names = { "-rp", "--receive-pin" }, arity = "0..1", defaultValue = "-1",
            description = "Sets the pin on which the receiver will listen. Default: interactive setting of pin")
    private int    receivePin;

    @Option(names = { "-t", "--transmit-port" }, arity = "0..1", fallbackValue = "__ManualSelect__", defaultValue = "__NoTransmit__",
            description = "The port to transmit data along. No transmission done if not present. If specified without parameter, gives list of options.")
    private String transmitPort;
    @Option(names = { "-tb", "--transmit-baud-rate" }, arity = "0..1", defaultValue = "-1",
            description = "Overrides \"--baud-rate\" The initial baud rate of the transmitting port. Default: 9600")
    private int    transmitBaud;
    @Option(names = { "-tbn", "--transmit-baud-rate-negotiated" }, arity = "0..1", fallbackValue = "-2", defaultValue = "-1",
            description = "Overrides \"--baud-rate-negotiated\" The target baud rate to negotiate to on the transmitting port, if possible. Leave blank to read options from device. Default: no renegotiation of baud rate")
    private int    transmitBaudNegotiate;
    @Option(names = { "-tp", "--transmit-pin" }, arity = "0..1", defaultValue = "-1",
            description = "Sets the pin on which the transmitter will send. Default: interactive setting of pin")
    private int    transmitPin;

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

    private int negotiateBaud(SerialPort port, Device device, int target, String name) throws InterruptedException {
        if (target > 16000000) {
            System.err.println("Cannot negotiate to frequencies over 16MHz");
            return 1;
        } else if (target < -2) {
            System.err.println("Invalid frequency selection");
            return 1;
        }
        if (target != -1) {
            // Find the channel index for the current channel
            ResponseCaptor<CoreModule.ChannelInfoCommand.ChannelInfoResponse> infoResp = ResponseCaptor.create();
            int currentChannelIndex = device.sendAndWaitExpectSuccess(CoreModule.channelInfoBuilder()
                            .capture(infoResp)
                            .build())
                    .getResponseFor(infoResp).orElseThrow()
                    .getCurrentChannel();

            // get the detailed info on the current channel
            ResponseCaptor<UartModule.ChannelInfoCommand.ChannelInfoResponse> uartInfo = ResponseCaptor.create();
            UartModule.ChannelInfoCommand.ChannelInfoResponse resp = device.sendAndWaitExpectSuccess(UartModule.channelInfoBuilder()
                            .setChannel(currentChannelIndex)
                            .capture(uartInfo)
                            .build())
                    .getResponseFor(uartInfo).orElseThrow();

            if (resp.getBitsetCapabilities().contains(UartModule.ChannelInfoCommand.ChannelInfoResponse.BitsetCapabilities.ArbitraryFrequency)) {
                // if the channel supports free baud rate setting, use that to set the desired baud rate
                return setBaudArbitrary(port, device, target, name, resp, currentChannelIndex);
            } else if (resp.hasFrequenciesSupported()) {
                if (target == -2) {
                    // present a menu of frequencies by iterating the frequency menu
                    return setBaudInteractiveMenu(port, device, name, resp, currentChannelIndex);
                } else {
                    // search the frequency options
                    return setBaudSearchMenu(port, device, target, name, resp, currentChannelIndex);
                }
            } else {
                System.err.println(name + " does not support frequency selection");
                return 1;
            }
        }
        return 0;
    }

    private int setBaudSearchMenu(SerialPort port, Device device, int target, String name, UartModule.ChannelInfoCommand.ChannelInfoResponse resp, int currentChannelIndex)
            throws InterruptedException {
        // find all the frequency options
        List<ResponseCaptor<UartModule.ChannelInfoCommand.ChannelInfoResponse>> captors = new ArrayList<>();

        CommandSequenceNode node     = new BlankCommandNode();
        boolean             hasFound = false;
        for (int i = 0; i < resp.getFrequenciesSupported().orElseThrow(); i++) {
            ResponseCaptor<UartModule.ChannelInfoCommand.ChannelInfoResponse> captor = ResponseCaptor.create();
            node = node.andThen(UartModule.channelInfoBuilder()
                    .setChannel(currentChannelIndex)
                    .setFrequencySelection(i).capture(captor)
                    .build());
            captors.add(captor);
        }

        ResponseSequenceCallback result = device.sendAndWaitExpectSuccess(node);
        // iterate the frequency options looking for an exact match
        for (int i = 0; i < resp.getFrequenciesSupported().orElseThrow(); i++) {
            UartModule.ChannelInfoCommand.ChannelInfoResponse respEach = result.getResponseFor(captors.get(i)).orElseThrow();

            int baudInt = parseBaud(respEach.getBaudRate());
            if (baudInt == target) {
                hasFound = true;
                device.sendAndWaitExpectSuccess(UartModule.channelSetupBuilder()
                        .setChannel(currentChannelIndex)
                        .setFrequencySelection(i)
                        .build());
                port.setBaudRate(target);
                break;
            }
        }
        if (!hasFound) {
            System.err.println(name + " does not support target frequency");
            return 1;
        }
        return 0;
    }

    private int setBaudInteractiveMenu(SerialPort port, Device device, String name, UartModule.ChannelInfoCommand.ChannelInfoResponse resp, int currentChannelIndex)
            throws InterruptedException {
        System.out.println(name + " available frequencies: ");

        List<ResponseCaptor<UartModule.ChannelInfoCommand.ChannelInfoResponse>> captors = new ArrayList<>();

        CommandSequenceNode node = new BlankCommandNode();
        // send all the get commands in one batch, for fun
        for (int i = 0; i < resp.getFrequenciesSupported().orElseThrow(); i++) {
            ResponseCaptor<UartModule.ChannelInfoCommand.ChannelInfoResponse> captor = ResponseCaptor.create();
            node = node.andThen(UartModule.channelInfoBuilder()
                    .setChannel(currentChannelIndex)
                    .setFrequencySelection(i)
                    .capture(captor)
                    .build());
            captors.add(captor);
        }
        int[] freqChoices = new int[resp.getFrequenciesSupported().orElseThrow()];

        ResponseSequenceCallback result = device.sendAndWaitExpectSuccess(node);
        for (int i = 0; i < resp.getFrequenciesSupported().orElseThrow(); i++) {
            UartModule.ChannelInfoCommand.ChannelInfoResponse respEach = result.getResponseFor(captors.get(i)).orElseThrow();

            int baudInt = parseBaud(respEach.getBaudRate());
            freqChoices[i] = baudInt;
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
        // apply chosen frequency
        device.sendAndWaitExpectSuccess(UartModule.channelSetupBuilder()
                .setChannel(currentChannelIndex)
                .setFrequencySelection(choice)
                .build());

        port.setBaudRate(freqChoices[choice]);
        return 0;
    }

    private int setBaudArbitrary(SerialPort port, Device device, int target, String name, UartModule.ChannelInfoCommand.ChannelInfoResponse resp, int currentChannelIndex)
            throws InterruptedException {
        int baudMax      = parseBaud(resp.getBaudRate());
        int actualTarget = target;
        if (target == -2) {
            System.out.println("Baud rates supported: 0-" + baudMax);
            System.out.print("Please choose a baud rate: ");
            actualTarget = getInputNumber(baudMax);
            System.out.println();
            if (actualTarget == -1) {
                System.err.println("Baud rate out of range, aborting.");
                return 1;
            }
        }
        if (baudMax < actualTarget) {
            System.err.println(name + " does not support negotiating to frequency given");
            return 1;
        }
        byte[] baud = new byte[] { (byte) (actualTarget >> 16), (byte) (actualTarget >> 8), (byte) actualTarget };
        device.sendAndWaitExpectSuccess(UartModule.channelSetupBuilder()
                .setChannel(currentChannelIndex)
                .setBaudRate(baud)
                .build());
        port.setBaudRate(actualTarget);
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

    private int parsePin(Device device, String name, String verb, Predicate<DigitalSetupResponse> requirements, int toIgnore, String requirementName)
            throws InterruptedException {
        ResponseCaptor<PinsModule.CapabilitiesCommand.CapabilitiesResponse> captor = ResponseCaptor.create();

        int pinCount = device.sendAndWaitExpectSuccess(PinsModule.capabilitiesBuilder()
                        .capture(captor)
                        .build())
                .getResponseFor(captor)
                .orElseThrow().getPinCount();

        System.out.println(name + " has " + pinCount + " pins");
        Set<Integer> pinsSupporting = new HashSet<>();
        // assemble a big command sequence to check pins in batches...
        final int batchSize = 8;
        for (int batchStart = 0; batchStart < pinCount; batchStart += batchSize) {
            CommandSequenceNode                        sequence = new BlankCommandNode();
            List<ResponseCaptor<DigitalSetupResponse>> captors  = new ArrayList<>();
            for (int i = batchStart; i < pinCount && i < batchStart + batchSize; i++) {
                if (i == toIgnore) {
                    captors.add(ResponseCaptor.create());
                    continue;
                }
                ResponseCaptor<DigitalSetupResponse> digitalCaptor = ResponseCaptor.create();
                sequence = sequence.andThen(PinsModule.digitalSetupBuilder()
                        .setPin(i).capture(digitalCaptor)
                        .build());
                captors.add(digitalCaptor);
            }
            ResponseSequenceCallback resp = device.sendAndWaitExpectSuccess(sequence);
            // check each pin to see if it supports required functions
            for (int i = 0; i < captors.size(); i++) {
                if (i + batchStart == toIgnore) {
                    continue;
                }
                DigitalSetupResponse pinResp = resp.getResponseFor(captors.get(i)).orElseThrow();
                if (requirements.test(pinResp)) {
                    pinsSupporting.add(i + batchStart);
                }
            }
        }
        if (pinsSupporting.isEmpty()) {
            System.err.println("Device has no pins supporting " + requirementName + ", so cannot receive morse");
            return -1;
        }
        System.out.print(verb + " is supported on pins: ");
        int     prevStart = -2;
        int     prevEnd   = -2;
        boolean isFirst   = true;
        for (int i = 0; i < pinCount; i++) {
            if (pinsSupporting.contains(i)) {
                if (prevEnd == i - 1) {
                    prevEnd++;
                } else {
                    prevStart = i;
                    prevEnd = i;
                }
            } else if (prevEnd == i - 1) {
                if (!isFirst) {
                    System.out.print(", ");
                }
                isFirst = false;
                if (prevStart == prevEnd) {
                    System.out.print(prevStart);
                } else {
                    System.out.print(prevStart + "-" + prevEnd);
                }
                prevStart = -2;
                prevEnd = -2;
            }
        }
        if (prevStart != -2) {
            if (!isFirst) {
                System.out.print(", ");
            }
            if (prevStart == prevEnd) {
                System.out.print(prevStart);
            } else {
                System.out.print(prevStart + "-" + prevEnd);
            }
        }
        System.out.println();

        System.out.print("Please select a pin for " + verb.toLowerCase() + ": ");
        int chosen = getInputNumber(pinCount);
        System.out.println();
        if (!pinsSupporting.contains(chosen)) {
            System.err.println("Chosen pin does not support " + requirementName + ", so cannot receive morse");
            return -1;
        }
        return chosen;

    }

    @Override
    public Integer call() throws Exception {
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
        if (rx == null && tx == null) {
            System.err.println("No operation selected.");
            return 1;
        }
        if (rx == null && (receiveBaud != -1 || receiveBaudNegotiate != -1 || receivePin != -1)) {
            System.err.println("Cannot set frequency on receiving port without receive device");
            return 1;
        }
        if (tx == null && (transmitBaud != -1 || transmitBaudNegotiate != -1 || transmitPin != -1)) {
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
            rxDevice = new Device(ZscriptModel.standardModel(), ZscriptNode.newNode(new SerialConnection(rx)));
            Thread.sleep(2000);
            rxDevice.sendAndWaitExpectSuccess(CoreModule.activateBuilder().build());
            if (negotiateBaud(rx, rxDevice, receiveBaudNegotiate == -1 ? generalBaudNegotiate : receiveBaudNegotiate, "Receive device") != 0) {
                rx.closePort();
                return 1;
            }
            if (receivePin == -1) {
                // ask user to specify receive pin
                receivePin = parsePin(rxDevice, "Receive", "Receiving", d -> d.hasSupportedNotifications() && d.getSupportedNotifications().contains(
                        DigitalSetupResponse.SupportedNotifications.OnChange) && d.getSupportedModes().contains(
                        DigitalSetupResponse.SupportedModes.Input), -1, "OnChange notifications");
                if (receivePin == -1) {
                    rx.closePort();
                    return 1;
                }
            }
        }
        Device txDevice = null;
        if (tx != null) {
            if (rx != null && tx.getSystemPortName().equals(rx.getSystemPortName())) {
                txDevice = rxDevice;
            } else {
                tx.setBaudRate(transmitBaud == -1 ? generalBaud : transmitBaud);
                txDevice = new Device(ZscriptModel.standardModel(), ZscriptNode.newNode(new SerialConnection(tx)));
                Thread.sleep(2000);
                txDevice.sendAndWaitExpectSuccess(CoreModule.activateBuilder().build());
                if (negotiateBaud(tx, txDevice, transmitBaudNegotiate == -1 ? generalBaudNegotiate : transmitBaudNegotiate, "Transmit device") != 0) {
                    tx.closePort();
                    if (rx != null) {
                        rx.closePort();
                    }
                    return 1;
                }
            }
            if (transmitPin == -1) {
                // ask user to specify receive pin
                boolean txRxSamePinAllowed = txDevice != rxDevice;
                transmitPin = parsePin(txDevice, "Transmit", "Transmitting", d -> d.getSupportedModes().contains(
                        DigitalSetupResponse.SupportedModes.Output), txRxSamePinAllowed ? -1 : receivePin, "Output");
                if (transmitPin == -1) {
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
            receiver = new MorseReceiver(translator, rxDevice, ditPeriod * 1000L, 2);
            receiver.startReceiving();
        }
        if (txDevice != null) {
            MorseTransmitter transmitter = new MorseTransmitter(txDevice, ditPeriod * 1000L, 13);
            transmitter.start();
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
            transmitter.close();
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

    public static void main(String[] args) {
        int exitCode = new CommandLine(new MorseFullCli()).execute(args);
        System.exit(exitCode);
    }
}
