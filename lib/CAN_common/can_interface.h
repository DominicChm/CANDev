#include <Arduino.h>

#include "can_common.h"
#include "can_hal.h"
#include "can_mapping.h"

#define MAX_INTERFACES 64

/**
 * Links the CAN HAL and mappings, at a specific address
 */
class CANInterface {
    CANMappings& mappings;
    const size_t base_address;
    void* data;

   public:
    // Tracks in-use interfaces.
    // Interfaces add themselves to this array when they are constructed.
    static CANInterface* interfaces[MAX_INTERFACES];
    static size_t num_interfaces;

    template <typename TData>
    CANInterface(const uint32_t base_address, TData& data, CANMappings& mappings) : mappings(mappings), base_address(base_address), data(&data) {
        CANInterface::interfaces[CANInterface::num_interfaces++] = this;  // Register this interface so that messages are pushed to it.
    }

    /**
     * Checks whether an address falls within the range this CANInterface can
     * handle.
     *
     * This range is defined as [BASE ADDRESS, BASE ADDRESS + Number of mappings]
     */
    bool handles_address(uint32_t incoming_address) {
        return incoming_address - base_address < mappings.num_mappings;
    }

    /**
     * Tries to match an incoming CAN frame to an address
     * this CANInterface controls. If a matching address is found,
     * and the size of the local data matches the incoming frame's size,
     * the CAN frame is directly copied to the local representation.
     *
     * Returns true if the frame was handled, false if not.
     */
    bool try_handle_message(canmsg_t msg) {
        if (!handles_address(msg.address)) return false;

        uint32_t offset = msg.address - base_address;

        // if (mappings.size_at_address(offset) != msg.size) return false;
        Serial.println("CAN MESSAGE HANDLED");
        memcpy(mappings.resolve_address_data_ptr(data, offset), msg.data, msg.size);

        return true;
    }

    /**
     * Transmits the current state of the data controlled by this
     * CANInterface onto the CANBus.
     */
    void transmit(bool pack = false) {
        for (size_t i = 0; i < mappings.num_mappings; i++) {
            const can_mapping_t* m = &mappings.mappings[i];

            void* ptr_data = mappings.resolve_mapping_ptr(&data, m);
            canmsg_t msg{
                base_address + i,
                m->size,
            };

            memcpy(&msg.data, ptr_data, m->size);

            can_hal_write(&msg);
        }
    }
};

CANInterface* CANInterface::interfaces[MAX_INTERFACES] = {0};
size_t CANInterface::num_interfaces = 0;