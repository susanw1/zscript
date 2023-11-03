package net.zscript.demo.morse;

import static net.zscript.model.modules.base.PinsModule.DigitalNotificationSectionContent.Value;

import java.util.ArrayList;
import java.util.List;

import net.zscript.javaclient.commandbuilder.commandnodes.ResponseCaptor;
import net.zscript.javaclient.devices.Device;
import net.zscript.model.modules.base.CoreModule;
import net.zscript.model.modules.base.OuterCoreModule;
import net.zscript.model.modules.base.PinsModule;
import net.zscript.model.modules.base.PinsModule.DigitalSetupCommand.Builder.Mode;
import net.zscript.model.modules.base.PinsModule.DigitalSetupCommand.Builder.NotificationMode;

public class MorseReceiver {
    private final MorseTranslator translator;
    private final Device          device;
    private final long            ditPeriodMaxUs;
    private final int             pin;

    private final List<MorseElement> currentCharacter = new ArrayList<>();

    private long lastTimeNano;

    public MorseReceiver(MorseTranslator translator, Device device, long ditPeriodMaxUs, int pin) {
        this.translator = translator;
        this.device = device;
        this.ditPeriodMaxUs = ditPeriodMaxUs;
        this.pin = pin;
    }

    public void startReceiving() {
        try {
            lastTimeNano = System.nanoTime();
            ResponseCaptor<PinsModule.DigitalNotificationSectionContent> captor = new ResponseCaptor<>();
            device.getNotificationHandle(PinsModule.DigitalNotificationId.get()).getSection(PinsModule.DigitalNotificationSectionId.get()).setCaptor(captor);
            device.setNotificationListener(PinsModule.DigitalNotificationId.get(), notificationSequenceCallback -> {
                PinsModule.DigitalNotificationSectionContent content = notificationSequenceCallback.getResponseFor(captor).get();

                long   current = System.nanoTime();
                double time    = (current - lastTimeNano) / 1000.0 / ditPeriodMaxUs;
                if (content.getValue() != Value.High) {
                    if (time > 2) {
                        currentCharacter.add(MorseElement.DAR);
                    } else {
                        currentCharacter.add(MorseElement.DIT);
                    }
                } else {
                    if (time > 2) {
                        System.out.print(translator.translate(currentCharacter));
                        currentCharacter.clear();
                        if (time > 6) {
                            System.out.print(' ');
                        }
                    }
                }
                lastTimeNano = current;
            });
            device.sendAndWaitExpectSuccess(CoreModule.activateBuilder().build());
            device.sendAndWaitExpectSuccess(OuterCoreModule.channelSetupBuilder().setAssignNotification().build());
            device.sendAndWaitExpectSuccess(PinsModule.digitalSetupBuilder().setPin(pin).setMode(Mode.Input).setNotificationMode(NotificationMode.OnChange).build());
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }

    public void close() {
        System.out.print(translator.translate(currentCharacter));
    }
}
