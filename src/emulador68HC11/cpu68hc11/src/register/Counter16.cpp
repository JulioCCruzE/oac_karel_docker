/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "Counter16.h"
#include "../bus/AddressBus16.h"
#include "../bus/DataBus16.h"
#include "Ccr.h"

void Counter16::reset() {
  CL = 0;
  CH = 0;
  Z = true;
  N = false;
}

void Counter16::outputToBus(DataBus16 &bus, AddressBus16 &abus, Ccr &ccr,
                            uint8_t nenable, uint8_t ctrl,
                            uint8_t counter) const {
  if (counter == 2)
    ccr.loadMuxAP(Z, N);
  else if (counter == 1)
    ccr.loadMuxPC(Z, N);

  if (ctrl != 0)
    return;

  // uint8_t CH = (enay == 0) ? XH : YH;
  bool e0 = (nenable & 0x01) != 0;
  bool e1 = (nenable & 0x02) != 0;
  bool e2 = (nenable & 0x04) != 0;

  if (!e0) {
    bus.driveL(CL);
  }
  if (!e1) {
    bus.driveH(CH);
  }
  if (!e2) {
    uint16_t C = 0;
    C |= uint16_t(CH) << 8;
    C |= uint16_t(CL);
    abus.drive(C);
  }
}

void Counter16::functn(const DataBus16 &bus, uint8_t ctrl) {
  if (ctrl == 0)
    return;

  uint16_t C = 0;
  // uint8_t H = (enay == 0) ? XH : YH;
  // uint8_t L = (enay == 0) ? XL : YL;
  C |= uint16_t(CH) << 8;
  C |= uint16_t(CL);

  switch (ctrl) {
  case 1:
    C++;
    break;
  case 2:
    C--;
    break;
  case 3:
    CL = bus.isDrivenL() ? bus.readL() : 127;
    C &= 0xFF00;
    C |= uint16_t(CL);
    break;
  case 4:
    CH = bus.isDrivenH() ? bus.readH() : 127;
    C &= 0x00FF;
    C |= uint16_t(CH) << 8;
    break;
  case 5:
    CL = bus.isDrivenL() ? bus.readL() : 127;
    CH = bus.isDrivenH() ? bus.readH() : 127;
    C = 0;
    C |= uint16_t(CH) << 8;
    C |= uint16_t(CL);
    break;
  case 6:
    C = C << 1;
    break;
  case 7:
    C = C >> 1;
    break;
  default:
    break;
  }

  CH = (C >> 8) & 0xFF;
  CL = (C >> 0) & 0xFF; // TODO: try without despl

  Z = (C == 0) ? true : false;
  N = (C & 0x8000) != 0;
}

uint16_t Counter16::read() const {
  uint16_t r = 0;
  r |= uint16_t(CH) << 8;
  r |= uint16_t(CL);
  return r;
}

bool Counter16::readZ() const { return Z; }

bool Counter16::readN() const { return N; }
