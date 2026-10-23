/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "Uci.h"
#include "../bus/DataBus16.h"
#include "../bus/DataBusQueries.h"

void Uci::reset() {
  INT = false;
  opcode = 0x000;
  vectorAddr = 0x0000;
}

void Uci::outputToBus(DataBus16 &bus, DataBusQueries &busq,
                      uint8_t interrupt) const {
  busq.setInput(qint, INT);

  if (interrupt != 1)
    return;

  bus.driveL(uint8_t(vectorAddr));
  bus.driveH(uint8_t(vectorAddr >> 8));
}

void Uci::handleInt(uint8_t nreset, uint8_t xccr, uint8_t iccr) {
  if (!enableRS) {
    INT = false;
    return;
  }

  INT = (nreset == 0 || !xRS || !iRS);

  if (nreset == 0) {
    opcode = opcodeROM[0];
    vectorAddr = vectorROM[0];
  } else if (!xRS && xccr == 0) {
    opcode = opcodeROM[1];
    vectorAddr = vectorROM[1];
  } else if (!iRS && iccr == 0) {
    opcode = opcodeROM[2];
    vectorAddr = vectorROM[2];
  } else {
    INT = false;
    opcode = opcodeROM[0];
    vectorAddr = vectorROM[0];
  }
}

void Uci::enableInt(uint8_t dint, uint8_t hint) {
  if (dint != hint)
    enableRS = (hint == 1);
}

void Uci::enableXI(uint8_t set_xirq, uint8_t nxirq, uint8_t set_irq,
                   uint8_t nirq) {
  if (set_xirq == nxirq)
    xRS = (nxirq == 1);

  if (set_irq == nirq)
    iRS = (nirq == 1);
}

uint16_t Uci::readOpcode() { return opcode; }
