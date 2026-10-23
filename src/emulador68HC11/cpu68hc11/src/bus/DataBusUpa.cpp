/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "DataBusUpa.h"

void DataBusUpa::clear() {
  value = 0;
  DB = 0;
  driven = false;
}

void DataBusUpa::drive(uint8_t v) {
  if (driven) {
    std::cerr << "ERROR: Data Bus ACC->UPA contention\n";
  }
  DB = v;
  driven = true;
}

uint8_t DataBusUpa::read() const { return DB; }

bool DataBusUpa::isDriven() const { return driven; }
