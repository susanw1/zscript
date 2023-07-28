package org.zcode.javareceiver.notifications;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.execution.ZcodeAction;

public class ZcodeNotificationAction implements ZcodeAction {
    enum NotificationType {
        NO_ACTION,
        RESET_NOTIFICATION,
        PERIPHERAL_NOTIFICATION,
        SCRIPT_SPACE_NOTIFICATION
    }

    private final NotificationType type;

    @Override
    public boolean isEmptyAction() {
        return type == NotificationType.NO_ACTION;
    }

    @Override
    public void performAction(Zcode z, ZcodeOutStream out) {
        // TODO Auto-generated method stub

    }

    @Override
    public boolean canLock(Zcode z) {
        // TODO
        return false;
    }

    @Override
    public boolean lock(Zcode z) {
        // TODO
        return false;
    }

}
