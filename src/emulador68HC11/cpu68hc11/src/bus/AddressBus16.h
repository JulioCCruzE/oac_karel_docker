/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef ADDRESSBUS16_H
#define ADDRESSBUS16_H

#include <cstdint>
#include <iostream>

class AddressBus16 {
public:
  void clear();

  void drive(uint16_t v);
  uint16_t read() const;
  bool isDriven() const;

private:
  uint16_t value = 0;
  bool driven = false;
};

#endif
