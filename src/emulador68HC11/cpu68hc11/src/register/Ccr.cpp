/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "Ccr.h"
#include "../bus/DataBus16.h" // ahora sí lo necesitamos
#include "../bus/DataBusQueries.h"

void Ccr::reset() { value = 0; }

// Check if read mode and Load to Internal Data Bus and Query Bus
void Ccr::outputToBus(DataBus16 &bus, DataBusQueries &busq, uint8_t nhb) const {

  busq.setInput(c, C);
  busq.setInput(v, V);
  busq.setInput(n, N);
  busq.setInput(z, Z);
  busq.setInput(i, I);
  busq.setInput(h, H);
  busq.setInput(x, X);
  busq.setInput(s, S);

  if (nhb != 0)
    return;

  bus.driveL(value);
}

// Check if Read mode and update flags
void Ccr::loadCCR(const DataBus16 &bus, uint8_t b9, uint8_t b8b7b6,
                  uint8_t b5b4b3, uint8_t b2b1, uint8_t b0, uint8_t cc,
                  uint8_t cn, uint8_t cv, uint8_t cz, uint8_t ci, uint8_t ch,
                  uint8_t cx, uint8_t cs, uint8_t nhb) {
  if (nhb == 0)
    return;

  uint8_t data = bus.isDrivenL() ? bus.readL() : 127;
  bool d0 = (data && 0x01) != 0;
  bool d1 = (data && 0x02) != 0;
  bool d2 = (data && 0x04) != 0;
  bool d3 = (data && 0x08) != 0;
  bool d4 = (data && 0x10) != 0;
  bool d5 = (data && 0x20) != 0;
  bool d6 = (data && 0x40) != 0;
  bool d7 = (data && 0x80) != 0;

  if (cc == 1)
    C = (b0 == 0) ? cupa : d0;

  if (cv == 1) {
    switch (b2b1) {
    case 0:
      V = vupa;
      break;
    case 1:
      V = d1;
      break;
    case 2:
      V = 0;
      break;
    default:
      break;
    }
  }

  if (cz == 1) {
    switch (b5b4b3) {
    case 0:
      Z = zupa;
      break;
    case 1:
      Z = za;
      break;
    case 2:
      Z = zb;
      break;
    case 3:
      Z = zx;
      break;
    case 4:
      Z = zap;
      break;
    case 5:
      Z = zpc;
      break;
    case 6:
      Z = d2;
      break;
    default:
      break;
    }
  }

  if (cn == 1) {
    switch (b8b7b6) {
    case 0:
      N = nupa;
      break;
    case 1:
      N = na;
      break;
    case 2:
      N = nb;
      break;
    case 3:
      N = nx;
      break;
    case 4:
      N = nap;
      break;
    case 5:
      N = npc;
      break;
    case 6:
      N = d3;
      break;
    default:
      break;
    }
  }

  if (ci == 1)
    I = d4;

  if (ch == 1)
    C = (b9 == 0) ? hupa : d5;

  if (cx == 1)
    X = d6;

  if (cs == 1)
    S = d7;

  // TODO: test this assigments
  value = C ? value | 0x01 : value & 0xFE;
  value = V ? value | 0x02 : value & 0xFD;
  value = Z ? value | 0x04 : value & 0xFB;
  value = N ? value | 0x08 : value & 0xF7;
  value = I ? value | 0x10 : value & 0xEF;
  value = H ? value | 0x20 : value & 0xDF;
  value = X ? value | 0x40 : value & 0xBF;
  value = S ? value | 0x80 : value & 0x7F;
}

void Ccr::loadMuxUpa(bool c, bool v, bool z, bool n, bool h) {
  cupa = c;
  vupa = v;
  zupa = z;
  nupa = n;
  hupa = h;
}

void Ccr::loadMuxAccA(bool z, bool n) {
  za = z;
  na = n;
}

void Ccr::loadMuxAccB(bool z, bool n) {
  zb = z;
  nb = n;
}

void Ccr::loadMuxIndex(bool z, bool n) {
  zx = z;
  nx = n;
}

void Ccr::loadMuxAP(bool z, bool n) {
  zap = z;
  nap = n;
}

void Ccr::loadMuxPC(bool z, bool n) {
  zpc = z;
  npc = n;
}

uint8_t Ccr::read() const { return value; }

bool Ccr::getI() const { return I; }

bool Ccr::getX() const { return X; }
