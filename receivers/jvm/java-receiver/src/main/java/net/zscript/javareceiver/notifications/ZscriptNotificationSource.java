package net.zscript.javareceiver.notifications;

import javax.annotation.Nonnull;

import net.zscript.javareceiver.core.LockSet;
import net.zscript.javareceiver.core.SequenceOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.execution.ActionSource;
import net.zscript.javareceiver.notifications.ZscriptNotificationAction.NotificationActionType;
import net.zscript.javareceiver.semanticparser.ContextView.AsyncActionNotifier;

public class ZscriptNotificationSource implements ActionSource {
    enum NotificationSourceState {
        NO_NOTIFICATION,
        NOTIFICATION_READY,
        NOTIFICATION_INCOMPLETE,
        NOTIFICATION_NEEDS_ACTION,
        NOTIFICATION_COMPLETE
    }

    private volatile LockSet                 locks         = null;
    private volatile int                     id            = 0;
    private volatile NotificationSourceState state         = NotificationSourceState.NO_NOTIFICATION;
    private volatile boolean                 isAddressing  = false;
    private          NotificationActionType  currentAction = NotificationActionType.WAIT_FOR_NOTIFICATION;
    private          boolean                 locked        = false;

    @Override
    public ZscriptNotificationAction getAction() {
        while (currentAction == NotificationActionType.INVALID) {
            currentAction = getActionType();
        }
        return new ZscriptNotificationAction(this, currentAction);
    }

    private NotificationActionType getActionType() {
        switch (state) {
        case NO_NOTIFICATION:
            return NotificationActionType.WAIT_FOR_NOTIFICATION;
        case NOTIFICATION_READY:
            state = NotificationSourceState.NOTIFICATION_INCOMPLETE;
            return NotificationActionType.NOTIFICATION_BEGIN;
        case NOTIFICATION_INCOMPLETE:
            return NotificationActionType.WAIT_FOR_ASYNC;
        case NOTIFICATION_NEEDS_ACTION:
            state = NotificationSourceState.NOTIFICATION_INCOMPLETE;
            return NotificationActionType.NOTIFICATION_MOVE_ALONG;
        case NOTIFICATION_COMPLETE:
            return NotificationActionType.NOTIFICATION_END;
        default:
            throw new IllegalStateException();
        }

    }

    @Override
    public SequenceOutStream getOutStream(Zscript zscript) {
        return zscript.getNotificationOutStream();
    }

    public int getID() {
        return id;
    }

    public LockSet getLocks() {
        return locks;
    }

    public boolean isAddressing() {
        return isAddressing;
    }

    public boolean setNotification(LockSet locks, int notificationID) {
        if (state != NotificationSourceState.NO_NOTIFICATION) {
            return false;
        }
        this.locks = locks;
        this.isAddressing = false;
        this.id = notificationID;
        state = NotificationSourceState.NOTIFICATION_READY;
        return true;
    }

    public boolean setNotification(int notificationID) {
        if (state != NotificationSourceState.NO_NOTIFICATION) {
            return false;
        }
        this.locks = null;
        this.isAddressing = false;
        this.id = notificationID;
        state = NotificationSourceState.NOTIFICATION_READY;
        return true;
    }

    public boolean setAddressing(LockSet locks, int addressingID) {
        if (state != NotificationSourceState.NO_NOTIFICATION) {
            return false;
        }
        this.locks = locks;
        this.isAddressing = false;
        this.id = addressingID;
        state = NotificationSourceState.NOTIFICATION_READY;
        return true;
    }

    public boolean setAddressing(int addressingID) {
        if (state != NotificationSourceState.NO_NOTIFICATION) {
            return false;
        }
        this.locks = null;
        this.isAddressing = false;
        this.id = addressingID;
        state = NotificationSourceState.NOTIFICATION_READY;
        return true;
    }

    public boolean set(LockSet locks, int notificationID, boolean isAddressing) {
        if (state != NotificationSourceState.NO_NOTIFICATION) {
            return false;
        }
        this.locks = locks;
        this.isAddressing = isAddressing;
        this.id = notificationID;
        state = NotificationSourceState.NOTIFICATION_READY;
        return true;
    }

    public boolean hasNotification() {
        return state != NotificationSourceState.NO_NOTIFICATION;
    }

    public void setNotificationComplete(final boolean b) {
        switch (state) {
        case NOTIFICATION_COMPLETE:
        case NOTIFICATION_INCOMPLETE:
            state = b ? NotificationSourceState.NOTIFICATION_COMPLETE : NotificationSourceState.NOTIFICATION_INCOMPLETE;
            break;
        default:
            throw new IllegalStateException("Invalid state transition");
        }
    }

    @Nonnull
    public AsyncActionNotifier getAsyncActionNotifier() {
        return () -> {
            switch (state) {
            case NOTIFICATION_INCOMPLETE:
                state = NotificationSourceState.NOTIFICATION_NEEDS_ACTION;
                break;
            default:
                throw new IllegalStateException("Invalid state transition");
            }
        };
    }

    public void actionExecuted() {
        if (currentAction == NotificationActionType.NOTIFICATION_END) {
            state = NotificationSourceState.NO_NOTIFICATION;
        }
        currentAction = NotificationActionType.INVALID;
    }

    public boolean isNotificationComplete() {
        return state == NotificationSourceState.NOTIFICATION_COMPLETE;
    }

    public boolean canLock(final Zscript zscript) {
        if (locked) {
            return true;
        }
        LockSet ls = locks;
        if (ls == null) {
            ls = LockSet.allLocked();
        }
        return zscript.canLock(ls);
    }

    public boolean lock(Zscript zscript) {
        if (locked) {
            return true;
        }
        LockSet ls = locks;
        if (ls == null) {
            ls = LockSet.allLocked();
        }
        locked = zscript.lock(ls);
        return locked;
    }

    public void unlock(Zscript zscript) {
        if (!locked) {
            return;
        }
        LockSet ls = locks;
        if (ls == null) {
            ls = LockSet.allLocked();
        }
        zscript.unlock(ls);
        locked = false;
    }

}
