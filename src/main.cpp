#include <Arduino.h>
#include <can_hal/mcp2515.h>
#include <can_modules/simpletest.h>

void setup() {
    Serial.begin(115200);
    Serial.println("Starting CAN...");
    can::begin();

    simple.transmit();
}

size_t last_send = 0;
size_t last_print = 500;

uint16_t ctr;
void loop() {
    if (millis() - last_send > 1000) {
        Serial.println("SEND");
        canmsg_t m{
            SIMPLE_ADDRESS + 1,
            2,
            {(uint8_t)++ctr, (uint8_t)(ctr << 8)}};

        can_hal_write(&m);

        int32_t nctr = -ctr;
        canmsg_t m2{
            SIMPLE_ADDRESS + 2,
            4,
            {(uint8_t)nctr, (uint8_t)((nctr) << 8), (uint8_t)((nctr) << 16), (uint8_t)((nctr) << 24)}};

        can_hal_write(&m2);

        last_send = millis();
    }

    if (millis() - last_print > 1000) {
        Serial.printf("Current value of simple %d, %d\n", simple.val1, simple.val2);
        last_print = millis();
    }

    can::loop();
}
