#include <Arduino.h>
#include "can.h"

// Possibly contained in external headers or smth IDK.
struct ecvt_data_t {
    uint16_t enghall;
    uint16_t rearhall;

    struct {
        double primary;
        double secondary;
    } load_cell;

    struct {
        int32_t primary;
        int32_t secondary;
    } mot_pos;

    uint8_t arr[10];
};

namespace ecvt {
ecvt_data_t data;

CANMappings mappings(&data, {
    link(data.enghall),
    link(data.rearhall),

    // You can link to an entire sub-struct...
    link(data.load_cell),

    // Or
    link(data.mot_pos.primary),
    link(data.mot_pos.secondary),

    link(data.arr[0]),
    link(data.arr[1]),
    link(data.arr[2]),
    link(data.arr[3]),
    link(data.arr[4]),
});

CANInterface iface(CAN_ADDR_ECVT_BASE, data, mappings);


// If we want to, we can create a second ECVT interface at 
// another address by simply allocating another ECVT data struct.
// The mappings are reusable (yes even though they technically point into the other struct)
ecvt_data_t data2;
CANInterface iface2(0x0500, data2, mappings);

}  // namespace ecvt