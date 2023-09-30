#pragma once

// https://github.com/autowp/arduino-mcp2515
#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>

#include "can.h"

#ifndef CAN_HAL_CS
#define CAN_HAL_CS D8
#endif

#ifndef CAN_HAL_IRQ
#define CAN_HAL_IRQ D1
#endif

MCP2515 mcp(CAN_HAL_CS);

volatile bool flag_can_irq = false;
can_frame frame;
uint8_t irqs;

IRAM_ATTR void can_irq() {
    flag_can_irq = true;
}

canmsg_t convert_frame() {
    canmsg_t m = {
        frame.can_id,
        frame.can_dlc,
    };

    memcpy(&m.data, &frame.data, 8);

    return m;
}

bool can_hal_begin() {
    mcp.reset();
    mcp.setBitrate(CAN_125KBPS);

    attachInterrupt(digitalPinToInterrupt(CAN_HAL_IRQ), can_irq, FALLING);

    mcp.setLoopbackMode();
    return true;
}

void update_irqs() {
    if (flag_can_irq) {
        irqs = mcp.getInterrupts();
    }
}

inline int bto1(int mask, int val) {
    return val & mask ? 1 : 0;
}

int can_hal_available() {
    update_irqs();
    return bto1(MCP2515::CANINTF_RX0IF, irqs) + bto1(MCP2515::CANINTF_RX1IF, irqs);
}

int can_hal_write_available() {
    update_irqs();
    return bto1(MCP2515::CANINTF_TX0IF, irqs) + bto1(MCP2515::CANINTF_TX1IF, irqs) + bto1(MCP2515::CANINTF_TX2IF, irqs);
}

canmsg_t can_hal_read() {
    if (!flag_can_irq) return canmsg_t{};
    flag_can_irq = false;

    irqs = mcp.getInterrupts();

    if (irqs & MCP2515::CANINTF_RX0IF) {
        irqs &= ~MCP2515::CANINTF_RX0IF;

        if (mcp.readMessage(MCP2515::RXB0, &frame) == MCP2515::ERROR_OK)
            return convert_frame();
    }

    if (irqs & MCP2515::CANINTF_RX1IF) {
        irqs &= ~MCP2515::CANINTF_RX1IF;

        if (mcp.readMessage(MCP2515::RXB1, &frame) == MCP2515::ERROR_OK)
            return convert_frame();
    }

    return canmsg_t{};
}

bool can_hal_write(canmsg_t *msg) {
    can_frame frame{
        msg->address,
        (uint8_t)msg->size};

    memcpy(&frame.data, &msg->data, 8);

    mcp.sendMessage(&frame);
}