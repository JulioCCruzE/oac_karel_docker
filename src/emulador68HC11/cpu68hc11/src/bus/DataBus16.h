/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef DATABUS16_H
#define DATABUS16_H

#include <cstdint>
#include <iostream>

class DataBus16 {
public:
  void clear();

  void driveL(uint8_t v);
  uint16_t readL() const;
  bool isDrivenL() const;

  void driveH(uint8_t v);
  uint16_t readH() const;
  bool isDrivenH() const;

  uint16_t read() const;

private:
  uint16_t value = 0;
  uint8_t DBH = 0;
  uint8_t DBL = 0;
  bool driven = false;
  bool drivenL = false;
  bool drivenH = false;
};

#endif
