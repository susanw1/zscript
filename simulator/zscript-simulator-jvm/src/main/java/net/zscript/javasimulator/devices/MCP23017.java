package net.zscript.javasimulator.devices;

import java.util.ArrayList;
import java.util.List;

import net.zscript.javasimulator.BlankCommunicationResponse;
import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;
import net.zscript.javasimulator.Connection;
import net.zscript.javasimulator.EntityController;
import net.zscript.javasimulator.ProtocolCategory;
import net.zscript.javasimulator.ProtocolConnection;
import net.zscript.javasimulator.connections.i2c.I2cAddress;
import net.zscript.javasimulator.connections.i2c.I2cConnection;
import net.zscript.javasimulator.connections.i2c.I2cProtocolCategory;
import net.zscript.javasimulator.connections.i2c.I2cReceivePacket;
import net.zscript.javasimulator.connections.i2c.I2cReceiveResponse;
import net.zscript.javasimulator.connections.i2c.I2cSendPacket;
import net.zscript.javasimulator.connections.i2c.SmBusAlertConnection;
import net.zscript.javasimulator.connections.i2c.SmBusAlertPacket;
import net.zscript.javasimulator.connections.pin.PinConnection;
import net.zscript.javasimulator.connections.pin.PinProtocolCategory;
import net.zscript.javasimulator.connections.pin.PinReadPacket;
import net.zscript.javasimulator.connections.pin.PinStatePacket;
import net.zscript.javasimulator.connections.pin.PinWritePacket;

public class MCP23017 extends EntityController {

    private static final int IODIR = 0;
    private static final int IPOL = 1;
    private static final int GPINTEN = 2;
    private static final int DEFVAL = 3;
    private static final int INTCON = 4;
    private static final int IOCON = 5;
    private static final int GPPU = 6;
    private static final int INTF = 7;
    private static final int INTCAP = 8;
    private static final int GPIO = 9;
    private static final int OLAT = 10;

    private final Connection<I2cProtocolCategory> outCon;

    private final List<Connection<PinProtocolCategory>> iosA = new ArrayList<>();

    private final byte[] regsA = new byte[11]; // includes IOCON, but that space is ignored.

    private final List<Connection<PinProtocolCategory>> iosB = new ArrayList<>();
    private final byte[] regsB = new byte[11]; // includes IOCON, but that space is ignored.

    private int regAddress = 0;

    private final byte addressBits;

    private byte IoConState = 0x44; // fakes the setup we kinda use anyway...
    private boolean isInterrupt = false;

    public MCP23017(byte addressBits) {
        this.addressBits = addressBits;
        regsA[0] = (byte) 0xFF;
        regsB[0] = (byte) 0xFF;

        outCon = entity.connectNew(I2cProtocolCategory.class);
        I2cConnection con = new I2cConnection(outCon);
        con.connect(new I2cAddress((this.addressBits & 0x7) | 0x20), entity);
        outCon.addProtocol(con);
        SmBusAlertConnection smbus = new SmBusAlertConnection(outCon);
        outCon.addProtocol(smbus);
        for (int i = 0; i < 8; i++) {
            Connection<PinProtocolCategory> line = entity.connectNew(PinProtocolCategory.class);
            PinConnection pinCon = new PinConnection(line);
            pinCon.addChild(entity);
            line.addProtocol(pinCon);
            iosA.add(line);
        }
        for (int i = 0; i < 8; i++) {
            Connection<PinProtocolCategory> line = entity.connectNew(PinProtocolCategory.class);
            PinConnection pinCon = new PinConnection(line);
            pinCon.addChild(entity);
            line.addProtocol(pinCon);
            iosB.add(line);
        }
    }

    private void writeReg(byte[] regs, byte toWrite, int index) {
        if (index == GPIO) {
            regs[OLAT] = toWrite;
        } else if (index == IOCON) {
            IoConState = toWrite;
        } else if (index == INTF || index == INTCAP) {
            // Read only
        } else if (index < 11) {
            regs[index] = toWrite;
        }

    }

    private byte readReg(byte[] regs, int index) {
        if (index == GPIO || index == INTCAP) {
            isInterrupt = false;
            regs[INTF] = 0;
        }
        if (index == GPIO) {
            outCon.sendMessage(entity, new SmBusAlertPacket(false));
            readPins(regs, regs == regsA ? iosA : iosB);
            return regs[index];
        } else if (index == IOCON) {
            return (byte) (IoConState & 0xFE);
        } else if (index < 11) {
            return regs[index];
        } else {
            return 0;
        }

    }

