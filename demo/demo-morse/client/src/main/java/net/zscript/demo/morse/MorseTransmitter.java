package net.zscript.demo.morse;

import java.util.List;

import static net.zscript.model.modules.base.PinsModule.digitalWriteBuilder;

import net.zscript.javaclient.devices.ZscriptDevice;
import net.zscript.model.modules.base.PinsModule;
import net.zscript.model.modules.base.PinsModule.DigitalSetupCommand.Builder.Mode;
import net.zscript.model.modules.base.PinsModule.DigitalWriteCommand.Builder.Value;

public class MorseTransmitter {
    private final ZscriptDevice device;
    private final long          ditPeriodUs;
    private final int           pin;

    public MorseTransmitter(ZscriptDevice device, long ditPeriodUs, int pin) {
        this.device = device;
        this.ditPeriodUs = ditPeriodUs;
        this.pin = pin;
    }

    public void start() {
        try {
            device.sendAndWaitExpectSuccess(PinsModule.digitalSetupBuilder().setPin(pin).setMode(Mode.Output).build());
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }

    public void close() {
        try {
            device.sendAndWaitExpectSuccess(PinsModule.digitalSetupBuilder().setPin(pin).setMode(Mode.Input).build());
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }

    public void transmit(List<MorseElement> elements) {
        try {
            boolean lastHigh = false;
            for (MorseElement element : elements) {
                if (lastHigh && element.isHigh()) {
                    sendElement(1, false);
                }
                lastHigh = element.isHigh();
                sendElement(element.getLength(), element.isHigh());
            }
            sendElement(10, false);
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
