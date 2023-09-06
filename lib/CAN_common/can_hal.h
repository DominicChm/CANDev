#ifndef CAN_HAL_H
#define CAN_HAL_H
#include "can_common.h"

bool can_hal_begin();
bool can_hal_available();
canmsg_t can_hal_read();
void can_hal_write(canmsg_t *msg);

#endif