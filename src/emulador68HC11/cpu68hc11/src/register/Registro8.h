/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef REGISTRO8_H
#define REGISTRO8_H

#include <cstdint>

class DataBus16;  // forward declaration
class DataBusUpa; // forward declaration
class Ccr;        // forward declaration

class Registro8 {
public:
  Registro8() = default;

  void reset();

  void outputToBus(DataBus16 &bus, DataBusUpa &busupa, Ccr &ccr, uint8_t enable,
                   uint8_t nw, bool acc) const;

  void loadFromBus(const DataBus16 &bus, uint8_t enable, uint8_t nw);

  uint8_t read() const;

  bool readZ() const;
  bool readN() const;

  void set(uint8_t v);

private:
  uint8_t value = 0;
  bool Z = true;
  bool N = false;
  bool ACC = false;
};

#endif // REGISTRO8_H
