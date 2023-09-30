#include <Arduino.h>

#include <can.h>

#define SIMPLE_ADDRESS 0x0001

// Total state definition of this "simple" module
struct simple_t {
    uint16 val1;
    int32_t val2;
};

namespace dashboard_ns {

// A null pointer, cast to a data* pointer.
// Effectively allows extraction of data offsets using standard pointer syntax
simple_t* mr = CANMappings::map_root<simple_t>();

// Defines relationships between the local data and the CAN bus
CANMappings mappings({
    link(mr->val1),  // CAN address to set data value is ADDRESS + 0x01
    link(mr->val2),  // CAN address to set data value is ADDRESS + 0x02
});
}  // namespace dashboard

// Instantiates the interface and notifies the wizard in my computer that
// this is a module we want to talk to
CANInterface<simple_t> simple(SIMPLE_ADDRESS, dashboard_ns::mappings);
