package net.zscript.demo.morse;

import static net.zscript.model.modules.base.PinsModule.digitalWriteBuilder;

import java.util.List;

import net.zscript.javaclient.devices.Device;
import net.zscript.model.modules.base.PinsModule;
import net.zscript.model.modules.base.PinsModule.DigitalSetupCommand.Builder.Mode;
import net.zscript.model.modules.base.PinsModule.DigitalWriteCommand.Builder.Value;

public class MorseTransmitter {
    private final Device device;
    private final long   ditPeriodUs;
    private final int    pin;

    public MorseTransmitter(Device device, long ditPeriodUs, int pin) {
        this.device = device;
        this.ditPeriodUs = ditPeriodUs;
        this.pin = pin;
    }

    public void transmit(List<MorseElement> elements) {
        try {
            device.sendAndWaitExpectSuccess(PinsModule.digitalSetupBuilder().setPin(pin).setMode(Mode.Output).build());
            for (MorseElement element : elements) {
                sendElement(element.getLength(), element.isHigh());
                sendElement(1, false);
            }
            device.sendAndWaitExpectSuccess(PinsModule.digitalSetupBuilder().setPin(pin).setMode(Mode.Input).build());
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }

    private void sendElement(int length, boolean isHigh) throws InterruptedException {
        long nsStart = System.nanoTime();
        if (isHigh) {
            device.sendAndWaitExpectSuccess(digitalWriteBuilder().setPin(pin).setValue(Value.High).build());
        } else {
            device.sendAndWaitExpectSuccess(digitalWriteBuilder().setPin(pin).setValue(Value.Low).build());
        }
        while (ditPeriodUs * length - (System.nanoTime() - nsStart) / 1000 > 0) {
        }
    }
}
