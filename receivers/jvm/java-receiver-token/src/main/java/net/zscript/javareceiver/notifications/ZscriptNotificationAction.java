package net.zscript.javareceiver.notifications;

import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.core.ZscriptCommandOutStream;
import net.zscript.javareceiver.core.LockSet;
import net.zscript.javareceiver.core.OutStream;
import net.zscript.javareceiver.execution.ZscriptAction;

public class ZscriptNotificationAction implements ZscriptAction {
    private final ZscriptNotificationSource source;

    public ZscriptNotificationAction(ZscriptNotificationSource source) {
        this.source = source;
    }

    @Override
    public boolean isEmptyAction() {
        return !source.hasNotification();
    }

    @Override
    public void performAction(Zscript z, OutStream out) {
        if (!source.hasNotification() || out == null) {
            return;
        }
        startResponse(out);
        if (z.getModuleRegistry().notification(out, source.getID(), source.isAddressing())) {
            out.endSequence();
            out.close();
            LockSet ls = source.getLocks();
            if (ls == null) {
                ls = LockSet.allLocked();
            }
            z.unlock(ls);
        }
        source.actionExecuted();
    }

    private void startResponse(OutStream out) {
        if (!out.isOpen()) {
            out.open();
        }
        ZscriptCommandOutStream commandStream = out.asCommandOutStream();
        if (source.isAddressing()) {
            commandStream.writeField('!', 0);
//            commandStream.writeField(Zchars.Z_ADDRESSING, (source.getID() >> 4));
        } else {
            commandStream.writeField('!', (source.getID() >> 4));
        }
    }

    @Override
    public boolean canLock(Zscript z) {
        LockSet ls = source.getLocks();
        if (ls == null) {
            ls = LockSet.allLocked();
        }
        return z.canLock(ls);
    }

    @Override
    public boolean lock(Zscript z) {
        LockSet ls = source.getLocks();
        if (ls == null) {
            ls = LockSet.allLocked();
        }
        return z.lock(ls);
    }

}
