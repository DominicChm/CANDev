#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H

#include <Arduino.h>

#include "can_common.h"
#include "can_hal.h"
#include "can_mapping.h"

#define MAX_INTERFACES 32

class CANInterfaceBase {
   public:
    // Interfaces add themselves to this array when they are constructed.
    inline static CANInterfaceBase* interfaces[MAX_INTERFACES] = {};
    inline static size_t num_interfaces = 0;

    void* ptr_data_root;
    CANMappings& mappings;
    const size_t base_address;

    CANInterfaceBase(const uint32_t base_address, CANMappings& mappings, void* data) : ptr_data_root(data), mappings(mappings), base_address(base_address) {
        CANInterfaceBase::interfaces[CANInterfaceBase::num_interfaces++] = this;  // Register this interface so that messages are pushed to it.
    }

    /**
     * Checks whether an address falls within the range this CANInterface can
     * handle.
     *
     * This range is defined as [BASE ADDRESS, BASE ADDRESS + Number of mappings]
     */
    bool handles_address(uint32_t incoming_address) {
        return (incoming_address - base_address - 1) < mappings.num_mappings;
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

        int32_t offset = msg.address - base_address - 1;

        if(offset < 0) {
            // SPECIAL CASE
            return true;
        }

        // if (mappings.address_offset_to_size(offset) != msg.size) return false;
        Serial.println("CAN MESSAGE HANDLED");
        void* data_target = mappings.address_offset_to_ptr(ptr_data_root, offset);
        if(!data_target) {
            Serial.println("COULDN'T RESOLVE MAPPING!");
            return true;
        } 
        memcpy(data_target, msg.data, msg.size);

        return true;
    }

   public:
    static bool distribute(canmsg_t& msg) {
        // Distribute the message to known interfaces until one successfully handles it.
        for (size_t i = 0; i < num_interfaces; i++) {
            if (interfaces[i]->try_handle_message(msg)) return true;
        }

        return false;
    }
};

/**
 * Links the CAN HAL, mappings, and data together using CANInterfaceBase.
 */
template <typename T>
class CANInterface : public T {
   private:
    CANInterfaceBase base;

   public:
    // https://stackoverflow.com/questions/4066233/pointer-to-base-class
    CANInterface(const uint32_t base_address, CANMappings& mappings) : base(CANInterfaceBase(base_address, mappings, dynamic_cast<T*>(this))) {
    }

    /**
     * Transmits the current state of the data controlled by this
     * CANInterface onto the CANBus.
     */
    void transmit(bool pack = false) {
        for (size_t i = 0; i < base.mappings.num_mappings; i++) {
            const can_mapping_t* m = &base.mappings.mappings[i];

            void* ptr_data = base.mappings.mapping_to_ptr(base.ptr_data_root, m);
            canmsg_t msg{
                base.base_address + i,
                m->size,
            };

            memcpy(&msg.data, ptr_data, m->size);

            can_hal_write(&msg);
        }
    }
};

#endif