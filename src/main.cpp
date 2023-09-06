#include <Arduino.h>
#include <can/can_hal_mcp2515.h>
#include <can/ecvt.h>

void setup() {
    Serial.begin(115200);
    Serial.println("Starting CAN...");
    can::begin();

    ecvt::iface.transmit();

    // put your setup code here, to run once:
}

size_t last_send = 0;
size_t last_print = 500;

uint16_t ctr;
void loop() {
    if (millis() - last_send > 1000) {
        Serial.println("SEND");
        canmsg_t m{
            CAN_ADDR_ECVT_BASE,
            2,
            {++ctr, ctr << 8}
        };

        can_hal_write(&m);

        // ecvt::iface.transmit();

        last_send = millis();
    }

    if(millis() - last_print > 1000) {
        Serial.printf("Current value of ECVT %d\n", ecvt::data.enghall);
        last_print = millis();
    }
    can::loop();
}
