/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "Index16.h"
#include "../bus/AddressBus16.h"
#include "../bus/DataBus16.h"
#include "../bus/DataBusQueries.h"
#include "Ccr.h"

void Index16::reset() {
  XL = 0;
  XH = 0;
  YL = 0;
  YH = 0;
  Z = true;
  N = false;
  enaRS = false;
}

void Index16::outputToBus(DataBus16 &bus, AddressBus16 &abus,
                          DataBusQueries &busq, Ccr &ccr, uint8_t nenable,
                          uint8_t ctrl) const {

  // drive data bus queries
  busq.setInput(y15, Y15);
  busq.setInput(y0, Y0);
  busq.setInput(x15, X15);
  busq.setInput(x0, X0);

  // drive ccr flags
  ccr.loadMuxIndex(Z, N);

  // drive data bus and address bus
  if (ctrl != 0)
    return;

  uint8_t CH = (enaRS) ? YH : XH;
  uint8_t CL = (enaRS) ? YL : XL;
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
    uint8_t C = 0;
    C |= uint16_t(CH) << 8;
    C |= uint16_t(CL);
    abus.drive(C);
  }
}

void Index16::functn(const DataBus16 &bus, uint8_t ctrl) {
  if (ctrl == 0)
    return;

  uint16_t C = 0;
  uint8_t H = (enaRS) ? YH : XH;
  uint8_t L = (enaRS) ? YL : XL;
  C |= uint16_t(H) << 8;
  C |= uint16_t(L);

  switch (ctrl) {
  case 1:
    C++;
    break;
  case 2:
    C--;
    break;
  case 3:
    L = bus.isDrivenL() ? bus.readL() : 127;
    // C &= 0xFF00;
    C |= uint16_t(L);
    break;
  case 4:
    H = bus.isDrivenH() ? bus.readH() : 127;
    // C &= 0xFF00;
    C |= uint16_t(H) << 8;
    break;
  case 5:
    L = bus.isDrivenL() ? bus.readL() : 127;
    H = bus.isDrivenH() ? bus.readH() : 127;
    C = 0;
    C |= uint16_t(H) << 8;
    C |= uint16_t(L);
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

  if (!enaRS) {
    XH = (C >> 8) & 0xFF;
    XL = (C >> 0) & 0xFF; // TODO: try without despl
  } else {
    YH = (C >> 8) & 0xFF;
    YL = (C >> 0) & 0xFF;
  }

  // get Y15, Y0, X15, X0
  Y15 = (YH & 0x80) != 0;
  Y0 = (YL & 0x01) != 0;
  X15 = (XH & 0x80) != 0;
  X0 = (XL & 0x01) != 0;

  Z = (C == 0) ? true : false;
  N = (C & 0x8000) != 0;
}

void Index16::indexRS(uint8_t enay, uint8_t instr_done) {
  if (enay != instr_done)
    enaRS = (enay == 1);
}

uint16_t Index16::read(uint8_t enay) const {
  uint16_t r = 0;
  if (enay == 0) {
    r |= uint16_t(XH) << 8;
    r |= uint16_t(XL);
  } else {
    r |= uint16_t(YH) << 8;
    r |= uint16_t(YL);
  }
  return r;
}

bool Index16::readZ() const { return Z; }

bool Index16::readN() const { return N; }
