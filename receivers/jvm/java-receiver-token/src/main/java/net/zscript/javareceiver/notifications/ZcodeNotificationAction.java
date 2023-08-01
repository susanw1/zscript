package net.zscript.javareceiver.notifications;

import net.zscript.javareceiver.core.Zcode;
import net.zscript.javareceiver.core.ZcodeCommandOutStream;
import net.zscript.javareceiver.core.ZcodeLockSet;
import net.zscript.javareceiver.core.ZcodeOutStream;
import net.zscript.javareceiver.execution.ZcodeAction;

public class ZcodeNotificationAction implements ZcodeAction {
    private final ZcodeNotificationSource source;

    public ZcodeNotificationAction(ZcodeNotificationSource source) {
        this.source = source;
    }

    @Override
    public boolean isEmptyAction() {
        return !source.hasNotification();
    }

    @Override
    public void performAction(Zcode z, ZcodeOutStream out) {
        if (!source.hasNotification() || out == null) {
            return;
        }
        startResponse(out);
        if (z.getModuleRegistry().notification(out, source.getID(), source.isAddressing())) {
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
        ZcodeCommandOutStream commandStream = out.asCommandOutStream();
        if (source.isAddressing()) {
            commandStream.writeField('!', 0);
//            commandStream.writeField(Zchars.Z_ADDRESSING, (source.getID() >> 4));
        } else {
            commandStream.writeField('!', (source.getID() >> 4));
        }
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
