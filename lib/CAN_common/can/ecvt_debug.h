#include <Arduino.h>
#include "can.h"

struct ecvt_debug_data_t {
    uint8_t fsm1state;
    uint8_t fsm2state;
    uint8_t fsm3state;
};

namespace ecvt {
ecvt_debug_data_t data;

CANMappings mappings(&data, {
    link(data.fsm1state),
    link(data.fsm2state),
    link(data.fsm3state),
});

CANInterface iface(CAN_ADDR_ECVT_DEBUG_BASE, data, mappings);
}  // namespace ecvt