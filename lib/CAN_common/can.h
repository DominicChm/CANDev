/** can.h
 * CAN framework to reduce boilerplate while remaining reasonably efficient.
 *
 * NOTE THAT this looks like it does because great pains have been taken
 * to ensure that CAN mappings (address -> data pointer), which might otherwise
 * take up lots of SRAM, are stored statically IN FLASH.
 */

#ifndef LIB_CAN_H
#define LIB_CAN_H
#include <Arduino.h>
#include <addr.h>

#include "can_interface.h"

#define CAN_ADDR_MAX 0xFFFF

namespace can {

bool begin() {
    return can_hal_begin();
}

void loop() {
    if (can_hal_available()) {
        Serial.println("CAN AVAILABLE");
        canmsg_t msg = can_hal_read();

        // Distribute the message to known interfaces until one successfully handles it.
        for (size_t i = 0; i < CANInterface::num_interfaces && !CANInterface::interfaces[i]->try_handle_message(msg); i++)
            ;
    }
}
}  // namespace can

#endif