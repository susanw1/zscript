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

public class Main3 {

    public static void main(String[] args) {
        MCP23017 io = new MCP23017((byte) 0x2);
        ZcodeEntityController device = new ZcodeEntityController();
        Connection<I2cProtocolCategory> i2ccon1 = device.getEntity().connectNew(I2cProtocolCategory.class);
        i2ccon1.addProtocol(new I2cConnection(i2ccon1));
        i2ccon1.addProtocol(new SmBusAlertConnection(i2ccon1));
        Connection<I2cProtocolCategory> i2ccon2 = device.getEntity().connectNew(I2cProtocolCategory.class);
        i2ccon2.addProtocol(new I2cConnection(i2ccon2));
        SmBusAlertConnection alertBetween = new SmBusAlertConnection(i2ccon2);
        i2ccon2.addProtocol(alertBetween);
        alertBetween.setAlertReceiver(device.getEntity());
        I2cChannel channel = I2cChannel.build(device.getEntity(), new I2cAddress(0x10), 0, 128);
        device.addChannel(channel);
        device.add(channel, 0);
        I2cModule module = new I2cModule(device.getEntity());
        device.addModule(module);
        device.add(module.getNotificationHandler(), 1);
        device.add(module.getNotificationSource());

        ZcodeEntityController device2 = new ZcodeEntityController();
        Connection<I2cProtocolCategory> i2ccon3 = device2.getEntity().connectNew(I2cProtocolCategory.class);
        i2ccon3.addProtocol(new I2cConnection(i2ccon3));
        i2ccon3.addProtocol(new SmBusAlertConnection(i2ccon3));
        device2.getEntity().connectNew(I2cProtocolCategory.class);
        device2.getEntity().getConnection(I2cProtocolCategory.class, 1).addProtocol(new I2cConnection(device2.getEntity().getConnection(I2cProtocolCategory.class, 1)));
        I2cChannel channel2 = I2cChannel.build(device2.getEntity(), new I2cAddress(0x12), 0, 128);
        device2.addChannel(channel2);
        device2.add(channel2, 0);
        I2cModule module2 = new I2cModule(device2.getEntity());
        device2.addModule(module2);
        device2.add(module2.getNotificationHandler(), 0);
        device2.add(module2.getNotificationHandler(), 1);
        device2.add(module2.getNotificationSource());

        LED led1 = new LED();
        LED led2 = new LED();
        LED led3 = new LED();
        Button button1 = new Button();
        io.getEntity().connect(PinProtocolCategory.class, 0, led1.getEntity(), 0);
        io.getEntity().connect(PinProtocolCategory.class, 1, led2.getEntity(), 0);
        io.getEntity().connect(PinProtocolCategory.class, 2, led3.getEntity(), 0);
        io.getEntity().connect(PinProtocolCategory.class, 3, button1.getEntity(), 0);

        device.getEntity().connect(I2cProtocolCategory.class, 1, device2.getEntity(), 0);
        device2.getEntity().connect(I2cProtocolCategory.class, 0, io.getEntity(), 0);
        device2.getEntity().getConnection(I2cProtocolCategory.class, 0).tapConnection(new PrintingConnectionTap());
        Connection<I2cProtocolCategory> I2C = device.getEntity().getConnection(I2cProtocolCategory.class, 0);
        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        I2C.sendMessage(io.getEntity(), new I2cSendPacket(new I2cAddress(0x10), "Z2&Z18N&Z51NAP1&Z52P1A12\"Z=0a\"\n".getBytes(StandardCharsets.UTF_8))); // set
        // IODIR
        try {
            while (device.hasActivity() || device2.hasActivity())
                Thread.sleep(10);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        System.out.println(StandardCharsets.UTF_8
                .decode(ByteBuffer.wrap(((I2cReceiveResponse) I2C.sendMessage(io.getEntity(), new I2cReceivePacket(new I2cAddress(0x10), 32))).getData())).toString()); // set IODIR
        System.out.println(StandardCharsets.UTF_8
                .decode(ByteBuffer.wrap(((I2cReceiveResponse) I2C.sendMessage(io.getEntity(), new I2cReceivePacket(new I2cAddress(0x10), 32))).getData())).toString()); // set IODIR

        I2C.sendMessage(io.getEntity(), new I2cSendPacket(new I2cAddress(0x10), "@5.1.12Z2&Z52P0A22+00f8&Z52P0A22+14ffff&Z52P0A22+140000\n".getBytes(StandardCharsets.UTF_8))); // set
        try {
            while (device.hasActivity() || device2.hasActivity())
                Thread.sleep(10);
//            Thread.sleep(500);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        System.out.println(StandardCharsets.UTF_8
                .decode(ByteBuffer.wrap(((I2cReceiveResponse) I2C.sendMessage(io.getEntity(), new I2cReceivePacket(new I2cAddress(0x10), 32))).getData())).toString()); // set IODIR
        System.out.println(StandardCharsets.UTF_8
                .decode(ByteBuffer.wrap(((I2cReceiveResponse) I2C.sendMessage(io.getEntity(), new I2cReceivePacket(new I2cAddress(0x10), 32))).getData())).toString()); // set IODIR
        System.out.println(StandardCharsets.UTF_8
                .decode(ByteBuffer.wrap(((I2cReceiveResponse) I2C.sendMessage(io.getEntity(), new I2cReceivePacket(new I2cAddress(0x10), 32))).getData())).toString()); // set IODIR
        device.stop();
        device2.stop();
    }

}
