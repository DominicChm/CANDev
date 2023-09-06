#include <Arduino.h>
#include "can.h"

struct ecvt_config_data_t {
    struct {
        double kp;
        double ki;
        double kd;
    } primary_pid;

    struct {
        double kp;
        double ki;
        double kd;
    } secondary_pid;
};

namespace ecvt {
ecvt_config_data_t data;

CANMappings mappings(&data, {
    link(data.primary_pid),
    link(data.secondary_pid),
});

CANInterface iface(CAN_ADDR_ECVT_CONFIG_BASE, data, mappings);
}  // namespace ecvt