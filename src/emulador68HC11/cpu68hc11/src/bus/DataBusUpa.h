/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef DATABUSUPA_H
#define DATABUSUPA_H

#include <cstdint>
#include <iostream>

class DataBusUpa {
public:
  void clear();
  void drive(uint8_t v);
  uint8_t read() const;
  bool isDriven() const;

private:
  uint16_t value = 0;
  uint8_t DB = 0;
  bool driven = false;
};

#endif
