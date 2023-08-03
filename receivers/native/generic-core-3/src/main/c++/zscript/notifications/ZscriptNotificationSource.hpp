/*
 * Zscript<ZP>*NotificationSource.hpp
 *
 *  Created on: 3 Aug 2023
 *      Author: alicia
 */

#ifndef SRC_MAIN_C___ZSCRIPT_NOTIFICATIONS_ZSCRIPTNOTIFICATIONSOURCE_HPP_
#define SRC_MAIN_C___ZSCRIPT_NOTIFICATIONS_ZSCRIPTNOTIFICATIONSOURCE_HPP_
#include "../execution/LockSet.hpp"
#include "../execution/ZscriptAction.hpp"

namespace Zscript {
namespace GenericCore {
enum NotificationSourceState {
    NO_NOTIFICATION,
    NOTIFICATION_READY,
    NOTIFICATION_INCOMPLETE,
    NOTIFICATION_NEEDS_ACTION,
    NOTIFICATION_COMPLETE
};

template<class ZP>
class ZscriptNotificationSource {
private:
    volatile LockSet<ZP> *locks = NULL;
    volatile int id = 0;
    volatile NotificationSourceState state = NO_NOTIFICATION;
    volatile bool isAddressingB = false;
    NotificationActionType currentAction = SemanticActionType::INVALID;
    bool locked = false;

    NotificationActionType getActionType() {
        switch (state) {
        case NO_NOTIFICATION:
            return NotificationActionType::WAIT_FOR_NOTIFICATION;
        case NOTIFICATION_READY:
            state = NotificationSourceState::NOTIFICATION_INCOMPLETE;
            return NotificationActionType::NOTIFICATION_BEGIN;
        case NOTIFICATION_INCOMPLETE:
            return NotificationActionType::WAIT_FOR_ASYNC;
        case NOTIFICATION_NEEDS_ACTION:
            state = NotificationSourceState::NOTIFICATION_INCOMPLETE;
            return NotificationActionType::NOTIFICATION_MOVE_ALONG;
        case NOTIFICATION_COMPLETE:
            return NotificationActionType::NOTIFICATION_END;
        default:
            return NotificationActionType::INVALID;
        }

    }

public:

    ZscriptAction<ZP> getAction() {
        while (currentAction == NotificationActionType::INVALID) {
            currentAction = getActionType();
        }
        return new ZscriptAction<ZP>(this, currentAction);
    }

    int getID() {
        return id;
    }

    LockSet<ZP>* getLocks() {
        return locks;
    }

    bool isAddressing() {
        return isAddressing;
    }

    bool setNotification(LockSet<ZP> *locks, int notificationID) {
        if (state != NotificationSourceState::NO_NOTIFICATION) {
            return false;
        }
        this->locks = locks;
        this->isAddressing = false;
        this->id = notificationID;
        state = NotificationSourceState::NOTIFICATION_READY;
        return true;
    }

    bool setNotification(int notificationID) {
        if (state != NotificationSourceState::NO_NOTIFICATION) {
            return false;
        }
        this->locks = NULL;
        this->isAddressing = false;
        this->id = notificationID;
        state = NotificationSourceState::NOTIFICATION_READY;
        return true;
    }

    bool setAddressing(LockSet<ZP> *locks, int addressingID) {
        if (state != NotificationSourceState::NO_NOTIFICATION) {
            return false;
        }
        this->locks = locks;
        this->isAddressing = false;
        this->id = addressingID;
        state = NotificationSourceState::NOTIFICATION_READY;
        return true;
    }

    bool setAddressing(int addressingID) {
        if (state != NotificationSourceState::NO_NOTIFICATION) {
            return false;
        }
        this->locks = NULL;
        this->isAddressing = false;
        this->id = addressingID;
        state = NotificationSourceState::NOTIFICATION_READY;
        return true;
    }

    bool set(LockSet<ZP> *locks, int notificationID, bool isAddressing) {
        if (state != NotificationSourceState::NO_NOTIFICATION) {
            return false;
        }
        this->locks = locks;
        this->isAddressing = isAddressing;
        this->id = notificationID;
        state = NotificationSourceState::NOTIFICATION_READY;
        return true;
    }

    bool hasNotification() {
        return state != NotificationSourceState::NO_NOTIFICATION;
    }

    void setNotificationComplete(bool b) {
        switch (state) {
        case NOTIFICATION_COMPLETE:
            case NOTIFICATION_INCOMPLETE:
            state = b ? NotificationSourceState::NOTIFICATION_COMPLETE : NotificationSourceState::NOTIFICATION_INCOMPLETE;
            break;
        default:
            //unreachable
            break;
        }
    }

    void actionPerformed(NotificationActionType action) {
        if (action == NotificationActionType::NOTIFICATION_END) {
            state = NotificationSourceState::NO_NOTIFICATION;
        }
        currentAction = NotificationActionType::INVALID;
    }

    bool isNotificationComplete() {
        return state == NotificationSourceState::NOTIFICATION_COMPLETE;
    }
    void notifyNeedsAction() {
        switch (state) {
        case NotificationSourceState::NOTIFICATION_INCOMPLETE:
            case NotificationSourceState::NOTIFICATION_NEEDS_ACTION:
            state = NOTIFICATION_NEEDS_ACTION;
            break;
        default:
            //Unreachable
            break;
        }
    }

    bool canLock(Zscript<ZP> *zscript) {
        if (locked) {
            return true;
        }
        if (locks == NULL) {
            LockSet<ZP> ls = LockSet<ZP>::allLocked();
            return zscript->canLock(&ls);
        }
        return zscript->canLock(locks);
    }

    bool lock(Zscript<ZP> *zscript) {
        if (locked) {
            return true;
        }
        if (locks == NULL) {
            LockSet<ZP> ls = LockSet<ZP>::allLocked();
            locked = zscript->lock(&ls);
            return locked;
        }
        locked = zscript->lock(locks);
        return locked;
    }

    void unlock(Zscript<ZP> *zscript) {
        if (!locked) {
            return;
        }
        if (locks == NULL) {
            LockSet<ZP> ls = LockSet<ZP>::allLocked();
            zscript->unlock(&ls);
            locked = false;
            return;
        }
        zscript->unlock(locks);
        locked = false;
        return;
    }
    AsyncActionNotifier<ZP> getAsyncActionNotifier() {
        return AsyncActionNotifier<ZP>(this);
    }
}
;
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_NOTIFICATIONS_ZSCRIPTNOTIFICATIONSOURCE_HPP_ */
