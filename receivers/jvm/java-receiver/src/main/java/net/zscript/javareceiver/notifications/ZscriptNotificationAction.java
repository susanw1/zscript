package net.zscript.javareceiver.notifications;

import net.zscript.javareceiver.core.CommandOutStream;
import net.zscript.javareceiver.core.SequenceOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.execution.NotificationContext;
import net.zscript.javareceiver.execution.ZscriptAction;

public class ZscriptNotificationAction implements ZscriptAction {
    enum NotificationActionType {
        INVALID,
        WAIT_FOR_NOTIFICATION,
        WAIT_FOR_ASYNC,
        NOTIFICATION_BEGIN,
        NOTIFICATION_MOVE_ALONG,
        NOTIFICATION_END
    }

    private final ZscriptNotificationSource source;
    private final NotificationActionType    type;

    ZscriptNotificationAction(ZscriptNotificationSource source, NotificationActionType type) {
        this.source = source;
        this.type = type;
    }

    @Override
    public boolean isEmptyAction() {
        return !source.hasNotification();
    }

    @Override
    public void performAction(Zscript z, SequenceOutStream out) {
        switch (type) {
        case NOTIFICATION_BEGIN:
            startResponse(out);
            z.getModuleRegistry().notification(new NotificationContext(source, z), false);
            break;
        case NOTIFICATION_END:
            out.endSequence();
            out.close();
            unlock(z);
            break;
        case NOTIFICATION_MOVE_ALONG:
            z.getModuleRegistry().notification(new NotificationContext(source, z), true);
            break;
        case WAIT_FOR_ASYNC:
        case WAIT_FOR_NOTIFICATION:
            break;
        case INVALID:
            throw new IllegalStateException();
        }
        source.actionExecuted();
    }

    private void startResponse(SequenceOutStream out) {
        if (!out.isOpen()) {
            out.open();
        }
        CommandOutStream commandStream = out.asCommandOutStream();
        if (source.isAddressing()) {
            commandStream.writeField('!', 0);
            //            commandStream.writeField(Zchars.Z_ADDRESSING, (source.getID() >> 4));
        } else {
            commandStream.writeField('!', (source.getID() >> 4));
        }
    }

    public void unlock(Zscript z) {
        source.unlock(z);
    }

    @Override
    public boolean canLock(Zscript z) {
        return source.canLock(z);
    }

    @Override
    public boolean lock(Zscript z) {
        return source.lock(z);
    }

}
