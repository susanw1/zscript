package org.zcode.javasimulator.test;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

import org.zcode.javasimulator.Connection;
import org.zcode.javasimulator.connections.i2c.I2cAddress;
import org.zcode.javasimulator.connections.i2c.I2cConnection;
import org.zcode.javasimulator.connections.i2c.I2cProtocolCategory;
import org.zcode.javasimulator.connections.i2c.I2cReceivePacket;
import org.zcode.javasimulator.connections.i2c.I2cReceiveResponse;
import org.zcode.javasimulator.connections.i2c.I2cSendPacket;
import org.zcode.javasimulator.connections.i2c.SmBusAlertConnection;
import org.zcode.javasimulator.connections.pin.PinProtocolCategory;
import org.zcode.javasimulator.devices.Button;
import org.zcode.javasimulator.devices.LED;
import org.zcode.javasimulator.devices.MCP23017;
import org.zcode.javasimulator.zcode.ZcodeEntityController;
import org.zcode.javasimulator.zcode.i2c.I2cChannel;
import org.zcode.javasimulator.zcode.i2c.I2cModule;

public class Main2 {

    public static void main(String[] args) {
        MCP23017 io = new MCP23017((byte) 0x2);
        ZcodeEntityController zcode = new ZcodeEntityController();
        zcode.getEntity().connectNew(I2cProtocolCategory.class);
        zcode.getEntity().getConnection(I2cProtocolCategory.class, 0).addProtocol(new I2cConnection(zcode.getEntity().getConnection(I2cProtocolCategory.class, 0)));
        zcode.getEntity().getConnection(I2cProtocolCategory.class, 0).addProtocol(new SmBusAlertConnection(zcode.getEntity().getConnection(I2cProtocolCategory.class, 0)));
        zcode.getEntity().connectNew(I2cProtocolCategory.class);
        zcode.getEntity().getConnection(I2cProtocolCategory.class, 1).addProtocol(new I2cConnection(zcode.getEntity().getConnection(I2cProtocolCategory.class, 1)));
        I2cChannel channel = I2cChannel.build(zcode.getEntity(), new I2cAddress(0x10), 0, 128);
        zcode.addChannel(channel);
        zcode.add(channel, 0);
        zcode.addModule(new I2cModule(zcode.getEntity()));

        LED led1 = new LED();
        LED led2 = new LED();
        LED led3 = new LED();
        Button button1 = new Button();
        io.getEntity().connect(PinProtocolCategory.class, 0, led1.getEntity(), 0);
        io.getEntity().connect(PinProtocolCategory.class, 1, led2.getEntity(), 0);
        io.getEntity().connect(PinProtocolCategory.class, 2, led3.getEntity(), 0);
        io.getEntity().connect(PinProtocolCategory.class, 3, button1.getEntity(), 0);

        zcode.getEntity().connect(I2cProtocolCategory.class, 1, io.getEntity(), 0);
        Connection<I2cProtocolCategory> I2C = zcode.getEntity().getConnection(I2cProtocolCategory.class, 0);
        I2C.sendMessage(io.getEntity(), new I2cSendPacket(new I2cAddress(0x10), "Z2&Z50&Z52P1A22+00f8&Z52P1A22+14ffff&Z52P1A22+140000\nZ0\n".getBytes(StandardCharsets.UTF_8))); // set
        // IODIR
        try {
            while (zcode.hasActivity())
                Thread.sleep(10);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        System.out.println(StandardCharsets.UTF_8
                .decode(ByteBuffer.wrap(((I2cReceiveResponse) I2C.sendMessage(io.getEntity(), new I2cReceivePacket(new I2cAddress(0x10), 64))).getData())).toString()); // set IODIR
        System.out.println(StandardCharsets.UTF_8
                .decode(ByteBuffer.wrap(((I2cReceiveResponse) I2C.sendMessage(io.getEntity(), new I2cReceivePacket(new I2cAddress(0x10), 64))).getData())).toString()); // set IODIR

        zcode.stop();
    }

}
