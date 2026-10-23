/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef LOGICAINTERNA_H
#define LOGICAINTERNA_H

#include <cstdint>

class InstructionRegister;
class Uci;

class LogicaInterna {
public:
  uint16_t RT = 0;
  uint16_t RI = 0;

  void reset();

  uint16_t esiguiente(uint8_t mi, uint8_t ncc, InstructionRegister &ir,
                      Uci &vector);

  void incrementador(uint16_t epresente);

  void setLiga(uint16_t liga);

  uint8_t readInstDone() const;

private:
  // uint32_t inputs = 0;
  uint16_t mPC = 0;
  uint16_t Liga = 0;
  uint8_t instr_done = false;
};

#endif // LOGICAINTERNA_H
