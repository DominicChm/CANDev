#include <Arduino.h>
#include <can/can_hal_mcp2515.h>
#include <can/dashboard.h>

void setup() {
    Serial.begin(115200);
    Serial.println("Starting CAN...");
    can::begin();

    iface.transmit();

    // put your setup code here, to run once:
}


size_t last_send = 0;
size_t last_print = 500;

uint16_t ctr;
void loop() {
    if (millis() - last_send > 1000) {
        Serial.println("SEND");
        canmsg_t m{
            1,
            4,
            {++ctr, ctr << 8}
        };

        can_hal_write(&m);

        // ecvt::iface.transmit();

        last_send = millis();
    }

    if(millis() - last_print > 1000) {
        Serial.printf("Current value of val1 %d\n", data.val1);
        last_print = millis();
    }
    can::loop();
}

