/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "AddressBus16.h"

void AddressBus16::clear() {
  value = 0;
  driven = false;
}

void AddressBus16::drive(uint16_t v) {
  if (driven) {
    std::cerr << "ERROR: Address Bus contention\n";
  }
  value = v;
  driven = true;
}

uint16_t AddressBus16::read() const { return value; }

bool AddressBus16::isDriven() const { return driven; }
