package net.zscript.demo.morse;

import static net.zscript.model.modules.base.PinsModule.DigitalNotificationSectionContent.Value;

import net.zscript.javaclient.commandbuilder.commandnodes.ResponseCaptor;
import net.zscript.javaclient.devices.Device;
import net.zscript.model.modules.base.CoreModule;
import net.zscript.model.modules.base.OuterCoreModule;
import net.zscript.model.modules.base.PinsModule;
import net.zscript.model.modules.base.PinsModule.DigitalSetupCommand.Builder.Mode;
import net.zscript.model.modules.base.PinsModule.DigitalSetupCommand.Builder.NotificationMode;

public class MorseReceiver {
    private final Device device;
    private final long   ditPeriodMaxMs;
    private final int    pin;

    private long lastTimeMs;

    public MorseReceiver(Device device, long ditPeriodMaxMs, int pin) {
        this.device = device;
        this.ditPeriodMaxMs = ditPeriodMaxMs;
        this.pin = pin;
    }

    public void startReceiving() {
        try {
            lastTimeMs = System.currentTimeMillis();
            ResponseCaptor<PinsModule.DigitalNotificationSectionContent> captor = new ResponseCaptor<>();
            device.getNotificationHandle(PinsModule.DigitalNotificationId.get()).getSection(PinsModule.DigitalNotificationSectionId.get()).setCaptor(captor);
            device.setNotificationListener(PinsModule.DigitalNotificationId.get(), notificationSequenceCallback -> {
                PinsModule.DigitalNotificationSectionContent content = notificationSequenceCallback.getResponseFor(captor).get();

                long current = System.currentTimeMillis();
                double time = (current - lastTimeMs + 0.0) / ditPeriodMaxMs;
                if (content.getValue() != Value.High) {
                    if(time>2){
                        System.out.print('-');
                    }else{
                        System.out.print('.');
                    }
                } else {
                    if(time>6){
                        System.out.print('/');
                    }else if (time>2){
                        System.out.print(' ');
                    }
                }
                lastTimeMs = current;
            });
            device.sendAndWaitExpectSuccess(CoreModule.activateBuilder().build());
            device.sendAndWaitExpectSuccess(OuterCoreModule.channelSetupBuilder().setAssignNotification().build());
            device.sendAndWaitExpectSuccess(PinsModule.digitalSetupBuilder().setPin(pin).setMode(Mode.Input).setNotificationMode(NotificationMode.OnChange).build());
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }
}
