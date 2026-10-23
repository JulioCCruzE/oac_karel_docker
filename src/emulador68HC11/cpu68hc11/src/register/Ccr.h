/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef CCR_H
#define CCR_H

#include <cstdint>

class DataBus16;      // forward declaration
class DataBusQueries; // forward declaration

class Ccr {
public:
  Ccr() = default;

  void reset();

  void outputToBus(DataBus16 &bus, DataBusQueries &busq, uint8_t nhb) const;

  void loadCCR(const DataBus16 &bus, uint8_t b9, uint8_t b8b7b6, uint8_t b5b4b3,
               uint8_t b2b1, uint8_t b0, uint8_t cc, uint8_t cn, uint8_t cv,
               uint8_t cz, uint8_t ci, uint8_t ch, uint8_t cx, uint8_t cs,
               uint8_t nhb);

  void loadMuxUpa(bool c, bool v, bool z, bool n, bool h);

  void loadMuxAccA(bool z, bool n);

  void loadMuxAccB(bool z, bool n);

  void loadMuxIndex(bool z, bool n);

  void loadMuxAP(bool z, bool n);

  void loadMuxPC(bool z, bool n);

  uint8_t read() const;

  bool getI() const;

  bool getX() const;

private:
  uint8_t value = 0;
  bool C = false;
  bool V = false;
  bool Z = false;
  bool N = false;
  bool I = false;
  bool H = false;
  bool X = false;
  bool S = false;

  enum MuxInput {
    c = 16,
    v = 17,
    z = 18,
    n = 19,
    i = 20,
    h = 21,
    x = 22,
    s = 23
  };

  // For multiplexor
  bool cupa = false;
  bool vupa = false;
  bool zupa = false;
  bool za = false;
  bool zb = false;
  bool zx = false;
  bool zap = false;
  bool zpc = false;
  bool nupa = false;
  bool na = false;
  bool nb = false;
  bool nx = false;
  bool nap = false;
  bool npc = false;
  bool hupa = false;
};

#endif // CCR_H
