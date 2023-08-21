/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_NOTIFICATIONS_ZSCRIPTNOTIFICATIONSOURCE_HPP_
#define SRC_MAIN_C___ZSCRIPT_NOTIFICATIONS_ZSCRIPTNOTIFICATIONSOURCE_HPP_
#include "../execution/LockSet.hpp"

namespace Zscript {
namespace GenericCore {
enum class NotificationActionType : uint8_t {
    INVALID,
    WAIT_FOR_NOTIFICATION,
    WAIT_FOR_ASYNC,
    NOTIFICATION_BEGIN,
    NOTIFICATION_MOVE_ALONG,
    NOTIFICATION_END
};

enum NotificationSourceState {
    NO_NOTIFICATION,
    NOTIFICATION_READY,
    NOTIFICATION_INCOMPLETE,
    NOTIFICATION_NEEDS_ACTION,
    NOTIFICATION_COMPLETE
};

template<class ZP>
class ZscriptAction;

template<class ZP>
class ZscriptNotificationSource {
private:
    LockSet<ZP> *volatile locks = NULL;
    volatile uint16_t id = 0;
    volatile NotificationSourceState state = NO_NOTIFICATION;
    volatile bool isAddressingB = false;
    NotificationActionType currentAction = NotificationActionType::INVALID;
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

    ZscriptAction<ZP> getAction();

    uint16_t getID() {
        return id;
    }

    LockSet<ZP>* getLocks() {
        return locks;
    }

    bool isAddressing() {
        return isAddressingB;
    }

    bool setNotification(LockSet<ZP> *locks, uint16_t notificationID) {
        if (state != NotificationSourceState::NO_NOTIFICATION) {
            return false;
        }
        this->locks = locks;
        this->isAddressingB = false;
        this->id = notificationID;
        state = NotificationSourceState::NOTIFICATION_READY;
        return true;
    }

    bool setNotification(uint16_t notificationID) {
        if (state != NotificationSourceState::NO_NOTIFICATION) {
            return false;
        }
        this->locks = NULL;
        this->isAddressingB = false;
        this->id = notificationID;
        state = NotificationSourceState::NOTIFICATION_READY;
        return true;
    }

    bool setAddressing(LockSet<ZP> *locks, uint16_t addressingID) {
        if (state != NotificationSourceState::NO_NOTIFICATION) {
            return false;
        }
        this->locks = locks;
        this->isAddressingB = false;
        this->id = addressingID;
        state = NotificationSourceState::NOTIFICATION_READY;
        return true;
    }

    bool setAddressing(uint16_t addressingID) {
        if (state != NotificationSourceState::NO_NOTIFICATION) {
            return false;
        }
        this->locks = NULL;
        this->isAddressingB = false;
        this->id = addressingID;
        state = NotificationSourceState::NOTIFICATION_READY;
        return true;
    }

    bool set(LockSet<ZP> *locks, uint16_t notificationID, bool isAddressingB) {
        if (state != NotificationSourceState::NO_NOTIFICATION) {
            return false;
        }
        this->locks = locks;
        this->isAddressingB = isAddressingB;
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

    bool canLock() {
        if (locked) {
            return true;
        }
        if (locks == NULL) {
            LockSet<ZP> ls = LockSet<ZP>::allLocked();
            return Zscript < ZP > ::zscript.canLock(&ls);
        }
        return Zscript < ZP > ::zscript.canLock(locks);
    }

    bool lock() {
        if (locked) {
            return true;
        }
        if (locks == NULL) {
            LockSet<ZP> ls = LockSet<ZP>::allLocked();
            locked = Zscript < ZP > ::zscript.lock(&ls);
            return locked;
        }
        locked = Zscript < ZP > ::zscript.lock(locks);
        return locked;
    }

    void unlock() {
        if (!locked) {
            return;
        }
        if (locks == NULL) {
            LockSet<ZP> ls = LockSet<ZP>::allLocked();
            Zscript < ZP > ::zscript.unlock(&ls);
            locked = false;
            return;
        }
        Zscript < ZP > ::zscript.unlock(locks);
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
