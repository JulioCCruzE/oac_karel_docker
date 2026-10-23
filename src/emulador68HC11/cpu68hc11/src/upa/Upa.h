/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef UPA_H
#define UPA_H

#include <cstdint>

class DataBus16;      // forward declaration
class DataBusUpa;     // forward declaration
class DataBusQueries; // forward declaration
class Ccr;            // forward declaration

class Upa {
public:
  Upa() = default;

  void reset();

  void outputToBus(DataBus16 &bus, DataBusQueries &busq, Ccr &ccr,
                   uint8_t noeupa) const;

  // void loadFromBus(const DataBus16 &bus, uint8_t enable, uint8_t nw);

  void source(DataBus16 &bus, DataBusUpa &busa, DataBusUpa &busb,
              uint8_t selbus, uint8_t source);

  void functn(uint8_t functn, uint8_t selmux, uint8_t accsec);

  void dest(DataBusUpa &busa, DataBusUpa &busb, uint8_t dest, uint8_t ndupa);

  // uint8_t read() const;

  bool getC() const;
  bool getV() const;
  bool getZ() const;
  bool getN() const;
  bool getH() const;

  uint8_t getY() const;
  uint8_t getQ() const;
  uint8_t getCounter() const;
  uint8_t getR() const;
  uint8_t getS() const;
  uint8_t getF() const;
  uint8_t getOp() const;
  uint8_t getCin() const;

  // void set(uint8_t v);

private:
  uint8_t Y = 0;
  uint8_t Q = 0;
  uint8_t contador = 0;
  uint8_t R = 0;
  uint8_t S = 0;
  uint8_t F = 0;
  bool FC = false;
  bool Q0 = false;
  bool Q7 = false;
  bool Y7 = false;
  bool Y0 = false;
  bool C = false;
  bool V = false;
  bool Z = false;
  bool N = false;
  bool H = false;
  enum MuxInput { q7 = 0, q0 = 1, y7 = 2, y0 = 3, fc = 14 };
  uint8_t lastOp = 0;
  bool cin = false;
};

#endif // UPA_H
