package org.zcode.javareceiver.notifications;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeLockSet;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.execution.ZcodeAction;

public class ZcodeNotificationAction implements ZcodeAction {
    private final ZcodeNotificationSource source;

    public ZcodeNotificationAction(ZcodeNotificationSource source) {
        this.source = source;
    }

    @Override
    public boolean isEmptyAction() {
        return source.getNotificationID() == 0;
    }

    @Override
    public void performAction(Zcode z, ZcodeOutStream out) {
        if (source.getNotificationID() == 0) {
            return;
        }
        startResponse(out);
        if (z.getModuleRegistry().notification(out, source.getNotificationID())) {
            out.endSequence();
            out.close();
            ZcodeLockSet ls = source.getLocks();
            if (ls == null) {
                ls = ZcodeLockSet.allLocked();
            }
            z.unlock(ls);
        }
        source.actionExecuted();
    }

    private void startResponse(ZcodeOutStream out) {
        if (!out.isOpen()) {
            out.open();
        }
        out.writeField('!', (source.getNotificationID() >> 4));
    }

    @Override
    public boolean canLock(Zcode z) {
        ZcodeLockSet ls = source.getLocks();
        if (ls == null) {
            ls = ZcodeLockSet.allLocked();
        }
        return z.canLock(ls);
    }

    @Override
    public boolean lock(Zcode z) {
        ZcodeLockSet ls = source.getLocks();
        if (ls == null) {
            ls = ZcodeLockSet.allLocked();
        }
        return z.lock(ls);
    }

}
