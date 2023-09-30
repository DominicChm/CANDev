#ifndef CAN_HAL_H
#define CAN_HAL_H
#include "can_common.h"

bool can_hal_begin();
int can_hal_available();
int can_hal_write_available();
canmsg_t can_hal_read();
bool can_hal_write(canmsg_t *msg);

#endif