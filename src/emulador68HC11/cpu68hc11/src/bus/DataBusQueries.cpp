/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "DataBusQueries.h"

void DataBusQueries::clear() { inputs = 0; }

void DataBusQueries::setInput(int index, bool value) {
  if (value)
    inputs |= (1u << index);
  else
    inputs &= ~(1u << index);
}

uint32_t DataBusQueries::read() const { return inputs; }
