#include <Arduino.h>
#include "can.h"

#define ADDRESS 0x0001

// Total state definition of this "simple" module
struct simple {
    float val1;
    double val2;    
};

// Actual storage of data
simple data;

// Defines relationships between the local variable (data)
// and the CAN bus
CANMappings mappings(&data, {
    link(data.val1), // CAN address to set data value is ADDRESS + 0x00
    link(data.val2), // CAN address to set data value is ADDRESS + 0x01
});

// Instantiates the interface and notifies the wizard in my computer that 
// this is a module we want to talk to
CANInterface iface(0, data, mappings);