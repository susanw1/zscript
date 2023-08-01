package net.zscript.javasimulator.connections.pin;

import net.zscript.javasimulator.BlankCommunicationResponse;
import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;

public class PinWritePacket implements CommunicationPacket<PinConnection> {

    private final double level;
    private final double pullStrength;

    public static PinWritePacket digitalDrive5V(int level) {
        return new PinWritePacket(level != 0 ? 5 : 0, 100); // 10mOhm
    }

    public static PinWritePacket digitalDrive5V(boolean level) {
        return new PinWritePacket(level ? 5 : 0, 100); // 10mOhm
    }

    public static PinWritePacket digitalDrive3V3(int level) {
        return new PinWritePacket(level != 0 ? 3.3 : 0, 100); // 10mOhm
    }

    public static PinWritePacket digitalDrive3V3(boolean level) {
        return new PinWritePacket(level ? 3.3 : 0, 100); // 10mOhm
    }

    public static PinWritePacket digitalDrive5V(int level, double resistance) {
        return new PinWritePacket(level != 0 ? 5 : 0, 1 / Math.max(resistance, 0.001));
    }

    public static PinWritePacket digitalDrive5V(boolean level, double resistance) {
        return new PinWritePacket(level ? 5 : 0, 1 / Math.max(resistance, 0.001));
    }

    public static PinWritePacket digitalDrive3V3(int level, double resistance) {
        return new PinWritePacket(level != 0 ? 3.3 : 0, 1 / Math.max(resistance, 0.001));
    }

    public static PinWritePacket digitalDrive3V3(boolean level, double resistance) {
        return new PinWritePacket(level ? 3.3 : 0, 1 / Math.max(resistance, 0.001));
    }

    public static PinWritePacket analogDrive5V(double fraction) {
        return new PinWritePacket(fraction * 5, 100);
    }

    public static PinWritePacket analogDrive5V(double fraction, double resistance) {
        return new PinWritePacket(fraction * 5, 1 / Math.max(resistance, 0.001));
    }

    public static PinWritePacket analogDrive3V3(double fraction) {
        return new PinWritePacket(fraction * 3.3, 100);
    }

    public static PinWritePacket analogDrive3V3(double fraction, double resistance) {
        return new PinWritePacket(fraction * 3.3, 1 / Math.max(resistance, 0.001));
    }

    public static PinWritePacket analogDriveV(double voltage) {
        return new PinWritePacket(voltage, 100);
    }

    public static PinWritePacket analogDriveV(double voltage, double resistance) {
        return new PinWritePacket(voltage, 1 / Math.max(resistance, 0.001));
    }

    public static PinWritePacket releasePin() {
        return new PinWritePacket(0, 0);
    }

    private PinWritePacket(double level, double pullStrength) {
        this.level = level;
        this.pullStrength = pullStrength;
    }

    @Override
    public Class<PinConnection> getProtocolConnectionType() {
        return PinConnection.class;
    }

    public double getLevel() {
        return level;
    }

    public double getPullStrength() {
        return pullStrength;
    }

    @Override
    public String prettyPrint() {
        if (pullStrength == 0) {
            return "High impedence";
        } else if (pullStrength >= 1) {
            return "Set pin to " + level + "V";
        } else {
            return "Pull pin to " + level + "V with strength " + Math.floor(1 / pullStrength) + "Ω";
        }
    }

    @Override
    public CommunicationResponse<PinConnection> generateResponse(ResponseInfo info) {
        return new BlankCommunicationResponse<>(PinConnection.class);
    }

}
