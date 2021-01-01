/*
 * RcodeExecutionSpace.cpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#include "RCodeExecutionSpace.hpp"

RCodeCommandChannel* RCodeExecutionSpace::getNotificationChannel() {
    return notifications->getNotificationChannel();
}
