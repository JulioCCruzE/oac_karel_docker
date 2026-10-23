/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "InstructionRegister.h"
#include "../bus/DataBus16.h" // ahora sí lo necesitamos

void InstructionRegister::reset() { value = 0; }

// Check if write mode and Read from Internal Data Bus
void InstructionRegister::loadFromBus(const DataBus16 &bus, uint8_t ncri) {
  if (ncri == 1)
    return;

  uint8_t opcode = bus.isDrivenL() ? bus.readL() : 127;

  value = uint16_t(opcode) << 4;
}

uint16_t InstructionRegister::read() const { return value; }

void InstructionRegister::set(uint16_t v) { value = v; }
