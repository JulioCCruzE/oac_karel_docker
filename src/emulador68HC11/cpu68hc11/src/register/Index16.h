/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef INDEX16_H
#define INDEX16_H

#include <cstdint>

class DataBus16;      // forward declaration
class AddressBus16;   // forward declaration
class DataBusQueries; // forward declaration
class Ccr;            // forward declaration

class Index16 {
public:
  Index16() = default;

  void reset();

  void outputToBus(DataBus16 &bus, AddressBus16 &abus, DataBusQueries &busq,
                   Ccr &ccr, uint8_t nenable, uint8_t ctrl) const;

  void functn(const DataBus16 &bus, uint8_t ctrl);

  void indexRS(uint8_t enay, uint8_t instr_done);

  uint16_t read(uint8_t enay) const;

  bool readZ() const;
  bool readN() const;

  // void set(uint16_t v);

private:
  uint8_t XL = 0;
  uint8_t XH = 0;
  uint8_t YL = 0;
  uint8_t YH = 0;
  bool Y15 = false;
  bool Y0 = false;
  bool X15 = false;
  bool X0 = false;
  bool Z = true;
  bool N = false;
  enum MuxInput { y15 = 4, y0 = 5, x15 = 6, x0 = 7 };
  bool enaRS = false;
};

#endif // INDEX16_H
