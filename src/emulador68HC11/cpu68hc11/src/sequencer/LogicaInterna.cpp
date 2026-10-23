/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "LogicaInterna.h"
#include "../uci/Uci.h"
#include "InstructionRegister.h"

void LogicaInterna::reset() {
  mPC = 0;
  Liga = 0;
  RT = 0;
  RI = 0;
  instr_done = false;
}

uint16_t LogicaInterna::esiguiente(uint8_t mi, uint8_t ncc,
                                   InstructionRegister &ir, Uci &uci) {
  instr_done = false;
  switch (mi) {
  case 0x00:
    return mPC;
    break;
  case 0x01:
    if (ncc == 1)
      return Liga;
    else
      return mPC;
    break;
  case 0x02:
    return ir.read();
    break;
  case 0x03:
    instr_done = true;
    if (ncc == 1)
      return uci.readOpcode();
    else
      return mPC;
    break;
  default:
    return mPC;
    break;
  }
}

void LogicaInterna::incrementador(uint16_t epresente) { mPC = epresente + 1; }

void LogicaInterna::setLiga(uint16_t liga) { Liga = liga; }

uint8_t LogicaInterna::readInstDone() const { return instr_done; }
