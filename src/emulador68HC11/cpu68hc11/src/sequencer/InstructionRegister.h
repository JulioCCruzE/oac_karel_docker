/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef INSTRUCTIONREGISTER_H
#define INSTRUCTIONREGISTER_H

#include <cstdint>

class DataBus16; // forward declaration

class InstructionRegister {
public:
  InstructionRegister() = default;

  void reset();

  void loadFromBus(const DataBus16 &bus, uint8_t ncri);

  uint16_t read() const;

  void set(uint16_t v);

private:
  uint16_t value = 0;
};

#endif // INSTRUCTIONREGISTER_H
