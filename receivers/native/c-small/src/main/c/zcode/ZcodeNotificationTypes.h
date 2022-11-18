/*
 * ZcodeNotificationTypes.h
 *
 *  Created on: 18 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_ZCODENOTIFICATIONTYPES_H_
#define SRC_MAIN_C_ZCODE_ZCODENOTIFICATIONTYPES_H_

enum ZcodeNotificationType {
    ResetNotification = 0,
    BusNotification = 1,
    ScriptSpaceNotification = 2,
    AddressingNotification = 3
};
typedef enum ZcodeNotificationType ZcodeNotificationType;

#endif /* SRC_MAIN_C_ZCODE_ZCODENOTIFICATIONTYPES_H_ */
