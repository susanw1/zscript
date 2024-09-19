package net.zscript.demo.morse;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import static net.zscript.model.modules.base.PinsModule.DigitalNotificationSectionContent.Value;

import net.zscript.javaclient.commandbuilder.commandnodes.ResponseCaptor;
import net.zscript.javaclient.devices.Device;
import net.zscript.model.modules.base.OuterCoreModule;
import net.zscript.model.modules.base.PinsModule;
import net.zscript.model.modules.base.PinsModule.DigitalSetupCommand.Builder.Mode;
import net.zscript.model.modules.base.PinsModule.DigitalSetupCommand.Builder.NotificationMode;

public class MorseReceiver {
    private final ScheduledExecutorService exec = Executors.newSingleThreadScheduledExecutor();

    private final MorseTranslator translator;
    private final Device          device;
    private final long            ditPeriodMaxUs;
    private final int             pin;

    private final List<MorseElement> currentCharacter = new ArrayList<>();

    private long    lastTimeNano;
    private boolean isHigh       = false;
    private boolean hasWrittenWS = true;

    public MorseReceiver(MorseTranslator translator, Device device, long ditPeriodMaxUs, int pin) {
        this.translator = translator;
        this.device = device;
        this.ditPeriodMaxUs = ditPeriodMaxUs;
        this.pin = pin;
    }

    public void startReceiving() {
        try {
            lastTimeNano = System.nanoTime();
            ResponseCaptor<PinsModule.DigitalNotificationSectionContent> captor = ResponseCaptor.create();
            device.getNotificationHandle(PinsModule.DigitalNotificationId.get()).getSection(PinsModule.DigitalNotificationSectionId.get()).setCaptor(captor);
            device.setNotificationListener(PinsModule.DigitalNotificationId.get(), notificationSequenceCallback -> {
                PinsModule.DigitalNotificationSectionContent content = notificationSequenceCallback.getResponseFor(captor).get();

                long   current = System.nanoTime();
                double time    = (current - lastTimeNano) / 1000.0 / ditPeriodMaxUs;
                if (content.getValue() != Value.High) {
                    hasWrittenWS = false;
                    isHigh = false;
                    if (time > 2) {
                        currentCharacter.add(MorseElement.DAR);
                    } else {
                        currentCharacter.add(MorseElement.DIT);
                    }
                } else {
                    isHigh = true;
                    if (time > 2) {
                        if (!currentCharacter.isEmpty()) {
                            System.out.print(translator.translate(currentCharacter));
                            currentCharacter.clear();
                        }
                        if (!hasWrittenWS) {
                            if (time > 10) {
                                System.out.print('\n');
                            } else if (time > 6) {
                                System.out.print(' ');
                            }
                        }
                        hasWrittenWS = true;
                    }
                }
                lastTimeNano = current;
            });
            device.sendAndWaitExpectSuccess(OuterCoreModule.channelSetupBuilder().setAssignNotification().build());
            device.sendAndWaitExpectSuccess(PinsModule.digitalSetupBuilder().setPin(pin).setMode(Mode.Input).setNotificationMode(NotificationMode.OnChange).build());
            exec.scheduleAtFixedRate(this::checkReceive, 100000, ditPeriodMaxUs * 4, TimeUnit.MICROSECONDS);
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }

    private void checkReceive() {
        double time = (System.nanoTime() - lastTimeNano) / 1000.0 / ditPeriodMaxUs;
        if (!isHigh && time > 4) {
            emptyBuffer();
            if (!hasWrittenWS && time > 10) {
                System.out.print('\n');
                hasWrittenWS = true;
            }
        }
    }

    private void emptyBuffer() {
        if (!currentCharacter.isEmpty()) {
            System.out.print(translator.translate(currentCharacter));
            currentCharacter.clear();
        }
    }

    public void close() {
        emptyBuffer();
        exec.shutdown();
    }
}
