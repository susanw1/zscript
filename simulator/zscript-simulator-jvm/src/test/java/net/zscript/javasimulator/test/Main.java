package net.zscript.javasimulator.test;

import net.zscript.javasimulator.Connection;
import net.zscript.javasimulator.connections.i2c.I2cAddress;
import net.zscript.javasimulator.connections.i2c.I2cProtocolCategory;
import net.zscript.javasimulator.connections.i2c.I2cReceivePacket;
import net.zscript.javasimulator.connections.i2c.I2cReceiveResponse;
import net.zscript.javasimulator.connections.i2c.I2cSendPacket;
import net.zscript.javasimulator.connections.pin.PinProtocolCategory;
import net.zscript.javasimulator.devices.Button;
import net.zscript.javasimulator.devices.LED;
import net.zscript.javasimulator.devices.MCP23017;

public class Main {

    public static void main(String[] args) {
        MCP23017 io = new MCP23017((byte) 0x2);
        LED led1 = new LED();
        LED led2 = new LED();
        LED led3 = new LED();
        Button button1 = new Button();
        io.getEntity().connect(PinProtocolCategory.class, 0, led1.getEntity(), 0);
        io.getEntity().connect(PinProtocolCategory.class, 1, led2.getEntity(), 0);
        io.getEntity().connect(PinProtocolCategory.class, 2, led3.getEntity(), 0);
        io.getEntity().connect(PinProtocolCategory.class, 3, button1.getEntity(), 0);

        Connection<I2cProtocolCategory> I2C = io.getEntity().getConnection(I2cProtocolCategory.class, 0);
        I2C.sendMessage(io.getEntity(), new I2cSendPacket(new I2cAddress(0x22), new byte[] { 0x00, (byte) 0xF8 })); // set IODIR
        I2C.sendMessage(io.getEntity(), new I2cSendPacket(new I2cAddress(0x22), new byte[] { 0x14, (byte) 0xFF, (byte) 0xFF })); // set OLAT to turn LEDs on
        I2C.sendMessage(io.getEntity(), new I2cSendPacket(new I2cAddress(0x22), new byte[] { 0x14, (byte) 0x00, (byte) 0x00 })); // set OLAT to turn LEDs off

        I2C.sendMessage(io.getEntity(), new I2cSendPacket(new I2cAddress(0x22), new byte[] { 0x0A, (byte) 0x64 })); // turn off sequential mode

        I2C.sendMessage(io.getEntity(), new I2cSendPacket(new I2cAddress(0x22), new byte[] { 0x12 })); // put the address at the GPIO reg

        // repeatedly read the button
        System.out.println(((I2cReceiveResponse) I2C.sendMessage(io.getEntity(), new I2cReceivePacket(new I2cAddress(0x22), 1))).getData()[0] != 0);
        button1.press();
        System.out.println(((I2cReceiveResponse) I2C.sendMessage(io.getEntity(), new I2cReceivePacket(new I2cAddress(0x22), 1))).getData()[0] != 0);
        button1.release();
        System.out.println(((I2cReceiveResponse) I2C.sendMessage(io.getEntity(), new I2cReceivePacket(new I2cAddress(0x22), 1))).getData()[0] != 0);

    }

}
