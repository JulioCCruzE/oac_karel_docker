/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "Registro8.h"
#include "../bus/DataBus16.h" // ahora sí lo necesitamos
#include "../bus/DataBusUpa.h"
#include "../register/Ccr.h"

void Registro8::reset() { value = 0; }

// Check if read mode and Load to Internal Data Bus or UPA
void Registro8::outputToBus(DataBus16 &bus, DataBusUpa &busupa, Ccr &ccr,
                            uint8_t enable, uint8_t nw, bool acc) const {
  if (acc)
    ccr.loadMuxAccB(Z, N);
  else
    ccr.loadMuxAccA(Z, N);

  if (nw == 0)
    return;
  switch (enable) {
  case 1:
    bus.driveL(value);
    break;
  case 2:
    bus.driveH(value);
    break;
  case 3:
    busupa.drive(value);
  default:
    break;
  }
}

// Check if write mode and Read from Internal Data Bus
void Registro8::loadFromBus(const DataBus16 &bus, uint8_t enable, uint8_t nw) {
  if (nw == 1)
    return;

  switch (enable) {
  case 1:
    value = bus.readL();
    break;
  case 2:
    value = bus.readH();
  default:
    break;
  }

  Z = (value == 0);
  N = (value > 127);
}

uint8_t Registro8::read() const { return value; }

bool Registro8::readZ() const { return Z; }

bool Registro8::readN() const { return N; }

void Registro8::set(uint8_t v) { value = v; }
