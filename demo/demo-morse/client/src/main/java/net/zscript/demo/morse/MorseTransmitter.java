package net.zscript.demo.morse;

import static net.zscript.model.modules.base.PinsModule.digitalWriteBuilder;

import java.util.List;

import net.zscript.javaclient.devices.Device;
import net.zscript.model.modules.base.PinsModule;
import net.zscript.model.modules.base.PinsModule.DigitalSetupCommand.Builder.Mode;
import net.zscript.model.modules.base.PinsModule.DigitalWriteCommand.Builder.Value;

public class MorseTransmitter {
    private final Device device;
    private final long   ditPeriodMs;
    private final int    pin;

    public MorseTransmitter(Device device, long ditPeriodMs, int pin) {
        this.device = device;
        this.ditPeriodMs = ditPeriodMs;
        this.pin = pin;
    }

    public void transmit(List<MorseElement> elements) {
        try {
            device.send(PinsModule.digitalSetupBuilder().setPin(pin).setMode(Mode.Output).build(), r -> {
            });
            Thread.sleep(100);
            for (MorseElement element : elements) {
                sendElement(element.getLength(), element.isHigh());
                sendElement(1, false);
            }
            device.send(PinsModule.digitalSetupBuilder().setPin(pin).setMode(Mode.Input).build(), r -> {
            });
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }

    private void sendElement(int length, boolean isHigh) throws InterruptedException {
        long msStart = System.currentTimeMillis();
        if (isHigh) {
            device.send(digitalWriteBuilder().setPin(pin).setValue(Value.High).build(), r -> {
            });
        } else {
            device.send(digitalWriteBuilder().setPin(pin).setValue(Value.Low).build(), r -> {
            });
        }
        while (System.currentTimeMillis() - msStart < ditPeriodMs * length) {
            //move-along...
            Thread.sleep(1);
        }
    }
}