    private void readPins(byte[] regs, List<Connection<PinProtocolCategory>> ios) {
        for (int i = 0; i < 8; i++) {
            byte mask = (byte) (1 << i);
            boolean invert = (regs[IPOL] & mask) != 0;
            boolean state = ((PinStatePacket) ios.get(i).sendMessage(entity, new PinReadPacket())).getState() > 2.5;
            if (state != invert) {
                regs[GPIO] |= mask;
            } else {
                regs[GPIO] &= ~mask;
            }
        }
    }

    private void setupPins(byte[] regs, List<Connection<PinProtocolCategory>> ios) {
        for (int i = 0; i < 8; i++) {
            byte mask = (byte) (1 << i);
            if ((regs[IODIR] & mask) != 0) {
                // read mode, check for weak pullups only
                if ((regs[GPPU] & mask) != 0) {
                    ios.get(i).sendMessage(entity, PinWritePacket.digitalDrive5V(true, 66000));
                }
            } else {
                // write mode
                ios.get(i).sendMessage(entity, PinWritePacket.digitalDrive5V((regs[OLAT] & mask) != 0));
            }
        }
    }

    private void checkInterrupt(byte[] regs, int i, double state) {
        byte mask = (byte) (1 << i);
        boolean invert = (regs[IPOL] & mask) != 0;
        boolean reading = (state > 2.05) != invert;
        if ((regs[GPINTEN] & mask) != 0) {
            boolean prev;
            if ((regs[INTCON] & mask) != 0) {
                prev = (regs[DEFVAL] & mask) != 0;
            } else {
                prev = (regs[GPIO] & mask) != 0;
            }
            if (prev != reading) {
                regs[INTCAP] = regs[GPIO];
                regs[INTF] |= mask;
                if (!isInterrupt) {
                    isInterrupt = true;
                    outCon.sendMessage(entity, new SmBusAlertPacket(true));
                }
            }
        }
        if (reading) {
            regs[GPIO] |= mask;
        } else {
            regs[GPIO] &= ~mask;
        }
    }

    private int findActualAddress() {
        if ((IoConState & 0x80) != 0) {
            return regAddress & 0xF;
        } else {
            return regAddress / 2;
        }
    }

    private boolean findSide() {
        if ((IoConState & 0x80) != 0) {
            return regAddress < 0x10;
        } else {
            return (regAddress % 2) == 0;
        }
    }

    private byte read() {
        boolean side = findSide();
        int actualAddress = findActualAddress();
        if ((IoConState & 0x20) == 0) {
            regAddress++;
        }
        return readReg(side ? regsA : regsB, actualAddress);
    }

    private void write(byte b) {
        boolean side = findSide();
        int actualAddress = findActualAddress();
        if ((IoConState & 0x20) == 0) {
            regAddress++;
        }
        writeReg(side ? regsA : regsB, b, actualAddress);
    }

    private byte[] readAll(int length) {
        byte[] data = new byte[length];
        for (int i = 0; i < length; i++) {
            data[i] = read();
        }
        return data;
    }

    private void writeAll(byte[] data) {
        if (data.length == 0) {
            return;
        }
        regAddress = Byte.toUnsignedInt(data[0]);
        for (int i = 1; i < data.length; i++) {
            write(data[i]);
        }
        if (data.length > 1) {
            setupPins(regsA, iosA);
            setupPins(regsB, iosB);
        }
    }

    @Override
    public <U extends ProtocolCategory, T extends ProtocolConnection<U, T>> CommunicationResponse<T> acceptIncoming(Class<U> type, int index,
            CommunicationPacket<T> packet) {
        if (type == I2cProtocolCategory.class) {
            if (packet.getClass() == I2cSendPacket.class) {
                writeAll(((I2cSendPacket) packet).getData());
                return packet.generateResponse(((I2cSendPacket) packet).success());
            } else if (packet.getClass() == I2cReceivePacket.class) {
                return packet.generateResponse(((I2cReceivePacket) packet).success(readAll(((I2cReceivePacket) packet).getLength())));
            } else {
                throw new IllegalArgumentException("Invalid I2C Message for MCP23017: " + packet.getClass().getName());
            }
        } else if (packet.getClass() == PinStatePacket.class) {
            if (index < 8) {
                checkInterrupt(regsA, index, ((PinStatePacket) packet).getState());
            } else if (index < 16) {
                checkInterrupt(regsB, index - 8, ((PinStatePacket) packet).getState());
            } else {
                throw new IllegalArgumentException("Invalid pin index for MCP23017: " + index);
            }
            return new BlankCommunicationResponse<T>(packet.getProtocolConnectionType());
        } else {
            throw new IllegalArgumentException("Invalid communication for MCP23017: " + packet.getProtocolConnectionType().getName());
        }
    }

}
