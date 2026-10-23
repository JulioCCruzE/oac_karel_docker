/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "AccessRegister.h"
#include "../bus/AddressBus16.h"

void AccessRegister::reset() { address = 0; }

void AccessRegister::loadFromBus(const AddressBus16 &abus, uint8_t ncbd) {
  if (ncbd == 1)
    return;

  address = abus.isDriven() ? abus.read() : 0xFFFF;
}

uint16_t AccessRegister::read() const { return address; }
