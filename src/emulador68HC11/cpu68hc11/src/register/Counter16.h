/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef COUNTER16_H
#define COUNTER16_H

#include <cstdint>

class DataBus16;    // forward declaration
class AddressBus16; // forward declaration
class Ccr;          // forward declaration

class Counter16 {
public:
  Counter16() = default;

  void reset();

  void outputToBus(DataBus16 &bus, AddressBus16 &abus, Ccr &ccr,
                   uint8_t nenable, uint8_t ctrl, uint8_t counter) const;

  void functn(const DataBus16 &bus, uint8_t ctrl);

  uint16_t read() const;

  bool readZ() const;
  bool readN() const;

  // void set(uint16_t v);

private:
  uint8_t CL = 0;
  uint8_t CH = 0;
  bool Z = true;
  bool N = false;
};

#endif // COUNTER16_H
